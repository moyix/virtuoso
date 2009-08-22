#!/usr/bin/env python


import sys
from bisect import bisect_right
from collections import defaultdict
from fixedint import *
from qemu_data import uses, defines
from qemu_trace import get_trace

trace = get_trace(open(sys.argv[1]), codeloc=True)

class Instance(object):
    def __init__(self):
        self.base = 0
        self.members = defaultdict(set)
        self.locations = set()

        # Objects that have been merged into this one
        self.merged = []
    def size(self):
        return max(self.members) + 4
    def __str__(self):
        s = "Object instance @ %#x size %d from %d codepoints\n" % (self.base,self.size(),len(self.locations))
        membstrs = []
        for off in sorted(self.members):
            membstrs.append("    %#x : [%s]" % (off, ", ".join("%#x" % x for x in self.members[off])))
        s += "\n".join(membstrs)
        return s
    def __repr__(self):
        return "[obj instance @ %#x size %d]" % (self.base,self.size())
    def merge(self, inst):
        assert self.base < inst.base < self.base + self.size()
        instoff = inst.base - self.base
        for off in inst.members:
            self.members[instoff + off] |= inst.members[off]
        self.locations |= inst.locations
    def name(self):
        return "obj_%08x" % self.base

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

        valid = False
        objbase = UInt(memb_addr)
        for i,s in slice[:-1]:
            if s.op == "IFLO_ADDL_A0_IM":
                objbase -= UInt(s.args[0])
            elif s.op == "IFLO_ADDL_A0_SEG":
                pass
            elif s.op == "IFLO_MOVL_A0_IM":
                objbase -= UInt(s.args[0])
            else:
                # This means there's some other kind of operation being
                # done on A0 that we don't know how to reverse. Examples
                # are adding a value from a register, 
                #print "DEBUG: Instruction sequence too complex: %s" % ",".join(r[1].op for r in slice)
                break
        else:
            valid = True

        if not valid: continue

        if objbase <= memb_addr:
            offset = int(UInt(memb_addr) - objbase)
            objbase = int(objbase)
            if offset > objbase:
                objbase, offset = offset, objbase
            insts[objbase].base = objbase
            insts[objbase].locations.add(insn.location)
            insts[objbase].members[offset].add(memb_val)
            values[memb_val].add(insts[objbase])

print "==> Found %d objects, largest: %#x bytes <==" % (len(insts),max(v.size() for v in insts.values()))
print "==> Observed %d distinct values <==" % len(values)
for i in sorted(insts):
    print insts[i]
print "="*60

#addrs = sorted(insts)
#for i,addr in enumerate(addrs):
#    if i == len(addrs) - 1: break
#    overlaps = []
#    objend = insts[addr].base + insts[addr].size()
#    j = i + 1
#    while addrs[j] < objend and j < len(addrs):
#        # Only include those overlaps that look like:
#        # [      ]      rather    [         ]
#        #    [     ]     than          [  ]
#        #if addrs[j] + insts[addrs[j]].size() > objend:
#        #    overlaps.append(insts[addrs[j]])
#        #j += 1
#    if overlaps:
#        print "%s overlaps with:" % repr(insts[addr])
#        print "\n".join("    " + repr(o) for o in overlaps)

mergemap = {}
merged = {}
addrs = sorted(insts)
while addrs:
    current = insts[addrs.pop(0)]
    while addrs:
        nextaddr = addrs.pop(0)
        next = insts[nextaddr]
        if next.base < current.base + current.size():
            print "Merging %s into %s" % (repr(next), repr(current))
            current.merge(next)
            mergemap[next.base] = current
        else:
            addrs.insert(0, nextaddr)
            break
    merged[current.base] = current

print "="*60
print "%d objects remain after merging" % len(merged)
for m in merged:
    print merged[m]
    print "    => Code Locations <="
    for l in merged[m].locations:
        print "    %s" % (l,)

print "="*60

f = open(sys.argv[2],'w')
print >>f, 'graph ['
print >>f, '  comment "%s"' % sys.argv[1]
print >>f, '  directed 1'
addrs = dict((a,i) for i,a in enumerate(merged))
for a,i in addrs.items():
    print >>f, '  node ['
    print >>f, '    id %d' % i
    print >>f, '    label "%s"' % repr(merged[a])
    print >>f, '  ]'

# Dot:
# '%s [label = "%s"]' % (merged[a].name(), repr(merged[a]))

addrlist = sorted(merged)
for v in values:
    pos = bisect_right(addrlist,v) - 1
    cand = merged[addrlist[pos]]
    if cand.base <= v < cand.base + cand.size():
        for src in values[v]:
            if not src.base in merged:
                real_src = mergemap[src.base]
            else:
                real_src = merged[src.base]
            print >>f, '  edge ['
            print >>f, '    source %d' % addrs[real_src.base]
            print >>f, '    target %d' % addrs[cand.base]
            print >>f, '  ]'
            # Dot:
            # print "%s -> %s" % (real_src.name(), cand.name())

print >>f, ']'
f.close()
