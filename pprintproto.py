#!/usr/bin/env python

import collections
import sys

class itercontext:
    '''an iterator that allows accessing previous and next elements'''
    def __init__(self,it,countprevious,countcomingnext):
        '''initializes the iterator
        it: the underlying iterator
        countprevious: number of elements to keep after being visited
        countcomingnext: number of elements to look ahead
        '''
        self.iter=iter(it)
        # current element
        self.current=None
        self.existcurrent=False
        # list of previous and next elements
        self.comingnext=collections.deque()
        self.previous=collections.deque()
        self.countprevious=countprevious
        try:
            for i in range(countcomingnext):
                self.comingnext.append(self.iter.next())
        except StopIteration:
            pass
    
    def __iter__(self):
        return self
    def next(self):
        try:
            self.comingnext.append(self.iter.next())
        except StopIteration:
            pass # if the underlying iterator is exhausted, empty comingnext
        if len(self.comingnext)==0:
            raise StopIteration # if no more item ahead, stop
        if self.existcurrent: self.previous.append(self.current)
        self.current=self.comingnext.popleft()
        self.existcurrent=True
        if len(self.previous)>self.countprevious:
            self.previous.popleft()
        return self.current
    
    def getComingnext(self):
        return self.comingnext
    def getPrevious(self):
        return self.previous
    def listcomingnext(self):
        return list(self.comingnext)
    def listprevious(self):
        return list(self.previous)

tofind = [ l.strip() for l in open(sys.argv[1]).readlines() ]

for f in sys.argv[2:]:
    it = itercontext(iter(open(f)),3,0)
    try:
        line = it.next()
    except StopIteration:
        break
    while True:
        for func in tofind:
            if func in line:
                prev = it.listprevious()
                next = [line]
                while ")" not in line:
                    try:
                        line = it.next()
                        next.append(line)
                    except StopIteration:
                        print "ERROR: Hit EOF before finding matching paren"
                        print " ".join(part.strip() for part in prev + next)
                        sys.exit(1)
                words = [ part.strip() for part in prev + next ]
                words = [ w for w in words if w and not w.startswith("//") ]
                print " ".join(words)
                tofind.remove(func)
                break
        try:
            line = it.next()
        except StopIteration:
            break

#print "\n".join(tofind)
