#!/usr/bin/env python

import sys, re
import IPython
from collections import defaultdict
from interval import Interval, IntervalSet, FrozenIntervalSet
from bisect import bisect_right

from qemu_trace import *
from newslice import make_slice_cfg, make_tbdict, make_tbs


sizes = dict(zip("BWLQ", [1,2,4,8]))
rex = re.compile('IFLO_OPS_MEM_(ST|LD)[US]?([BWLQ])')

def is_memop(x):
    return x.op.startswith('IFLO_OPS_MEM')

def getrange(insn):
    m = rex.search(insn.op)
    if not m:
        raise ValueError("Not a memory operation")

    rw, sz = m.groups()
    rw = "r" if rw == "LD" else "w"
    sz = sizes[sz]
    return (insn.args[2], insn.args[2]+sz, rw)

def get_tb(index, tbmap, trace_ranges):
    starts = [t[0] for t in trace_ranges]
    pos = bisect_right(starts, index) - 1
    if pos < 0 or pos >= len(starts):
        return None
    else:
        return tbmap[trace_ranges[pos]]

if __name__ == "__main__":
    insns = get_insns(file(sys.argv[1]))
    trace = list(enumerate(TraceEntry(t) for t in insns))
    tbs = make_tbs(trace)
    tbdict = make_tbdict(tbs)
    cfg = make_slice_cfg(tbs)
    
    memops = filter(lambda x: is_memop(x[1]), trace)
    ranges = [ (i,) + getrange(insn) for i,insn in memops ]

    d = defaultdict(list)
    for i,st,ed,m in ranges:
        d[(st,ed)].append(m)
    d = dict(d)

    # Things written to before they were read
    writes = [k for k in d if d[k][0] == 'w']

    # Coalesce overlapping writes
    iset = IntervalSet()
    for w in writes:
        iset.add(Interval(*w))
    iset = FrozenIntervalSet(iset)

    tbmap = dict((tb.range(), tb) for tb in tbs)
    trace_ranges = sorted(tbmap.keys())
    
    embedshell = IPython.Shell.IPShellEmbed(argv=[])
    embedshell()
