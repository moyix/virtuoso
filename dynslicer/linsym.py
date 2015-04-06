#!/usr/bin/env python
# -*- coding: utf-8 -*-
# © 2011 Massachusetts Institute of Technology

from bisect import bisect_right

class Syms:
    def __init__(self, fname):
        with open(fname) as f:
            tups = [line.strip().split() for line in f]
            tups = [(int(t[0],16), t[2], t[3] if len(t) > 3 else "") for t in tups if t[1] != 'a']
            tups.sort()
        self.starts = [t[0] for t in tups]
        self.syms = tups

    def lookup(self, a):
        pos = bisect_right(self.starts,a)
        if pos == 0 or pos == len(self.starts): return None
        addr, name, mod = self.syms[pos-1]
        off = a - addr
        return "%s%s%s" % (mod + "!" if mod else "", name, "+%#x" % off if off else "")

    def __call__(self, a):
        return self.lookup(a)

if __name__ == "__main__":
    import sys
    import IPython
    lookup = Syms(sys.argv[1])
    
    embedshell = IPython.Shell.IPShellEmbed(argv=[])
    embedshell()
