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

from forensics.object2 import *
from forensics.win32.tasks import process_list
from vutils import *

class curr_proc(forensics.commands.command):

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
        return  "Print current process"
    
    def execute(self):
	(addr_space, symtab, types) = load_and_identify_image(self.op,
            self.opts)

        KPCR_ADDR = 0xFFDFF000
        prof = Profile()
        kpcr = Object('_KPCR', KPCR_ADDR, addr_space, profile=prof)
        
        thread = kpcr.Prcb.CurrentThread
        thread = thread.cast('_ETHREAD')
        print "Current thread:", thread
        print "%x" % thread.m('ThreadsProcess').v()
        proc = thread.ThreadsProcess
        print "%s (%d): %d" % (proc.ImageFileName, proc.UniqueProcessId.v(), proc.VirtualSize)
        if proc.Peb.is_valid():
            print "Command Line:", proc.Peb.ProcessParameters.CommandLine
        else:
            print "Command Line:", "PEB not resident for process!"
