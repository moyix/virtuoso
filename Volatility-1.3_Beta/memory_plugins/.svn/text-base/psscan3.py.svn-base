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

from vutils import *
from forensics.win32.scan2 import GenMemScanObject, PoolScanner, PoolScanProcessFast2
from forensics.win32.scan2 import scan_addr_space
from forensics.windowed import WindowedAddressSpace
import forensics.win32.meta_info as meta_info

class RobustPsScanner(PoolScanner):
    def __init__(self, poffset, outer):
        PoolScanner.__init__(self, poffset, outer)
        self.sz = outer.pool_size

    def check_addr(self,buff,found):
        cnt = 0
        for func in self.constraints:
            val = func(buff,found)
            if val == True:
                cnt = cnt+1
            else:
                return cnt
        return cnt

    # Need to override process_buffer since we don't want to
    # check pool tags
    def process_buffer(self, buf, poffset, metadata=None):
        if poffset + self.sz >= self.outer.addr_space.fsize:
            return

        for i in range(0, (len(buf)-self.sz)+1, 8):
            match_count = self.check_addr(buf, i)
            if match_count == self.get_limit():
                self.matches.append(poffset+i)
                self.object_action(buf,i)

class ProcessScanFast3(GenMemScanObject):
    def __init__(self, addr_space):
        GenMemScanObject.__init__(self, addr_space)
        self.pool_size = obj_size(types, '_EPROCESS')
        self.matches = []

    class Scan(RobustPsScanner,PoolScanProcessFast2.Scan):
        def __init__(self, poffset, outer):
            RobustPsScanner.__init__(self, poffset, outer)
            self.add_constraint(self.check_ws_lock_count)
            self.add_constraint(self.check_ac_lock_count)
            self.add_constraint(self.check_dtb)
            self.add_constraint(self.check_dtb_aligned)
            self.add_constraint(self.check_granted_access)
            self.add_constraint(self.check_vadroot)
            self.add_constraint(self.check_object_table)
            self.add_constraint(self.check_threadlist_flink)
            self.add_constraint(self.check_pcb_threadlist_flink)
            self.add_constraint(self.check_readylist_flink)
            self.add_constraint(self.check_wsl)

        def object_action(self, buf, found):
            PoolScanProcessFast2.Scan.object_action(self, buf, found)
        
        def object_offset(self, found):
            return found

        def check_in_kernel(self, buf, field, found):
            kernel = 0x80000000
            val = read_obj_from_buf(buf, types, field, found)
            return val >= kernel

        def check_granted_access(self, buf, found):
            granted_access = read_obj_from_buf(buf, types, ['_EPROCESS','GrantedAccess'], found)
            return (granted_access & 0x1f07fb) == 0x1f07fb
        
        def check_vadroot(self, buf, found):
            field = ['_EPROCESS', 'VadRoot']
            if self.check_in_kernel(buf, field, found):
                return True
            else:
                val = read_obj_from_buf(buf, types, field, found)
                if val == 0:
                    return True
                else:
                    return False
        
        def check_object_table(self, buf, found):
            field = ['_EPROCESS','ObjectTable']
            return self.check_in_kernel(buf, field, found)

        def check_threadlist_flink(self, buf, found):
            field = ['_EPROCESS','ThreadListHead','Flink']
            return self.check_in_kernel(buf, field, found)

        def check_pcb_threadlist_flink(self, buf, found):
            field = ['_EPROCESS','Pcb','ThreadListHead','Flink']
            return self.check_in_kernel(buf, field, found)

        def check_readylist_flink(self, buf, found):
            field = ['_EPROCESS','Pcb','ReadyListHead','Flink']
            return self.check_in_kernel(buf, field, found)

        def check_wsl(self, buf, found):
            field = ['_EPROCESS','Vm','VmWorkingSetList']
            val = read_obj_from_buf(buf, types, field, found)
            return val >= 0xc0000000
        
        def check_ws_lock_count(self, buf, found):
            field = ['_EPROCESS','WorkingSetLock','Count']
            val = read_obj_from_buf(buf, types, field, found)
            return val == 1

        def check_ac_lock_count(self, buf, found):
            field = ['_EPROCESS','AddressCreationLock','Count']
            val = read_obj_from_buf(buf, types, field, found)
            return val == 1

class psscan3(forensics.commands.command):

    # Declare meta information associated with this plugin
    
    meta_info = forensics.commands.command.meta_info 
    meta_info['author'] = 'Brendan Dolan-Gavitt'
    meta_info['copyright'] = 'Copyright (c) 2007,2008 Brendan Dolan-Gavitt'
    meta_info['contact'] = 'brendandg@gatech.edu'
    meta_info['license'] = 'GNU General Public License 2.0 or later'
    meta_info['url'] = 'http://moyix.blogspot.com/'
    meta_info['os'] = 'WIN_32_XP_SP2'
    meta_info['version'] = '1.0'

    def help(self):
        return  ""
    
    def execute(self):
        meta_info.set_datatypes(types)
        scanners = []
        space = FileAddressSpace(self.opts.filename)
        #search_space = WindowedAddressSpace(space, 0x0223f020 - 0x1000, 0x2000) 
        search_space = space
        print "PID    PPID   Time created             Time exited              Offset     PDB        Remarks\n"+ \
              "------ ------ ------------------------ ------------------------ ---------- ---------- ----------------";
        scanners.append((ProcessScanFast3(search_space)))
        scan_addr_space(search_space,scanners)
