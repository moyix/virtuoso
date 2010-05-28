#!/usr/bin/env python

from cPickle import load
import sys

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

    def disable(self):
        self.HEADER = ''
        self.OKBLUE = ''
        self.OKGREEN = ''
        self.WARNING = ''
        self.FAIL = ''
        self.ENDC = ''

trace = load(open(sys.argv[1]))
for i, t in trace:
    if t.op == 'IFLO_TB_HEAD_EIP':
        print bcolors.WARNING + repr(t) + bcolors.ENDC
    elif t.in_slice:
        print bcolors.FAIL + repr(t) + bcolors.ENDC
    else:
        print bcolors.OKBLUE  + repr(t) + bcolors.ENDC
