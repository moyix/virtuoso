#!/usr/bin/env python

from dynslice import *
import sys

trace = get_insns(open(sys.argv[1]))
trace = list(enumerate(trace))
out = trace.pop()
outbuf = memrange(*out[1][1])
slice = dynslice([t[1] for t in trace], outbuf, outbuf_hack=True)
loops = detect_loops(trace)
real_loop = reroll_loops(trace, slice)[0]
print real_loop
