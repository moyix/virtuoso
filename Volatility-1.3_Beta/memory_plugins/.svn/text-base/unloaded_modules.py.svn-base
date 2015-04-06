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
@author:       AAron Walters and Brendan Dolan-Gavitt
@license:      GNU General Public License 2.0 or later
@contact:      awalters@volatilesystems.com,bdolangavitt@wesleyan.edu
@organization: Volatile Systems
"""

from time import ctime

from forensics.object2 import *
from forensics.win32.info import find_kddebuggerdatablock
from forensics.win32.datetime import windows_to_unix_time
from vutils import *

unloaded_mod_types = {
  '_UNLOADED_MODULE' : [ 0x18, {
    'Name' : [0x0, ['_UNICODE_STRING']],
    'Start' : [0x8, ['pointer', ['void']]],
    'End' : [0xc, ['pointer', ['void']]],
    'Timestamp' : [0x10, ['_LARGE_INTEGER']],
} ],
}

class unloaded_modules(forensics.commands.command):

    # Declare meta information associated with this plugin
    
    meta_info = forensics.commands.command.meta_info 
    meta_info['author'] = 'Brendan Dolan-Gavitt'
    meta_info['copyright'] = 'Copyright (c) 2007,2008 Brendan Dolan-Gavitt'
    meta_info['contact'] = 'bdolangavitt@wesleyan.edu'
    meta_info['license'] = 'GNU General Public License 2.0 or later'
    meta_info['url'] = 'http://moyix.blogspot.com/'
    meta_info['os'] = 'WIN_32_XP_SP2'
    meta_info['version'] = '1.0'

    def help(self):
        return  "List unloaded modules"
    
    def execute(self):
        from vtypes import xpsp2types
        
        xpsp2types['_KDDEBUGGER_DATA64'][1]['MmUnloadedDrivers'] = [ 0x220, ['unsigned long long']]
        xpsp2types['_KDDEBUGGER_DATA64'][1]['MmLastUnloadedDriver'] = [ 0x228, ['unsigned long long']]

        profile = Profile()
        profile.add_types(unloaded_mod_types)

        (addr_space, symtab, types) = load_and_identify_image(self.op, self.opts)

        KdDebuggerDataBlock = find_kddebuggerdatablock(addr_space, types)
        if not KdDebuggerDataBlock:
            return 0
        dbg_block = Object("_KDDEBUGGER_DATA64", KdDebuggerDataBlock,
                           addr_space, profile=Profile())
        
        drv_list_addr = Object('unsigned long', dbg_block.MmUnloadedDrivers, addr_space, profile=profile).v()
        drv_count = Object('unsigned long', dbg_block.MmLastUnloadedDriver, addr_space, profile=profile).v()

        print "%-20s %-10s %-10s %s" % ("Name", "Start", "End", "Timestamp")
        i = 0
        while i < drv_count:
            drv = Object('_UNLOADED_MODULE', drv_list_addr+(i*0x18), addr_space, profile=profile)
            ts_win = drv.Timestamp.HighPart << 32 | drv.Timestamp.LowPart
            ts_unix = windows_to_unix_time(ts_win)
            print "%-20s %#08x %#08x %s" % (drv.Name.Buffer, drv.Start.v(), drv.End.v(), ctime(ts_unix))
            i += 1
