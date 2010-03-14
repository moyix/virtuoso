#!/usr/bin/env python

import sys, re
import IPython
from collections import defaultdict
from interval import Interval, IntervalSet, FrozenIntervalSet
from bisect import bisect_right

from qemu_trace import *
from qemu_data import defines,uses,is_jcc,is_dynjump,memrange,qemu_regs
from newslice import load_trace, slice_trace, make_slice_cfg, make_tbdict, make_tbs
from fixedint import *

# Format: address, number of argument bytes, name
# For now stdcall is assumed
mallocs = [
    (0x7c9105d4, 12, 'RtlAllocateHeap'),
]

def counter():
    i = 0
    while True:
        yield i
        i += 1

def find_retaddr(calltb):
    for i, insn in calltb.body[::-1]:
        if insn.op.startswith('IFLO_OPS_MEM_STL'):
            return insn.args[-1]

def get_tb(index, tbmap, trace_ranges):
    starts = [t[0] for t in trace_ranges]
    pos = bisect_right(starts, index) - 1
    if pos < 0 or pos >= len(starts):
        return None
    else:
        return tbmap[trace_ranges[pos]]

def is_memop(insn):
    return insn.op.startswith("IFLO_OPS_MEM")

def remake_trace(trace):
    trace = list(enumerate(t for i,t in trace))
    tbs = make_tbs(trace)
    tbdict = make_tbdict(tbs)
    cfg = make_slice_cfg(tbs)
    return trace, tbs, tbdict, cfg

def linked_vars(insns, sink, source, start=-1, end=0, debug=False):
    if debug: print "Linking vars sink: %s source: %s between trace positions %d and %d" % (sink, source, end, start)
    if start == -1: start = len(insns) - 1

    work = set([sink])
    for i,insn in reversed(insns[end:start+1]):
        defs_set = set(defines(insn))
        uses_set = set(uses(insn))
        # For this one special case we DON'T want to track
        # the derivation of the address of a buffer.
        if is_memop(insn):
            uses_set -= set(["A0"])

        if defs_set & work:
            work = (work - defs_set) | uses_set
            if debug: print i,repr(insn)

    if debug: print "Working set at end:", work
    return source in work

if __name__ == "__main__":
    trace, inbufs, outbufs = load_trace(file(sys.argv[1]))
    tbs = make_tbs(trace)
    tbdict = make_tbdict(tbs)
    cfg = make_slice_cfg(tbs)
    
    alloc_ctr = counter()
    alloc_calls = defaultdict(list)
    alloc_rets = []
    for m,argbytes,name in mallocs:
        for idx in range(len(tbdict[m])):  # This allows us to change the tbdict during
            tb = tbdict[m][idx]       # iteration without getting stale data.

            i = tbs.index(tb)
            callsite = tbs[i-1]
            
            # Find the return address and get its TB object
            # Note: this might fail if we have nested calls, but worry
            # about that later. The "right" solution is a shadow stack
            retaddr = find_retaddr(callsite)
            for t in tbs[i:]:
                if t.label == retaddr:
                    retsite = t
                    break

            print "Call to %#x (%s) at callsite %s returns to %s" % (m, name, callsite._label_str(), retsite._label_str())
            remove_start, remove_end = callsite.end() + 1, retsite.start()

            # We replace each call to malloc with a summary function that
            # fixes the stack (since the Windows API uses callee-cleanup)
            # and inserts an IFLO_MALLOC pseudo-op into the trace.
            trace[remove_start:remove_end] = [
                (None, TraceEntry(('IFLO_TB_HEAD_EIP',  ["ALLOC_%d" % alloc_ctr.next()]))),
                (None, TraceEntry(('IFLO_ADDL_ESP_IM',  [argbytes]))),
                (None, TraceEntry(('IFLO_MALLOC',       []))),
            ]
            
            trace, tbs, tbdict, cfg = remake_trace(trace)
            alloc_rets.append(retsite.label)
            alloc_calls[(m,argbytes)].append(callsite.label)

    alloc_calls = dict(alloc_calls)
    
    trace, tbs, tbdict, cfg = slice_trace(trace, inbufs, outbufs)

#    first_ret = tbdict[alloc_rets[0]][0]
#    memops = [t for t in trace[first_ret.start():] if t[1].op.startswith("IFLO_OPS_MEM_")]
#    allocs = defaultdict(list)
#    for site in alloc_rets:
#        for tb in tbdict[site]:
#            tbstart = tb.start()
#            for i,m in memops:
#                if linked_vars(trace, "A0", EAX, start=i, end=tbstart):
#                    allocs[tb].append((i,m))
#    allocs = dict(allocs)

    EAX = "REGS_0"
    allocs = defaultdict(list)
    for site in alloc_rets:
        tb = tbdict[site][0]
        tbstart = tb.start()
        for c in cfg:
            curtb = tbdict[c][0]
            print "Checking %s" % repr(curtb)
            if not curtb.start() > tb.end(): continue
            for i,insn in curtb.body:
                print "Checking instruction %s" % ((i,insn),)
                if is_memop(insn) and linked_vars(trace, "A0", EAX, start=i, end=tbstart):
                    print "Adding memop %s in %s" % (repr(insn), repr(tb))
                    off = i - curtb.start()
                    allocs[site] += [t.body[off] for t in tbdict[c]]
    allocs = dict(allocs)
    
    #embedshell = IPython.Shell.IPShellEmbed(argv=[])
    #embedshell()
