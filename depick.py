#!/usr/bin/env python
# ©2011 Massachusetts Institute of Technology

import sys
from cPickle import load

transdict,userland,idt = load(open(sys.argv[1]))
print "transdict = {}"
print
for k in transdict:
    print 'transdict[%s] = """' % ("'%s'" % k if isinstance(k,str) else "%#x" % k,)
    print transdict[k]
    print '"""'
    print

print "userland = %s" % userland

print "idt = %s" % idt
