#!/usr/bin/env python
# -*- coding: utf-8 -*-
# © 2011 Massachusetts Institute of Technology

import operator, networkx
from collections import defaultdict

def pdominators(G,start='STOP'):
    G = G.reverse()
    dom = {}
    for n in G.nodes():
        dom[n] = set(G.nodes())

    # Start node dominates only itself
    dom[start] = set([start])

    changed = True
    while changed:
        changed = False
        for n in set(G.nodes()) - set([start]):
            new_set = set(reduce(operator.and_, [dom[p] for p in G.predecessors(n)])) | set([n])
            if new_set != dom[n]:
                dom[n] = new_set
                changed = True
    return dom

def pdominators_strict(G,start):
    dom = pdominators(G,start)
    sdom = dict( (n,set(dom[n])-set([n])) for n in dom )
    return sdom

def pdominator_tree(G, sdom=None, start='STOP'):
    if not sdom:
        sdom = pdominators_strict(G,start)
    tdom = {}
    for r in sdom:
        n = None
        for n in sdom[r]:
            if all( n not in sdom[x] for x in sdom[r] - set([n]) ): break
        if n is not None: tdom[r] = n
    return tdom

def tree_dot(t,fn):
    f = open(fn,'w')

    nodes = set()
    nodes.update(t.keys())
    for n in t:
        nodes.add(t[n])

    print >>f, "digraph G {"
    print >>f, "node [shape=record]"
    print >>f, "\n".join('n_%s [label="%s"]' % (i,i) for i in nodes)
    print >>f, "\n".join("n_%s -> n_%s" % (j,i) for i,j in t.items())
    print >>f, "}"
    f.close()

def graph_dot(g,fn):
    f = open(fn,'w')
    print >>f, "digraph G {"
    print >>f, "node [shape=record]"
    print >>f, "\n".join('n_%s [label="%s"]' % (i,i) for i in g.nodes())
    print >>f, "\n".join('n_%s -> n_%s' % i for i in g.edges())
    print >>f, "}"
    f.close()

def branches(cfg, sdom=None, start='STOP'):
    if sdom is None:
        sdom = pdominators_strict(cfg, start)
    return [ (a,b) for (a,b) in cfg.edges() if b not in sdom[a] ]

def control_deps(G, start='ENTRY', stop='STOP'):
    sdom = pdominators_strict(G,stop)
    tdom = pdominator_tree(G,sdom)
    S = branches(G, sdom)

    cdeps = defaultdict(set)
    for a,b in S:
        L = tdom[a] if a not in sdom[b] else a
        deps = []
        cur = b
        while cur != L:
            deps.append(cur)
            cur = tdom[cur]
        cdeps[a].update(deps)
        #print (a,b), deps, a

    return dict(cdeps)
