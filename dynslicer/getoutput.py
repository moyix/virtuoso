#!/usr/bin/env python
# -*- coding: utf-8 -*-
# © 2011 Massachusetts Institute of Technology

from newslice import *
import sys

trace, ins, outs = load_trace(sys.argv[1], quiet=True)
outs = dict( (int(x.replace('MEM_',''),16), '') for x in outs )

m2p = {
    'Q': 'Q',
    'L': 'I',
    'W': 'H',
    'B': 'B',
}
for i in range(len(trace)):
    insn = trace[i]
    m = memrex.search(insn.op)
    if m:
        addr = insn.args[1]
        if addr not in outs: continue

        tp, sz = m.groups()
        if tp == 'LD': continue

        buf = pack("<%s" % m2p[sz], insn.args[-1])
        for i, c in zip(range(addr, addr+len(buf)), buf):
            outs[i] = c
s = ''
for k in sorted(outs):
    if outs[k] == '': break
    s += outs[k]
print s.encode('hex')
