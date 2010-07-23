#!/usr/bin/env python

import sys
import IPython
from immutablelist import ImmutableList
from iferret_ops import iferret_log_op_enum_r
from ctypes import *

IFLAT_NONE = ord('0')
IFLAT_UI8 = ord('1')
IFLAT_UI16 = ord('2')
IFLAT_UI32 = ord('4')
IFLAT_UI64 = ord('8')
IFLAT_STR = ord('s')

OP_IN_SLICE  = 0x1
OP_IS_OUTPUT = 0x2

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
    def __iter__(self):
        class It(object):
            def __init__(sub):
                sub.i = 0
            def next(sub):
                n = sub.i
                if n >= self.n:
                    raise StopIteration()
                sub.i += 1
                return self.cptr[n]
            def __iter__(sub):
                return sub
        return It()
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
        return "%s(%s)" % (self.op, ",".join(repr(x) for x in CArray(self._args, self.num_args)))
    def __repr__(self):
        return str(self)

class op_arr_t(Structure):
    _fields_ = [
        ("num", c_ulong),
        ("max", c_ulong),
        ("_ops", POINTER(iferret_op_t)),
    ]
    
    @property
    def ops(self):
        return CArray(self._ops, self.num)

def load_trace(base, start=0, num=1):
    iferret = cdll.LoadLibrary("./iferret.so")
    iferret.init(base, start, num)
    oa = op_arr_t.in_dll(iferret, "op_arr")
    trace = ImmutableList(oa.ops)
    return trace

if __name__ == "__main__":
    trace = load_trace(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))
    embedshell = IPython.Shell.IPShellEmbed(argv=[])
    embedshell()
