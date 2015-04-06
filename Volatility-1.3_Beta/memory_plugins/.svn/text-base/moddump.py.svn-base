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

import re
from sre_constants import error as REError
from sre_constants import SRE_FLAG_IGNORECASE

from forensics.win32.tasks import process_list,process_dtb,create_addr_space
from forensics.object2 import *
from forensics.win32.modules import modules_list
from forensics.win32.executable import rebuild_exe_dsk,rebuild_exe_mem
from vutils import *

def find_space(addr_space, types, symtab, addr):
    # short circuit if given one works
    if addr_space.is_valid_address(addr): return addr_space

    pslist = process_list(addr_space, types, symtab)
    for p in pslist:
        dtb = process_dtb(addr_space, types, p)
        space = create_addr_space(addr_space, dtb)
        if space.is_valid_address(addr):
            return space
    return None

class moddump(forensics.commands.command):

    # Declare meta information associated with this plugin
    
    meta_info = forensics.commands.command.meta_info 
    meta_info['author'] = 'Brendan Dolan-Gavitt'
    meta_info['copyright'] = 'Copyright (c) 2007,2008 Brendan Dolan-Gavitt'
    meta_info['contact'] = 'bdolangavitt@wesleyan.edu'
    meta_info['license'] = 'GNU General Public License 2.0 or later'
    meta_info['url'] = 'http://moyix.blogspot.com/'
    meta_info['os'] = 'WIN_32_XP_SP2'
    meta_info['version'] = '1.0'

    def parser(self):
        forensics.commands.command.parser(self)
        self.op.add_option('-m', '--mode',
                      help=('strategy to use when saving executable. Use "disk" to '
                            'save using disk-based section sizes, "mem" for memory-'
                            'based sections. (default: "mem")'),
                      action='store', type='string', default="mem", dest='mode')
        self.op.add_option('-u', '--unsafe',
                      help='do not perform sanity checks on sections when dumping',
                      action='store_false', default=True, dest='safe')
        self.op.add_option('-o', '--offset',
                      help='dump module whose base address is OFFSET (hex)',
                      action='store', type='string', dest='offset')
        self.op.add_option('-p', '--pattern',
                      help='dump modules matching REGEX',
                      action='store', type='string', dest='regex')
        self.op.add_option('-i', '--ignore-case',
                      help='ignore case in pattern match',
                      action='store_true', default=False, dest='ignore_case')


    def help(self):
        return  "Dump loaded kernel modules to disk."
    
    def execute(self):
	(addr_space, symtab, types) = load_and_identify_image(self.op,
            self.opts)
        
        if self.opts.mode == "disk":
            rebuild_exe = rebuild_exe_dsk
        elif self.opts.mode == "mem":
            rebuild_exe = rebuild_exe_mem
        else:
            self.op.error('"mode" must be one of "disk" or "mem"')

        if self.opts.regex:
            try:
                if self.opts.ignore_case:
                    mod_re = re.compile(self.opts.regex, flags=SRE_FLAG_IGNORECASE)
                else:
                    mod_re = re.compile(self.opts.regex)
            except REError, e:
                self.op.error('Error parsing regular expression: %s' % e)

        if self.opts.offset:
            modbase = int(self.opts.offset, 16)
            print "Dumping (unknown module name) @%x" % modbase
            if addr_space.is_valid_address(modbase):
                f = open("driver.%x.sys" % modbase, 'wb')
                rebuild_exe(addr_space, types, modbase, f, safe=self.opts.safe)
                f.close()
            else:
                print "Unable to dump module; header not resident"
            return

        modlist = modules_list(addr_space, types, symtab)
        theProfile = Profile()

        for m in modlist:
            mod = Object("_LDR_DATA_TABLE_ENTRY", m, addr_space, profile=theProfile)
            
            if self.opts.regex:
                if not mod_re.search(mod.FullDllName.Buffer): continue

            space = find_space(addr_space, types, symtab, mod.DllBase.v())
            print "Dumping", mod.FullDllName, "(%s)" % mod.BaseDllName, "@%x" % mod.DllBase.v()
            if space:
                f = open("driver." + mod.BaseDllName.Buffer, 'wb')
                rebuild_exe(space, types, mod.DllBase.v(), f, safe=self.opts.safe)
                f.close()
            else:
                print "Unable to dump module; header not resident"
