#!/usr/bin/env python

import sys
from cPickle import dump, HIGHEST_PROTOCOL

execfile(sys.argv[1])
for k in transdict:
    transdict[k] = "\n".join(l.rstrip() for l in transdict[k].splitlines() if l.rstrip())
dump(transdict, sys.stdout, protocol=HIGHEST_PROTOCOL)
