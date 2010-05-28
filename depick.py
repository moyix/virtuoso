#!/usr/bin/env python

import sys
from cPickle import load

transdict = load(open(sys.argv[1]))
print "transdict = {}"
print
for k in transdict:
    print 'transdict[%s] = """' % ("%#x" % k if isinstance(k, int) else "'%s'" % k,)
    print transdict[k]
    print '"""'
    print
