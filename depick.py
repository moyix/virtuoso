#!/usr/bin/env python

import sys
from cPickle import load

transdict,userland = load(open(sys.argv[1]))
print "transdict = {}"
print
for k in transdict:
    print 'transdict[%s] = """' % ("'%s'" % k if isinstance(k,str) else "%#x" % k,)
    print transdict[k]
    print '"""'
    print

print "userland = %s" % userland
