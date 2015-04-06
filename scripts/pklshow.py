#!/usr/bin/env python
# ©2011 Massachusetts Institute of Technology

import sys
from cPickle import load

x = load(open(sys.argv[1]))
code = x[0]
for k in code:
    print "============ %s ============" % (k if isinstance(k, str) else "%#x" % k)
    print code[k]
