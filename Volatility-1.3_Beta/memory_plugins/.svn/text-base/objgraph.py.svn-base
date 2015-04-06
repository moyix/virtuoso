# Volatility
# Copyright (C) 2008 Brendan Dolan-Gavitt
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

from vutils import *
from forensics.object2 import Profile
from forensics.win32.lists import list_entry_old
from forensics.win32.tasks import process_dtb, create_addr_space
from forensics.win32.typutil import gather_pointers, gather_lists, deref
from annotations import wrkx86_annotations as annotations

from time import time

import sys
sys.setrecursionlimit(10000)

class objgraph(forensics.commands.command):
    # Declare meta information associated with this plugin
    meta_info = forensics.commands.command.meta_info 
    meta_info['author'] = 'Brendan Dolan-Gavitt'
    meta_info['copyright'] = 'Copyright (c) 2007,2008 Brendan Dolan-Gavitt'
    meta_info['contact'] = 'brendandg@gatech.edu'
    meta_info['license'] = 'GNU General Public License 2.0 or later'
    meta_info['url'] = 'https://www.volatilesystems.com/default/volatility'
    meta_info['os'] = 'WIN_32_XP_SP2'
    meta_info['version'] = '1.0'
        
    # Standard parser:
    #    '-f', '--file', '(required) Image file'
    #    '-b', '--base', '(optional) Physical offset (in hex) of DTB'
    #    '-t', '--type', '(optional) Identify the image type'
    def parser(self):

        forensics.commands.command.parser(self)

        self.op.add_option('-s', '--start',
            help='(required) address of seed instance',
            action='store', type='int', dest='start')
        self.op.add_option('-n', '--name',
            help='(required) name of seed instance type',
            action='store', type='string', dest='name')
        self.op.add_option('-d', '--depth',
            help='(optional) number of levels to recurse',
            action='store', type='int', dest='depth', default=5)

    def help(self):
        return  "Create a graph of object instances"

    # List annotations:
    # (TP, Member) => (TP, Member)
    def build_graph(self, space, type_name, address, limit=5, level=0):
        #print " "*level + "Visiting %s %#x DTB %x" % (type_name, address, space.pgd_vaddr)
        self.nodes.add( (type_name, address) )
        
        # Bail out after adding the node if we're too deep
        if level > limit:
            #print "Recursion depth hit, backing out."
            return
        
        # If this is a process, we want to propagate its space
        # as we go forward.
        if type_name == '_EPROCESS':
            try:
                dtb = process_dtb(space, self.types, address)
                new_space = create_addr_space(space, dtb)
                if new_space:
                    #print "Changing DTB from %x to %x" % (space.pgd_vaddr, dtb)
                    space = new_space
                else:
                    #print "Failed to create new space for process"
                    return
            except Exception, e:
                print e

        pointers = gather_pointers(self.types, type_name)
        lists = gather_lists(self.types, type_name)

        annotated_lists = []
        while lists:
            l = lists.pop()
            
            if len(l) > 1:
                _, tp = get_obj_offset(self.types, [type_name] + l[:-1])
                m = l[-1]
            else:
                tp = type_name 
                m = l[0]
                
            if (tp, m) in annotations:
                annotated_lists.append((l,tp,m))
            else:
                pointers.append(l + ['Flink'])
                pointers.append(l + ['Blink'])
        
        for p in pointers:
            next = deref(space, self.types, [type_name] + p, address)
            if not next:
                #print "Discarding invalid pointer."
                continue

            next_tp, next_addr = next
            if next_tp in builtin_types:
                #print "Discarding pointer to native type."
                pass
            elif next_tp == "array":
                #print "Discarding pointer to array."
                pass
            elif next_tp == "void":
                #print "Discarding pointer to void."
                pass
            elif next_addr < 0x80000000:
                #print "Discarding pointer to userland."
                pass
            else:
                self.edges.add( ((type_name, ".".join(p), address), (next_tp, next_addr)) )
                if (next_tp, next_addr) == (self.opts.name, self.opts.start):
                    #print "Added edge back to start node from %s %#x %s" % (type_name, address, p)
                    pass

                if (next_tp, next_addr) in self.nodes:
                    pass
                    #print "Skipping traversal (already visited)"
                else:
                    #print " "*level + "Will visit pointer", p
                    self.build_graph(space, next_tp, next_addr, limit, level+1)

        #print "Gathering lists..."
        # Code to actually traverse these lists
        for l,tp,m in annotated_lists:
            entry_type, field = annotations[(tp,m)]
            l_off, _ = get_obj_offset(self.types, [type_name] + l)

            # We only want to go to the immediate neighbors here
            gen_flink = list_entry_old(space, self.types, address+l_off, entry_type, fieldname=field, forward=True)
            gen_blink = list_entry_old(space, self.types, address+l_off, entry_type, fieldname=field, forward=False)
            for g in [gen_flink, gen_blink]:
                try:
                    next_tp, next_addr = g.next()
                except StopIteration:
                    continue

                self.edges.add( ((type_name, ".".join(l), address), (next_tp, next_addr)) )
                if (next_tp, next_addr) == (self.opts.name, self.opts.start):
                    #print "Added edge back to start node from %s %#x %s" % (type_name, address, l)
                    pass
                if not (next_tp, next_addr) in self.nodes:
                    #print " "*level + "Will visit list entry", l
                    self.build_graph(space, next_tp, next_addr, limit, level+1)

            # This code is wrong: would add *every* entry in list as a neighbor
            #for next_tp, next_addr in 
            #    self.edges.add( ((type_name, address), (next_tp, next_addr)) )
            #    if (next_tp, next_addr) == (self.opts.name, self.opts.start):
            #        print "Added edge back to start node from %s %#x %s" % (type_name, address, l)
            #    if not (next_tp, next_addr) in self.nodes:
            #        #print "Will visit list entry", l
            #        self.build_graph(space, next_tp, next_addr, limit, level+1)

    def indegree(self, node):
        ind = 0
        for (src, dst) in self.edges:
            mod_src = (src[0],src[2])
            if mod_src != node and dst == node:
                #print (src,dst)
                ind += 1
        return ind

    def execute(self):
        (space, self.symtab, self.types) = load_and_identify_image(self.op, self.opts)
        self.profile = Profile()
        self.nodes = set()
        self.edges = set()

        if not self.opts.start or not self.opts.name:
            self.op.error("Address and name of seed instance are required")
        
        type_name = self.opts.name
        address = self.opts.start
        start_time = time()
        self.build_graph(space, type_name, address, limit=self.opts.depth)
        end_time = time()
        #print "%d,%d,%d,%d,%f" % (self.opts.depth, len(self.nodes), len(self.edges), self.indegree((type_name, address)), end_time - start_time)
        #for n,a in self.nodes:
        #    print "[%#x %s]" % (a,n)
        #for (src, dst) in self.edges:
        #    print "[%#x %s] => [%#x %s]" % (src[1],src[0],dst[1],dst[0])

        print "digraph instances {"
        for n,a in self.nodes:
            print '    %s_%x [label="{ %s | %x}", shape=record] ;' % (n,a,n,a)
        for (src, dst) in self.edges:
            print "    %s_%x -> %s_%x ;" % (src[0],src[2],dst[0],dst[1])
        print "}"

