#!/usr/bin/env python

import sys
import IPython
from immutablelist import ImmutableList
from iferret_ops import iferret_log_op_enum_r
from ctypes import *

iferret = cdll.LoadLibrary("./iferret.so")

IFLAT_NONE = ord('0')
IFLAT_UI8 = ord('1')
IFLAT_UI16 = ord('2')
IFLAT_UI32 = ord('4')
IFLAT_UI64 = ord('8')
IFLAT_STR = ord('s')

OP_IS_VALID  = 0x1
OP_IN_SLICE  = 0x2
OP_IS_OUTPUT = 0x4 

class CArray(object):
    def __init__(self, cptr, n):
        self.cptr = cptr
        self.n = n
    def __len__(self):
        return self.n
    def __getitem__(self, i):
        if 0 <= i < self.n:
            return self.cptr[i]
        else:
            raise IndexError(i)
    def __getslice__(self,start,stop):
        return [self[i] for i in range(start,stop)]
    def __str__(self):
        return str(list(self))
    def __repr__(self):
        return repr(list(self))

class ValUnion(Union):
    _fields_ = [
        ("u8", c_ubyte),
        ("u16", c_ushort),
        ("u32", c_uint),
        ("u64", c_ulong),
        ("str", c_char_p),
    ]

class iferret_op_arg_t(Structure):
    _fields_ = [
        ("typ", c_uint),
        ("_val", ValUnion),
    ]

    @property
    def val(self):
        if self.typ == IFLAT_UI8:
            return self._val.u8
        elif self.typ == IFLAT_UI16:
            return self._val.u16
        elif self.typ == IFLAT_UI32:
            return self._val.u32
        elif self.typ == IFLAT_UI64:
            return self._val.u64
        elif self.typ == IFLAT_STR:
            return self._val.str

    def __str__(self):
        if self.typ == IFLAT_STR:
            return self.val
        else:
            return "%#x" % self.val

    def __repr__(self):
        if self.typ == IFLAT_STR:
            return "'" + self.val + "'"
        else:
            return "%#x" % self.val

class iferret_op_t(Structure):
    _fields_ = [
        ("num", c_uint),
        ("_args", POINTER(iferret_op_arg_t)),
        ("num_args", c_uint),
        ("flags", c_uint),
        ("syscall", c_void_p),
    ]

    @property
    def in_slice(self):
        return bool(self.flags & OP_IN_SLICE)

    @in_slice.setter
    def in_slice(self, value):
        if value:
            self.flags |= OP_IN_SLICE
        else:
            self.flags &= ~OP_IN_SLICE

    @property
    def is_valid(self):
        return bool(self.flags & OP_IS_VALID)

    @is_valid.setter
    def is_valid(self, value):
        if value:
            self.flags |= OP_IS_VALID
        else:
            self.flags &= ~OP_IS_VALID

    @property
    def is_output(self):
        return bool(self.flags & OP_IS_OUTPUT)

    @is_output.setter
    def is_output(self, value):
        if value:
            self.flags |= OP_IS_OUTPUT
        else:
            self.flags &= ~OP_IS_OUTPUT

    @property
    def args(self):
        return [a.val for a in CArray(self._args, self.num_args)]

    @property
    def op(self):
        return iferret_log_op_enum_r[self.num]

    def mark(self):
        self.in_slice = True
    
    def set_output_label(self, label):
        self.is_output = True

    def __str__(self):
        raise RuntimeError("oh no you di'int")
        return repr(self)

    def __repr__(self):
        return "%s(%s)" % (self.op, ",".join(repr(x) for x in CArray(self._args, self.num_args)))

shadow = {}

class op_arr_t(Structure):
    _fields_ = [
        ("num", c_ulong),
        ("max", c_ulong),
        ("_ops", POINTER(iferret_op_t)),
    ]

    def __getitem__(self, i):
        if isinstance(i, slice):
            start, stop, step = i.start, i.stop, i.step
            if start is None: start = 0
            if stop is None: stop = len(self)
            if step is None: step = 1
            return [self[j] for j in range(start, stop, step)]
        if i < 0: i = self.num + i
        if not 0 <= i < self.num: raise IndexError(i)

        e = self._ops[i]
        return e if e.is_valid else shadow[i]

    def __setitem__(self, i, v):
        #print "Setting", i, "to", `v`
        iferret.op_arr_clear(i, 1)
        shadow[i] = v

    def __setslice__(self, i, j, seq):
        #print "Setting %d-%d to %s" % (i,j,seq)
        n = len(seq)

        if j - i < n:
            #print "Moving array down by %d starting at %d" % (n-(j-i),j)
            iferret.op_arr_movedown(j, n-(j-i))

        iferret.op_arr_clear(i, n)

        if j - i > n:
            #print "Moving array up by %d starting at %d" % ((j-i)-n,j)
            iferret.op_arr_moveup(j, (j - i) - n)

        # Move the shadow entries
        #print "Adjusting shadow..."
        self._shad_move(j, n - (j - i))
        
        for x,v in enumerate(seq, start=i):
            shadow[x] = v

    def __delitem__(self, i):
        del self[i:i+1]    

    def _shad_move(self, i, n):
        if n == 0: return
        updates = []
        for k in shadow.keys():
            if k >= i:
                updates.append( ((k+n), shadow[k]) )
                del shadow[k]
        shadow.update(updates)

    def __delslice__(self, i, j):
        if i < 0: i = self.num + i
        if j > self.num: j = self.num
        if not 0 <= i < self.num: raise IndexError(i)
        #print "Deleting from %d-%d" % (i,j)

        #print "Moving array up by %d starting at %d" % (j-i, j)
        iferret.op_arr_moveup(j, j-i)
        
        for x in range(i, j):
            if x in shadow: del shadow[x]

        self._shad_move(i, -(j-i))

    def __len__(self):
        return self.num

    def optimize(self):
        iferret.op_arr_fit()

    def find_interrupts(self):
        ints = []
        a, b = c_int(), c_int()

        ret = iferret.op_arr_find_interrupt(0, pointer(a), pointer(b))
        while ret == 1:
            ints.append( (a.value, b.value) )
            ret = iferret.op_arr_find_interrupt(b, pointer(a), pointer(b))

        if ret == -1:
            raise ValueError("Unbalanced interrupts")
        else:
            return ints

    def find_inputs(self, addr):
        ins = []
        t = c_int()
        ret = iferret.op_arr_find_input(0, addr, pointer(t))
        while ret != -1:
            ins.append( ("T%d" % t.value, ret) )
            ret = iferret.op_arr_find_input(ret+1, addr, pointer(t))
        return ins

def load_trace(base, start=0, num=1):
    iferret.init(base, start, num)
    oa = op_arr_t.in_dll(iferret, "op_arr")
    #trace = ImmutableList(oa.ops)
    return oa

if __name__ == "__main__":
    trace = load_trace(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))
    embedshell = IPython.Shell.IPShellEmbed(argv=[])
    embedshell()
