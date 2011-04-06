#!/usr/bin/env python
# ©2011 Massachusetts Institute of Technology

import sys
from cPickle import load

code, _, _ = load(open(sys.argv[1]))
print len(code)
