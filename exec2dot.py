#!/usr/bin/env python
# ©2011 Massachusetts Institute of Technology

import sys
from collections import defaultdict

if len(sys.argv) < 2:
    f = sys.stdin
else:
    f = open(sys.argv[1])

nodes = set()
edges = defaultdict(int)

entries = [l.strip() for l in f]
for i in range(0,len(entries)-1):
    nodes.add(entries[i])
    edges[entries[i],entries[i+1]] += 1
nodes.add(entries[i+1])

if len(sys.argv) < 2:
    o = sys.stdout
else:
    fname = sys.argv[1].rsplit('.',1)[0] + '.dot'
    o = open(fname,'w')

print >>o, "digraph G {"
print >>o, 'node [shape="record"]'
for n in nodes:
    print >>o, 'n%s [label="%s"]' % (n,n)
for s,d in edges:
    print >>o, 'n%s -> n%s [label="%d"]' % (s,d,edges[s,d])
print >>o, "}"

if len(sys.argv) >= 2:
    o.close()

