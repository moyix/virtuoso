#!/usr/bin/env python

from dynslice import *
import sys

trace = get_insns(open(sys.argv[1]))
trace = list(enumerate(trace))
out = trace.pop()
outbuf = memrange(*out[1][1])
slice = dynslice([t[1] for t in trace], outbuf)
loops = detect_loops(trace)
newslice = reroll_loops(trace, slice)
for i, insn in newslice:
    if isinstance(insn, Loop):
        print insn
    else:
        print uop_to_py(*insn)
