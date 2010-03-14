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

def make_cfg_dot(cfg,tbdict,fname):
    of = open(fname,'w')
    print >>of, "digraph G {"
    print >>of, "node [shape=record]"
    
    nodeset = set(c for c in cfg)
    for c in cfg:
        nodeset.update(cfg[c])
    
    for n in nodeset:
        if any(t.has_slice() for t in tbdict[n]):
            filled = 'style="filled" '
        else:
            filled = ''
        print >>of, '%s [ label="%s" %s]' % (nodename(n),nodestr(n),filled)
    
    for c in cfg:
        for t in cfg[c]:
            print >>of, '%s -> %s' % (nodename(c),nodename(t))
    
    print >>of,"}"
    of.close()
