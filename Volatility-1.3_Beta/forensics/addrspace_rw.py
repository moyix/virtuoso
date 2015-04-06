# Volatility
# Copyright (C) 2007,2008 Volatile Systems
#
# Original Source:
# Copyright (C) 2004,2005,2006 4tphi Research
# Author: {npetroni,awalters}@4tphi.net (Nick Petroni and AAron Walters)
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
@contact:      awalters@volatilesystems.com
@organization: Volatile Systems
"""

""" Read/Write file address space """

import os
import struct
from forensics.addrspace import FileAddressSpace

class WritableFileAddressSpace:
    def write(self, addr, buf):
        self.fhandle.seek(addr)        
        return self.fhandle.write(buf)    

    def write_long(self, addr, val):
        longbuf =  struct.pack('=L', val)
        self.write(addr, buf)

class FileAddressSpaceWr(FileAddressSpace,WritableFileAddressSpace):
    def __init__(self, fname, mode='rb+', fast=False):
        super(FileAddressSpaceWr,self).__init__(self, fname, mode, fast)
