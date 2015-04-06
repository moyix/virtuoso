# Volatility
# Copyright (C) 2008 Volatile Systems
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details. 
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
#

"""
@author:       Brendan Dolan-Gavitt
@license:      GNU General Public License 2.0 or later
@contact:      brendandg@gatech.edu
@organization: Georgia Institute of Technology
"""

from datetime import datetime,timedelta
from bisect import bisect_left, bisect_right
from forensics.win32.typutil import gather_members
from vutils import *
import csv

FMT = "%Y-%m-%d %H:%M:%S"

class dsips(forensics.commands.command):

    # Declare meta information associated with this plugin
    
    meta_info = forensics.commands.command.meta_info 
    meta_info['author'] = 'Brendan Dolan-Gavitt'
    meta_info['copyright'] = 'Copyright (c) 2007,2008 Brendan Dolan-Gavitt'
    meta_info['contact'] = 'brendandg@gatech.edu'
    meta_info['license'] = 'GNU General Public License 2.0 or later'
    meta_info['url'] = 'http://moyix.blogspot.com/'
    meta_info['os'] = 'WIN_32_XP_SP2'
    meta_info['version'] = '1.0'

    def parser(self):
        forensics.commands.command.parser(self)

        self.op.add_option('-o', '--offset',
            help='data structure offset',
            action='store', type='int', dest='offset')

        self.op.add_option('-n', '--name',
            help='generate profile for this structure',
            action='store', type='string', dest='name')

    def find_memb(self, membs, base, addr):
        # Find the start and end of the range that contains this offset
        starts = [m[0] for m in membs]
        b = bisect_right(starts, addr-base)
        a = bisect_left(starts, membs[b-1][0])
        #print "Ranges for offset %d found from %d to %d:" % (addr-base, st, ed)
        #print membs[st:ed]
        found = []
        for i in range(a,b):
            st, ed, m = membs[i]
            if addr >= base+st and addr < base+ed:
                found.append((i,m))
        return found

    def help(self):
        return  ""
    
    def execute(self):
        typ = self.opts.name
        base = self.opts.offset

        tups = []
        for m in gather_members(types, typ):
            offset, tp = get_obj_offset(types, [typ] + m)
            sz = builtin_types[tp][0]
            tups.append((offset,offset+sz,m))
        tups.sort()
        tp_size = obj_size(types, typ)

        hist = [0]*len(tups)
        ips = [set() for i in hist]

        reader = csv.reader(open(self.opts.filename))
        for row in reader:
            try:
                addr, ip, code = int(row[0],16),int(row[1],16),int(row[2])
            except ValueError:
                continue

            if code and addr >= base and addr < base + tp_size:
                for i,m in self.find_memb(tups, base, addr):
                    hist[i] += 1
                    if not ip in ips[i]: ips[i].add(ip)
                    #print "%#x %#x %s" % (addr,ip,m)

        #print "index,count,ips # field_name"
        for i in range(len(hist)):
            _,tp = get_obj_offset(types, [typ] + tups[i][2])
            if tp == 'pointer':
                print "%d,%d,%d,%s # %s" % (i,hist[i],len(ips[i]), ",".join("%x" % j for j in ips[i]), ".".join(str(t) for t in tups[i][2]))
            #print "%d,%d,%s" % (i,hist[i],",".join("%x" % j for j in ips[i]))
            #print "%d,%d,%s" % (i,hist[i],",".join([typ] + [str(f) for f in tups[i][2]]))
