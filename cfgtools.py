from collections import defaultdict

def nodename(n):
    if isinstance(n,str):
        return "n_" + n.replace("_","")
    else:
        return "n_%x" % n

def nodestr(n):
    if isinstance(n,str):
        return n
    else:
        return "%#x" % n

def make_cfg_dot(tbs,tbdict,fname):
    of = open(fname,'w')
    print >>of, "digraph G {"
    print >>of, "node [shape=record]"

    nodeset = set()
    edges = defaultdict(int)

    for i in range(len(tbs)-1):
        nodeset.add(tbs[i].label)
        edges[tbs[i].label,tbs[i+1].label] += 1
    nodeset.add(tbs[i+1].label)

    for n in nodeset:
        if any(t.has_slice() for t in tbdict[n]):
            filled = 'style="filled" '
        else:
            filled = ''
        print >>of, '%s [ label="%s" %s]' % (nodename(n),nodestr(n),filled)
    
    for c,t in edges:
        print >>of, '%s -> %s [label="%d"]' % (nodename(c),nodename(t),edges[c,t])
    
    print >>of,"}"
    of.close()
