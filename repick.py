#!/usr/bin/env python

import sys
from cPickle import dump, HIGHEST_PROTOCOL

execfile(sys.argv[1])
for k in transdict:
    transdict[k] = "\n".join(l.strip() for l in transdict[k].splitlines() if l.strip())
dump(transdict, sys.stdout, protocol=HIGHEST_PROTOCOL)
