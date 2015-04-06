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
from forensics.win32.tasks import process_list
import csv

class listfeatures(forensics.commands.command):

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

        self.op.add_option('-l', '--list',
            help='list of fields to show',
            action='store', type='string', dest='fieldlist')

    def help(self):
        return  ""
    
    def execute(self):
        (addr_space, symtab, types) = load_and_identify_image(self.op, self.opts)
        fieldfile = open(self.opts.fieldlist)
        pslist = process_list(addr_space, types, symtab)
    
        for line in fieldfile:
            line = line.strip()
            fields = line.split(".")
            try:
                fields = fields[:-1] + [int(fields[-1])]
            except ValueError:
                pass
            print line + "," + ",".join("%x" % read_obj(addr_space, types, ['_EPROCESS'] + fields, p) for p in pslist)
