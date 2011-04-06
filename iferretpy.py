#!/usr/bin/env python
# © 2011 Massachusetts Institute of Technology

import sys
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

class op_arr_t(Structure):
    _fields_ = [
        ("num", c_ulong),
        ("max", c_ulong),
        ("_ops", POINTER(iferret_op_t)),
    ]

    def __getitem__(self, i):
        if i < 0: i = self.num + i
        if not 0 <= i < self.num: raise IndexError(i)

        return self._ops[i]

    def __setitem__(self, i, v):
        #print "Setting", i, "to", `v`
        iferret.op_arr_clear(byref(self), i, 1)

    def __setslice__(self, i, j, seq):
        #print "Setting %d-%d to %s" % (i,j,seq)
        n = len(seq)

        if j - i < n:
            iferret.op_arr_movedown(byref(self), j, n-(j-i))

        iferret.op_arr_clear(byref(self), i, n)

        if j - i > n:
            iferret.op_arr_moveup(byref(self), j, (j - i) - n)

    def __delitem__(self, i):
        del self[i:i+1]    

    def __delslice__(self, i, j):
        if i < 0: i = self.num + i
        if j > self.num: j = self.num
        if not 0 <= i < self.num: raise IndexError(i)
        #print "Deleting from %d-%d" % (i,j)

        iferret.op_arr_moveup(byref(self), j, j-i)

    def __len__(self):
        return self.num

    def optimize(self):
        iferret.op_arr_fit(byref(self))

    def find_interrupts(self):
        ints = []
        a, b = c_int(), c_int()

        ret = iferret.op_arr_find_interrupt(byref(self), 0, pointer(a), pointer(b))
        while ret == 1:
            ints.append( (a.value, b.value) )
            ret = iferret.op_arr_find_interrupt(byref(self), b, pointer(a), pointer(b))

        if ret == -1:
            raise ValueError("Unbalanced interrupts")
        else:
            return ints

    def find_inputs(self, addr):
        ins = []
        t = c_int()
        ret = iferret.op_arr_find_input(byref(self), 0, addr, pointer(t))
        while ret != -1:
            ins.append( ("T%d" % t.value, ret) )
            ret = iferret.op_arr_find_input(byref(self), ret+1, addr, pointer(t))
        return ins

class py_op_arr:
    def __init__(self, trace):
        self.trace = trace
        self.shadow = [0]*len(trace)
    def __getitem__(self, i):
        if isinstance(i, slice):
            start, stop, step = i.start, i.stop, i.step
            if start is None: start = 0
            if stop is None: stop = len(self)
            if step is None: step = 1
            return [self[j] for j in range(start, stop, step)]

        e = self.trace[i]
        if e.is_valid:
            return e
        else:
            return self.shadow[i]
    def __setitem__(self, i, v):
        self.trace[i] = v
        self.shadow[i] = v
    def __delitem__(self, i):
        del self[i:i+1]    
    def __setslice__(self, i, j, seq):
        self.trace[i:j] = seq
        self.shadow[i:j] = seq
    def __delslice__(self, i, j):
        del self.trace[i:j]
        del self.shadow[i:j]
    def __len__(self):
        return len(self.trace)
    def optimize(self):
        self.trace.optimize()
    def find_interrupts(self):
        return self.trace.find_interrupts()
    def find_inputs(self, addr):
        return self.trace.find_inputs(addr)
    def __hash__(self):
        return id(self)

def load_trace(base, start=0, num=1):
    iferret.init.restype = POINTER(op_arr_t)
    oa = iferret.init(base, start, num)
    #oa = op_arr_t.in_dll(iferret, "op_arr")
    #trace = ImmutableList(oa.ops)
    trace = py_op_arr(oa.contents)
    return trace

if __name__ == "__main__":
    trace = load_trace(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))
    import IPython
    embedshell = IPython.Shell.IPShellEmbed(argv=[])
    embedshell()
