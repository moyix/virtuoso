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
@author:       AAron Walters
@license:      GNU General Public License 2.0 or later
@contact:      awalters@volatilesystems.com
@organization: Volatile Systems
"""

"""Module to make virtual address spaces writable.
"""
import struct
from forensics.x86 import IA32PagedMemory,IA32PagedMemoryPae

class WritableMemory:
    """
    Mixin class that can be used to add write functionality
    To any standard address space that supports write() and
    vtop().
    """
    def write(self, vaddr, buf):
        length = len(buf)
        first_block = 0x1000 - vaddr % 0x1000
        full_blocks = ((length + (vaddr % 0x1000)) / 0x1000) - 1
        left_over = (length + vaddr) % 0x1000
        
        paddr = self.vtop(vaddr)
        if paddr == None:        
            return None
        
        if length < first_block:
            self.base.write(paddr, buf)
            return

        self.base.write(paddr, buf[:first_block])
        buf = buf[first_block:]

        new_vaddr = vaddr + first_block
        for i in range(0,full_blocks):
            paddr = self.vtop(new_vaddr)
            if paddr == None:
                raise Exception("Failed to write to page at %#x" % new_vaddr)
            self.base.write(paddr, buf[:0x1000])
            new_vaddr = new_vaddr + 0x1000
            buf = buf[0x1000:]

        if left_over > 0:
            paddr = self.vtop(new_vaddr)
            if paddr == None:
                raise Exception("Failed to write to page at %#x" % new_vaddr)
            assert len(buf) == left_over
            self.base.write(paddr, buf)

    def write_long_phys(self, addr, val):
        buf = struct.pack('=L', val)
        self.base.write(addr, buf)

class IA32PagedMemoryWr(IA32PagedMemory, WritableMemory):
    pass

class IA32PagedMemoryPaeWr(IA32PagedMemoryPae, WritableMemory):
    pass
