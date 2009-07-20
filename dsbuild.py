#!/usr/bin/env python


import sys
from collections import defaultdict
from fixedint import *
from qemu_data import uses, defines
from qemu_trace import get_trace

trace = get_trace(open(sys.argv[1]))

class Instance(object):
    def __init__(self):
        self.base = 0
        self.members = defaultdict(set)
    def size(self):
        return max(self.members)+4
    def __str__(self):
        s = "Object instance @ %#x size %d\n" % (self.base,self.size())
        membstrs = []
        for off in sorted(self.members):
            membstrs.append("    %#x : [%s]" % (off, ", ".join("%#x" % x for x in self.members[off])))
        s += "\n".join(membstrs)
        return s
    def __repr__(self):
        return "[obj instance @ %#x size %d]" % (self.base,self.size())

insts = defaultdict(Instance)
values = defaultdict(set)

for i in xrange(len(trace)-1,-1,-1):
    idx, insn = trace[i]
    if (insn.op == 'IFLO_OPS_MEM_LDL_T0_A0' or insn.op == 'IFLO_OPS_MEM_LDL_T1_A0'):
        memb_addr = insn.args[1]
        memb_val = insn.args[2]
        #print "Pointer dereference => Read 4 bytes at %#x = %#x" % (memb_addr,memb_val)

        # Tiny version of dynamic slicing -- track A0 until it comes from somewhere else
        j = i
        work = set(['A0'])
        slice = []
        while True:
            j -= 1
            _ , trcent = trace[j]
            defs_set = set(defines(trcent))
            uses_set = set(uses(trcent))
            if defs_set & work:
                work = (work - defs_set) | uses_set
                slice.append((j,trcent))
            if 'A0' not in work: break

        objbase = UInt(memb_addr)
        for i,s in slice:
            if s.op == "IFLO_ADDL_A0_IM":
                objbase -= UInt(s.args[0])

        if objbase <= memb_addr:
            offset = int(UInt(memb_addr) - objbase)
            objbase = int(objbase)
            if offset > objbase:
                objbase, offset = offset, objbase
            insts[objbase].base = objbase
            insts[objbase].members[offset].add(memb_val)
            values[memb_val].add(insts[objbase])

print "==> Found %d objects, largest: %#x bytes <==" % (len(insts),max(v.size() for v in insts.values()))
for i in sorted(insts):
    print insts[i]
print "="*60
addrs = sorted(insts)
for i,addr in enumerate(addrs):
    if i == len(addrs) - 1: break
    overlaps = []
    objend = insts[addr].base + insts[addr].size()
    j = i + 1
    while addrs[j] < objend and j < len(addrs):
        overlaps.append(insts[addrs[j]])
        j += 1
    if overlaps:
        print "%s overlaps with:" % repr(insts[addr])
        print "\n".join("    " + repr(o) for o in overlaps)
