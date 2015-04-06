#!/usr/bin/env python
# -*- coding: utf-8 -*-
# ©2011 Massachusetts Institute of Technology

from cPickle import load
from distorm import Decode, Decode32Bits
import sys

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

    def disable(self):
        self.HEADER = ''
        self.OKBLUE = ''
        self.OKGREEN = ''
        self.WARNING = ''
        self.FAIL = ''
        self.ENDC = ''

trace = load(open(sys.argv[1]))
for i, t in trace:
    s = repr(t)
    if t.op == 'IFLO_TB_HEAD_EIP':
        color = bcolors.WARNING
    elif t.op == 'IFLO_INSN_BYTES':
        color = bcolors.OKGREEN
        a,_,dis,b = Decode(t.args[0], t.args[1].decode('hex'), Decode32Bits)[0]
        s = "==> %08x  %-15s %s" % (a,dis,b)
    elif t.in_slice:
        color = bcolors.FAIL
    else:
        color = bcolors.OKBLUE
    print color + s + bcolors.ENDC
