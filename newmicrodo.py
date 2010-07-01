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
@organization: Georgia Institute of Technology / MIT Lincoln Laboratory
"""

from interval import Interval, IntervalSet
from fixedint import *
from forensics.object2 import *
from forensics.object import *
from vutils import *
from forensics.win32.datetime import windows_to_unix_time
from forensics.x86_rw import IA32PagedMemoryWr
from time import ctime,asctime,gmtime
from struct import unpack,pack
from collections import defaultdict
import cPickle as pickle

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

    def disable(self):
        self.HEADER = ''
        self.OKBLUE = ''
        self.OKGREEN = ''
        self.WARNING = ''
        self.FAIL = ''
        self.ENDC = ''

try:
    from collections import namedtuple
except ImportError:
    from namedtuple import namedtuple

def ULInt32(buf):
    assert len(buf) == 4
    return UInt(unpack("<I", buf)[0])

def ULInt16(buf):
    assert len(buf) == 2
    return UInt(unpack("<H", buf)[0])

def ULInt8(buf):
    assert len(buf) == 1
    return UInt(unpack("<B", buf)[0])

def SLInt8(buf):
    assert len(buf) == 1
    return UInt(unpack("<b", buf)[0])

def DATA_BITS(SHIFT):
    return (1 << (3 + SHIFT))

def DATA_MASK(SHIFT):
    return { 0: 0xff,
             1: 0xffff,
             2: 0xffffffff,
             3: 0xffffffffffffffff }[SHIFT]

def SHIFT1_MASK(DATA_BITS):
    if DATA_BITS <= 32:
        return 0x1f
    else:
        return 0x3f

def load_eflags(eflags, update_mask, EFL):
    CC_SRC = eflags & (CC_O | CC_S | CC_Z | CC_A | CC_P | CC_C)
    DF = 1 - (2 * int((eflags >> 10) & 1))
    eflags = (EFL & ~update_mask) | (eflags & update_mask)
    return CC_SRC, DF, eflags

# Functions for condition code calculations

CC_C = 0x0001
CC_P = 0x0004
CC_A = 0x0010
CC_Z = 0x0040
CC_S = 0x0080
CC_O = 0x0800

parity_table = [
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    CC_P, 0, 0, CC_P, 0, CC_P, CC_P, 0,
    0, CC_P, CC_P, 0, CC_P, 0, 0, CC_P,
]

def lshift(x,n):
    if (n >= 0):
        return (x << n)
    else:
        return (x >> -n)

def compute_c_adcb(CC_SRC, CC_DST):
    src1 = Int(CC_SRC)
    cf = (Int(Byte(CC_DST)) <= Int(Byte(src1)))
    return cf

def compute_c_adcl(CC_SRC, CC_DST):
    src1 = Int(CC_SRC)
    cf = (CC_DST <= UInt(src1))
    return cf

def compute_c_adcw(CC_SRC, CC_DST):
    src1 = Int(CC_SRC)
    cf = (Int(UShort(CC_DST)) <= Int(UShort(src1)))
    return cf

def compute_c_addb(CC_SRC, CC_DST):
    src1 = Int(CC_SRC)
    cf = (Int(Byte(CC_DST)) < Int(Byte(src1)))
    return cf

def compute_c_addl(CC_SRC, CC_DST):
    src1 = Int(CC_SRC)
    cf = (CC_DST < UInt(src1))
    return cf

def compute_c_addw(CC_SRC, CC_DST):
    src1 = Int(CC_SRC)
    cf = (Int(UShort(CC_DST)) < Int(UShort(src1)))
    return cf

def compute_c_eflags(CC_SRC, CC_DST):
    return Int((CC_SRC & UInt(1)))

def compute_c_incl(CC_SRC, CC_DST):
    return Int(CC_SRC)

def compute_c_logicb(CC_SRC, CC_DST):
    return 0

def compute_c_logicl(CC_SRC, CC_DST):
    return 0

def compute_c_logicw(CC_SRC, CC_DST):
    return 0

def compute_c_mull(CC_SRC, CC_DST):
    cf = (CC_SRC != UInt(0))
    return cf

def compute_c_sarl(CC_SRC, CC_DST):
    return Int((CC_SRC & UInt(1)))

def compute_c_sbbb(CC_SRC, CC_DST):
    src1 = Int(((CC_DST + CC_SRC) + UInt(1)))
    src2 = Int(CC_SRC)
    cf = (Int(Byte(src1)) <= Int(Byte(src2)))
    return cf

def compute_c_sbbl(CC_SRC, CC_DST):
    src1 = Int(((CC_DST + CC_SRC) + UInt(1)))
    src2 = Int(CC_SRC)
    cf = (UInt(src1) <= UInt(src2))
    return cf

def compute_c_sbbw(CC_SRC, CC_DST):
    src1 = Int(((CC_DST + CC_SRC) + UInt(1)))
    src2 = Int(CC_SRC)
    cf = (Int(UShort(src1)) <= Int(UShort(src2)))
    return cf

def compute_c_shlb(CC_SRC, CC_DST):
    return Int(((CC_SRC >> ((1 << 3) - 1)) & UInt(1)))

def compute_c_shll(CC_SRC, CC_DST):
    return Int(((CC_SRC >> ((1 << 5) - 1)) & UInt(1)))

def compute_c_shlw(CC_SRC, CC_DST):
    return Int(((CC_SRC >> ((1 << 4) - 1)) & UInt(1)))

def compute_c_subb(CC_SRC, CC_DST):
    src1 = Int((CC_DST + CC_SRC))
    src2 = Int(CC_SRC)
    cf = (Int(Byte(src1)) < Int(Byte(src2)))
    return cf

def compute_c_subl(CC_SRC, CC_DST):
    src1 = Int((CC_DST + CC_SRC))
    src2 = Int(CC_SRC)
    cf = (UInt(src1) < UInt(src2))
    return cf

def compute_c_subw(CC_SRC, CC_DST):
    src1 = Int((CC_DST + CC_SRC))
    src2 = Int(CC_SRC)
    cf = (Int(UShort(src1)) < Int(UShort(src2)))
    return cf

def compute_all_adcb(CC_SRC, CC_DST):
    src1 = Int(CC_SRC)
    src2 = Int(((CC_DST - CC_SRC) - UInt(1)))
    cf = (Int(Byte(CC_DST)) <= Int(Byte(src1)))
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((Int(Byte(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 3));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(UInt((src1 ^ src2) ^ -1) & (UInt(src1) ^ CC_DST)), 12 - (1 << 3));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_adcl(CC_SRC, CC_DST):
    src1 = Int(CC_SRC)
    src2 = Int(((CC_DST - CC_SRC) - UInt(1)))
    cf = (CC_DST <= UInt(src1))
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((CC_DST == UInt(0)) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 5));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(UInt((src1 ^ src2) ^ -1) & (UInt(src1) ^ CC_DST)), 12 - (1 << 5));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_adcw(CC_SRC, CC_DST):
    src1 = Int(CC_SRC)
    src2 = Int(((CC_DST - CC_SRC) - UInt(1)))
    cf = (Int(UShort(CC_DST)) <= Int(UShort(src1)))
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((Int(UShort(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 4));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(UInt((src1 ^ src2) ^ -1) & (UInt(src1) ^ CC_DST)), 12 - (1 << 4));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_addb(CC_SRC, CC_DST):
    src1 = Int(CC_SRC)
    src2 = Int((CC_DST - CC_SRC))
    cf = (Int(Byte(CC_DST)) < Int(Byte(src1)))
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((Int(Byte(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 3));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(UInt((src1 ^ src2) ^ -1) & (UInt(src1) ^ CC_DST)), 12 - (1 << 3));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_addl(CC_SRC, CC_DST):
    src1 = Int(CC_SRC)
    src2 = Int((CC_DST - CC_SRC))
    cf = (CC_DST < UInt(src1))
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((CC_DST == UInt(0)) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 5));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(UInt((src1 ^ src2) ^ -1) & (UInt(src1) ^ CC_DST)), 12 - (1 << 5));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_addw(CC_SRC, CC_DST):
    src1 = Int(CC_SRC)
    src2 = Int((CC_DST - CC_SRC))
    cf = (Int(UShort(CC_DST)) < Int(UShort(src1)))
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((Int(UShort(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 4));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(UInt((src1 ^ src2) ^ -1) & (UInt(src1) ^ CC_DST)), 12 - (1 << 4));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_decb(CC_SRC, CC_DST):
    src1 = Int((CC_DST + UInt(1)))
    src2 = 1
    cf = Int(CC_SRC)
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((Int(Byte(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 3));
    sf = (tmp & 128)
    of = (((CC_DST & UInt(255)) == ((UInt(1) << ((1 << 3) - 1)) - UInt(1))) << 11)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_decl(CC_SRC, CC_DST):
    src1 = Int((CC_DST + UInt(1)))
    src2 = 1
    cf = Int(CC_SRC)
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((CC_DST == UInt(0)) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 5));
    sf = (tmp & 128)
    of = (((CC_DST & UInt(4294967295)) == ((UInt(1) << ((1 << 5) - 1)) - UInt(1))) << 11)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_decw(CC_SRC, CC_DST):
    src1 = Int((CC_DST + UInt(1)))
    src2 = 1
    cf = Int(CC_SRC)
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((Int(UShort(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 4));
    sf = (tmp & 128)
    of = (((CC_DST & UInt(65535)) == ((UInt(1) << ((1 << 4) - 1)) - UInt(1))) << 11)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_eflags(CC_SRC, CC_DST):
    return Int(CC_SRC)

def compute_all_incb(CC_SRC, CC_DST):
    src1 = Int((CC_DST - UInt(1)))
    src2 = 1
    cf = Int(CC_SRC)
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((Int(Byte(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 3));
    sf = (tmp & 128)
    of = (((CC_DST & UInt(255)) == (UInt(1) << ((1 << 3) - 1))) << 11)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_incl(CC_SRC, CC_DST):
    src1 = Int((CC_DST - UInt(1)))
    src2 = 1
    cf = Int(CC_SRC)
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((CC_DST == UInt(0)) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 5));
    sf = (tmp & 128)
    of = (((CC_DST & UInt(4294967295)) == (UInt(1) << ((1 << 5) - 1))) << 11)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_incw(CC_SRC, CC_DST):
    src1 = Int((CC_DST - UInt(1)))
    src2 = 1
    cf = Int(CC_SRC)
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((Int(UShort(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 4));
    sf = (tmp & 128)
    of = (((CC_DST & UInt(65535)) == (UInt(1) << ((1 << 4) - 1))) << 11)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_logicb(CC_SRC, CC_DST):
    cf = 0
    pf = Int(parity_table[int(CC_DST)])
    af = 0
    zf = ((Int(Byte(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 3));
    sf = (tmp & 128)
    of = 0
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_logicl(CC_SRC, CC_DST):
    cf = 0
    pf = Int(parity_table[int(CC_DST)])
    af = 0
    zf = ((CC_DST == UInt(0)) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 5));
    sf = (tmp & 128)
    of = 0
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_logicw(CC_SRC, CC_DST):
    cf = 0
    pf = Int(parity_table[int(CC_DST)])
    af = 0
    zf = ((Int(UShort(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 4));
    sf = (tmp & 128)
    of = 0
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_mulb(CC_SRC, CC_DST):
    cf = (CC_SRC != UInt(0))
    pf = Int(parity_table[int(CC_DST)])
    af = 0
    zf = ((Int(Byte(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 3));
    sf = (tmp & 128)
    of = (cf << 11)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_mull(CC_SRC, CC_DST):
    cf = (CC_SRC != UInt(0))
    pf = Int(parity_table[int(CC_DST)])
    af = 0
    zf = ((CC_DST == UInt(0)) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 5));
    sf = (tmp & 128)
    of = (cf << 11)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_mulw(CC_SRC, CC_DST):
    cf = (CC_SRC != UInt(0))
    pf = Int(parity_table[int(CC_DST)])
    af = 0
    zf = ((Int(UShort(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 4));
    sf = (tmp & 128)
    of = (cf << 11)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_sarb(CC_SRC, CC_DST):
    cf = Int((CC_SRC & UInt(1)))
    pf = Int(parity_table[int(CC_DST)])
    af = 0
    zf = ((Int(Byte(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 3));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(CC_SRC ^ CC_DST), 12 - (1 << 3));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_sarl(CC_SRC, CC_DST):
    cf = Int((CC_SRC & UInt(1)))
    pf = Int(parity_table[int(CC_DST)])
    af = 0
    zf = ((CC_DST == UInt(0)) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 5));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(CC_SRC ^ CC_DST), 12 - (1 << 5));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_sarw(CC_SRC, CC_DST):
    cf = Int((CC_SRC & UInt(1)))
    pf = Int(parity_table[int(CC_DST)])
    af = 0
    zf = ((Int(UShort(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 4));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(CC_SRC ^ CC_DST), 12 - (1 << 4));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_sbbb(CC_SRC, CC_DST):
    src1 = Int(((CC_DST + CC_SRC) + UInt(1)))
    src2 = Int(CC_SRC)
    cf = (Int(Byte(src1)) <= Int(Byte(src2)))
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((Int(Byte(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 3));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(UInt(src1 ^ src2) & (UInt(src1) ^ CC_DST)), 12 - (1 << 3));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_sbbl(CC_SRC, CC_DST):
    src1 = Int(((CC_DST + CC_SRC) + UInt(1)))
    src2 = Int(CC_SRC)
    cf = (UInt(src1) <= UInt(src2))
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((CC_DST == UInt(0)) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 5));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(UInt(src1 ^ src2) & (UInt(src1) ^ CC_DST)), 12 - (1 << 5));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_sbbw(CC_SRC, CC_DST):
    src1 = Int(((CC_DST + CC_SRC) + UInt(1)))
    src2 = Int(CC_SRC)
    cf = (Int(UShort(src1)) <= Int(UShort(src2)))
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((Int(UShort(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 4));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(UInt(src1 ^ src2) & (UInt(src1) ^ CC_DST)), 12 - (1 << 4));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_shlb(CC_SRC, CC_DST):
    cf = Int(((CC_SRC >> ((1 << 3) - 1)) & UInt(1)))
    pf = Int(parity_table[int(CC_DST)])
    af = 0
    zf = ((Int(Byte(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 3));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(CC_SRC ^ CC_DST), 12 - (1 << 3));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_shll(CC_SRC, CC_DST):
    cf = Int(((CC_SRC >> ((1 << 5) - 1)) & UInt(1)))
    pf = Int(parity_table[int(CC_DST)])
    af = 0
    zf = ((CC_DST == UInt(0)) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 5));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(CC_SRC ^ CC_DST), 12 - (1 << 5));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_shlw(CC_SRC, CC_DST):
    cf = Int(((CC_SRC >> ((1 << 4) - 1)) & UInt(1)))
    pf = Int(parity_table[int(CC_DST)])
    af = 0
    zf = ((Int(UShort(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 4));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(CC_SRC ^ CC_DST), 12 - (1 << 4));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_subb(CC_SRC, CC_DST):
    src1 = Int((CC_DST + CC_SRC))
    src2 = Int(CC_SRC)
    cf = (Int(Byte(src1)) < Int(Byte(src2)))
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((Int(Byte(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 3));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(UInt(src1 ^ src2) & (UInt(src1) ^ CC_DST)), 12 - (1 << 3));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_subl(CC_SRC, CC_DST):
    src1 = Int((CC_DST + CC_SRC))
    src2 = Int(CC_SRC)
    cf = (UInt(src1) < UInt(src2))
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((CC_DST == UInt(0)) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 5));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(UInt(src1 ^ src2) & (UInt(src1) ^ CC_DST)), 12 - (1 << 5));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

def compute_all_subw(CC_SRC, CC_DST):
    src1 = Int((CC_DST + CC_SRC))
    src2 = Int(CC_SRC)
    cf = (Int(UShort(src1)) < Int(UShort(src2)))
    pf = Int(parity_table[int(CC_DST)])
    af = Int((((CC_DST ^ UInt(src1)) ^ UInt(src2)) & UInt(16)))
    zf = ((Int(UShort(CC_DST)) == 0) << 6)
    tmp = lshift(Int(CC_DST), 8 - (1 << 4));
    sf = (tmp & 128)
    tmp___0 = lshift(Int(UInt(src1 ^ src2) & (UInt(src1) ^ CC_DST)), 12 - (1 << 4));
    of = (tmp___0 & 2048)
    return (((((cf | pf) | af) | zf) | sf) | of)

( CC_OP_DYNAMIC, CC_OP_EFLAGS,
  CC_OP_MULB, CC_OP_MULW, CC_OP_MULL, CC_OP_MULQ,
  CC_OP_ADDB, CC_OP_ADDW, CC_OP_ADDL, CC_OP_ADDQ,
  CC_OP_ADCB, CC_OP_ADCW, CC_OP_ADCL, CC_OP_ADCQ,
  CC_OP_SUBB, CC_OP_SUBW, CC_OP_SUBL, CC_OP_SUBQ,
  CC_OP_SBBB, CC_OP_SBBW, CC_OP_SBBL, CC_OP_SBBQ,
  CC_OP_LOGICB, CC_OP_LOGICW, CC_OP_LOGICL, CC_OP_LOGICQ,
  CC_OP_INCB, CC_OP_INCW, CC_OP_INCL, CC_OP_INCQ,
  CC_OP_DECB, CC_OP_DECW, CC_OP_DECL, CC_OP_DECQ,
  CC_OP_SHLB, CC_OP_SHLW, CC_OP_SHLL, CC_OP_SHLQ,
  CC_OP_SARB, CC_OP_SARW, CC_OP_SARL, CC_OP_SARQ,
  CC_OP_NB ) = range(43)

CCEntry = namedtuple("CCEntry", "compute_all compute_c")

cc_table = [
    CCEntry(None, None), 
    CCEntry(compute_all_eflags, compute_c_eflags), 
    CCEntry(compute_all_mulb, compute_c_mull), 
    CCEntry(compute_all_mulw, compute_c_mull), 
    CCEntry(compute_all_mull, compute_c_mull), 
    CCEntry(None, None), 
    CCEntry(compute_all_addb, compute_c_addb), 
    CCEntry(compute_all_addw, compute_c_addw), 
    CCEntry(compute_all_addl, compute_c_addl), 
    CCEntry(None, None), 
    CCEntry(compute_all_adcb, compute_c_adcb), 
    CCEntry(compute_all_adcw, compute_c_adcw), 
    CCEntry(compute_all_adcl, compute_c_adcl), 
    CCEntry(None, None), 
    CCEntry(compute_all_subb, compute_c_subb), 
    CCEntry(compute_all_subw, compute_c_subw), 
    CCEntry(compute_all_subl, compute_c_subl), 
    CCEntry(None, None), 
    CCEntry(compute_all_sbbb, compute_c_sbbb), 
    CCEntry(compute_all_sbbw, compute_c_sbbw), 
    CCEntry(compute_all_sbbl, compute_c_sbbl), 
    CCEntry(None, None), 
    CCEntry(compute_all_logicb, compute_c_logicb), 
    CCEntry(compute_all_logicw, compute_c_logicw), 
    CCEntry(compute_all_logicl, compute_c_logicl), 
    CCEntry(None, None), 
    CCEntry(compute_all_incb, compute_c_incl), 
    CCEntry(compute_all_incw, compute_c_incl), 
    CCEntry(compute_all_incl, compute_c_incl), 
    CCEntry(None, None), 
    CCEntry(compute_all_decb, compute_c_incl), 
    CCEntry(compute_all_decw, compute_c_incl), 
    CCEntry(compute_all_decl, compute_c_incl), 
    CCEntry(None, None), 
    CCEntry(compute_all_shlb, compute_c_shlb), 
    CCEntry(compute_all_shlw, compute_c_shlw), 
    CCEntry(compute_all_shll, compute_c_shll), 
    CCEntry(None, None), 
    CCEntry(compute_all_sarb, compute_c_sarl), 
    CCEntry(compute_all_sarw, compute_c_sarl), 
    CCEntry(compute_all_sarl, compute_c_sarl), 
    CCEntry(None, None)
]

# Some paging constants

PAGE_SIZE = 0x1000

( PD_PRESENT,
  PD_RW,
  PD_US,
  PD_PWT,
  PD_PCD,
  PD_A,
  PD_RESERVED,
  PD_PAGE_SIZE,
  PD_GLOBAL ) = [1 << i for i in range(9)]

( PT_PRESENT,
  PT_RW,
  PT_US,
  PT_PWT,
  PT_PCD,
  PT_A,
  PT_D,
  PT_PAT,
  PT_GLOBAL ) = [1 << i for i in range(9)]

class OutSpace:
    def __init__(self):
        self.scratch = defaultdict(dict)
        self.debug = False

    def write(self, addr, val, pack_char, label):
        if self.debug: print "Writing %#x to address %#x (output label %s)" % (val, addr, label)
        buf = pack("<" + pack_char, int(val))
        for (i,c) in enumerate(buf):
            self.scratch[label][int(addr+i)] = c

    def get_output(self, label):
        if label not in self.scratch:
            raise KeyError("Output label '%s' not defined." % label)

        data = {}
        start = 0
        buf = ""
        for i in sorted(self.scratch[label]):
            if not buf: start = i
            buf += self.scratch[label][i]
            if i+1 not in self.scratch[label]:
                data[start] = buf
                buf = ""
        # Assumption: the output buffer is contiguous
        assert len(data) == 1
        return data.values()[0]

class PCOW(object):
    def __init__(self, base):
        self.base = base
        self.scratch = {}
        self.debug = False
        self.extended_area = 0x80000000
        
        # Set up TPR, phys address 0xfee00080
        self.write(0xfee00080, '\x00\x00\x00\x00')

    def reserve(self, num_pages):
        old = self.extended_area
        self.extended_area += (num_pages * PAGE_SIZE)
        return old

    def read(self, addr, length):
        if self.debug: print "DEBUG: Reading %d bytes at %#x" % (length, addr)
        data = []
        for i in range(addr,addr+length):
            if i in self.scratch:
                data.append(self.scratch[i])
            else:
                b = self.base.read(i,1)
                if not b:
                    raise ValueError("Memory read error at %#x" % i)
                else:
                    data.append(b)
        if self.debug: print "DEBUG: Read", "".join(data).encode('hex')
        return "".join(data)

    def write(self, addr, buf):
        for (i,c) in enumerate(buf):
            self.scratch[int(addr+i)] = c

    def get_scratch(self):
        data = {}
        start = 0
        buf = ""
        for i in sorted(self.scratch):
            if not buf: start = i
            buf += self.scratch[i]
            if i+1 not in self.scratch:
                data[start] = buf
                buf = ""
        return data

    def dump_scratch(self):
        for start, buf in self.get_scratch().items():
            print hex(start),":",buf.encode('hex')
    
    def dd(self,addr,length=0x80):
        for x in range(0,length,16):
            print "%08x  " % (addr+x),
            for i in range(0,16,4):
                dword = []
                for j in range(4):
                    if addr+x+i+j in self.scratch:
                        b = self.scratch[addr+x+i+j]
                        hc = (bcolors.FAIL + "%02x" + bcolors.ENDC) % ord(b)
                    else:
                        b = self.base.read(addr+x+i+j,1)
                        if not b:
                            hc = "??"
                        else:
                            hc = "%02x" % ord(b)
                    dword.insert(0,hc)
                print "".join(dword),
            print

class VCOW:
    def __init__(self, base):
        self.base = base
        self.debug = False
        self.userland = {}
        self.reserved = []
        self.allocated = IntervalSet()
        self.heap_ptr = self.reserve_addr_space()

    def init_userland(self, userland):
        self.userland = userland

    def reserve_addr_space(self):
        """Reserves a 4M chunk in the guest. Returns the starting virtual address."""
        pcow = self.base.base
        if self.base.pae:
            raise NotImplementedError("See your friendly local developer for details")
        else:
            cr3 = self.base.pgd_vaddr
            entries = unpack("<1024I", pcow.read(cr3, 0x1000))
            for i, e in enumerate(entries):
                vaddr = i << 22
                if not e & 1 and not vaddr in self.reserved:
                    self.reserved.append(vaddr)
                    break
            else:
                raise MemoryError("No free space left in guest")

            # Allocate the new PT
            pt = pcow.reserve(1)
            # Add the PDE
            pcow.write(cr3 + i*4, pack("<I", pt | PD_PRESENT | PD_RW))
            # Alloacte pages
            pages_start = pcow.reserve(PAGE_SIZE / 4)
            # Add PTEs to the PD
            for j in range(PAGE_SIZE / 4):
                pcow.write(pt + j*4,
                    pack("<I", (pages_start+(j*PAGE_SIZE)) | PT_PRESENT | PT_RW))
            return vaddr

    def read(self, addr, length):
        if self.debug: print "DEBUG: Reading %d bytes at %#x" % (length, addr)

        data = []
        for i in range(addr,addr+length):
            if i in self.userland:
                data.append(self.userland[i])
            else:
                b = self.base.read(i,1)
                if not b:
                    raise ValueError("Memory read error at %#x" % i)
                else:
                    data.append(b)
        if self.debug: print "DEBUG: Read", "".join(data).encode('hex')
        return "".join(data)

    def write(self, addr, val, pack_char):
        buf = pack("<" + pack_char, int(val))
        if self.debug: print "DEBUG: Writing %#x to address %#x" % (val, addr)

        for (i,c) in enumerate(buf):
            if int(addr+i) in self.userland:
                self.userland[int(addr+i)] = c
            else:
                self.base.write(int(addr+i),c)

    def alloc(self, size):
        if not size: raise ValueError("Invalid size %d for malloc" % size)
        size = int(size)

        base = self.heap_ptr & 0xffc00000
        limit = base + 0x400000
        
        # Store this so we can return it
        vaddr = self.heap_ptr

        if self.heap_ptr + size > limit:
            self.heap_ptr = self.reserve_addr_space()
            vaddr = self.heap_ptr
        else:
            self.heap_ptr += size

        self.allocated.add(Interval(vaddr,vaddr+size-1))

        if self.debug: print "DEBUG: Allocating %#x bytes at %#010x" % (size, vaddr)
        return UInt(vaddr)

#    def get_scratch(self):
#        data = {}
#        start = 0
#        buf = ""
#        for i in sorted(self.heap):
#            if not buf: start = i
#            buf += self.heap[i]
#            if i+1 not in self.heap:
#                data[start] = buf
#                buf = ""
#        return data
#
#    def dump_scratch(self):
#        for start, buf in self.get_scratch().items():
#            print hex(start),":",buf.encode('hex')
#    
    def dd(self,addr,length=0x80):
        for x in range(0,length,16):
            print "%08x  " % (addr+x),
            for i in range(0,16,4):
                dword = []
                for j in range(4):
                    if addr+x+i+j in self.userland:
                        b = self.userland[addr+x+i+j]
                        hc = (bcolors.FAIL + "%02x" + bcolors.ENDC) % ord(b)
                    else:
                        b = self.base.read(addr+x+i+j,1)
                        if not b:
                            hc = "??"
                        else:
                            hc = "%02x" % ord(b)
                    dword.insert(0,hc)
                print "".join(dword),
            print

class Goto(Exception):
    def __init__(self, label):
        self.label = label

Segment = namedtuple("Segment", "selector base limit flags")

def seg_base(e1, e2):
    return ((e1 >> 16) | ((e2 & 0xff) << 16) | (e2 & 0xff000000))

def seg_limit(e1, e2):
    limit = (e1 & 0xffff) | (e2 & 0x000f0000)
    if (e2 & DESC_G_MASK):
        limit = (limit << 12) | 0xfff
    return limit

def load_seg(mem, selector, GDT, LDT):
    selector &= 0xffff
    if (selector & 0x4):
        DT = LDT
    else:
        DT = GDT

    index = selector & ~7
    assert (index + 7) <= DT.limit
    
    ptr = DT.base + index
    
    # GDT/LDT entries are 64 bits. Load each half.
    e1 = ULInt32(mem.read(ptr,   4))
    e2 = ULInt32(mem.read(ptr+4, 4))
    base = seg_base(e1, e2)
    limit = seg_limit(e1, e2)
    flags = e2

    return Segment(selector, base, limit, flags)
    
def load_env(env_file):
    regs = {}
    # GP registers
    for i in range(2):
        current = env_file.readline().strip().split()
        for c in current:
            var,val = c.split('=')
            regs[var] = UInt(int(val, 16))
    # EIP and EFLAGS
    current = env_file.readline().strip()
    eip,efl,_ = current.split(" ", 2)
    for c in [eip,efl]:
        var,val = c.split('=')
        regs[var] = UInt(int(val, 16))
    # Segment registers
    for i in range(8):
        var,vals = env_file.readline().strip().split('=')
        var = var.strip()
        vals = [UInt(int(v.strip(),16)) for v in vals.split()]
        regs[var] = Segment(*vals)
    # GDT/IDT
    for i in range(2):
        var,vals = env_file.readline().strip().split('=')
        var = var.strip()
        vals = [UInt(int(v.strip(),16)) for v in vals.split()]
        regs[var] = Segment(None,vals[0],vals[1],None)
    # Control Registers
    current = env_file.readline().strip().split()
    for c in current:
        var,val = c.split('=')
        regs[var] = UInt(int(val, 16))
    
    # Check for the sysenter MSRs
    current = env_file.readline().strip().split()
    while current:
        if current[0].startswith('sysenter'):
            for c in current:
                var,val = c.split('=')
                regs[var] = UInt(int(val, 16))
            break
        current = env_file.readline().strip().split()

    return regs

PAE_SHIFT = 5
PAE_FLAG = 1 << PAE_SHIFT
DF_SHIFT = 10
DF_MASK = 1 << DF_SHIFT

TF_SHIFT   = 8
IOPL_SHIFT = 12
VM_SHIFT   = 17

TF_MASK   = 0x00000100
IF_MASK   = 0x00000200
DF_MASK   = 0x00000400
IOPL_MASK = 0x00003000
NT_MASK   = 0x00004000
RF_MASK   = 0x00010000
VM_MASK   = 0x00020000
AC_MASK   = 0x00040000
VIF_MASK  = 0x00080000
VIP_MASK  = 0x00100000
ID_MASK   = 0x00200000

DESC_G_MASK = 1 << 23

class newmicrodo(forensics.commands.command):

    # Declare meta information associated with this plugin
    
    meta_info = {} 
    meta_info['author'] = 'Brendan Dolan-Gavitt'
    meta_info['copyright'] = 'Copyright (c) 2007,2008 Brendan Dolan-Gavitt'
    meta_info['contact'] = 'brendandg@gatech.edu'
    meta_info['license'] = 'GNU General Public License 2.0 or later'
    meta_info['url'] = 'http://cc.gatech.edu/~brendan/'
    meta_info['os'] = 'WIN_32_XP_SP2'
    meta_info['version'] = '1.0'
        
    def parser(self):
        forensics.commands.command.parser(self)
        self.op.add_option('-m', '--micro-op',
            help='filename of translated micro-op trace to execute',
            dest='micro')
        self.op.add_option('-e', '--env',
            help='CPU environment to use (output of info registers in QEMU)',
            dest='env')
        self.op.add_option('-i', '--interp',
            help='Python function f that takes the outbuf and interprets it',
            dest='interp')
        self.op.add_option('-n', '--input',
            help='Input vars; should evaluate to a Python list',
            dest='inputs')
        self.op.add_option('-d', '--debug',
            help='enable debug output (print every block as executed)',
            action='store_true', default=False, dest='debug')

    def help(self):
        return  "execute translated QEMU micro-ops"

    def execute(self):
        theProfile = Profile()
        #(addr_space, symtab, types) = load_and_identify_image(self.op, self.opts)
        thefile = FileAddressSpace(self.opts.filename)
        flat = PCOW(thefile)

        if not self.opts.env:
            self.op.error('Must provide a valid CPU environment (use "info registers")')
        if not self.opts.micro:
            self.op.error('We need some code to execute')

        # Load the CPU environment
        env = load_env(open(self.opts.env))
        if self.opts.debug:
            print "Starting environment:"
            print open(self.opts.env).read()
        locals().update(env)
        
        # Set up virtual memory
        if CR4 & PAE_FLAG:
            addr_space = IA32PagedMemoryPae(flat, int(CR3))
        else:
            addr_space = IA32PagedMemoryWr(flat, int(CR3))

        # Wrap it in copy-on-write
        mem = VCOW(addr_space)
        out = OutSpace()

        if self.opts.debug:
            mem.debug = True
            flat.debug = False
            out.debug = True

        # Inputs to the block of micro-ops we want to to execute
        #inputs = [ 0xdeadf000 ] # pBuf -- pointer to the output buffer
        if self.opts.inputs:
            inputs = eval(self.opts.inputs)
        else:
            inputs = []

        if self.opts.debug:
            print "Starting with inputs:"
            print "  inputs = [ %s ]" % (", ".join("%#x" % i for i in inputs))
            print

        # With real introspection, these would be actual values
        # from the env.
        if (EFL & DF_MASK) >> DF_SHIFT:
            DF = -1
        else:
            DF = 1

        # This actually runs the translated code
        code, userland = pickle.load(open(self.opts.micro))
        mem.init_userland(userland)
        label = 'START'
        import time
        t1 = time.time()
        while True:
            if self.opts.debug:
                print "Executing block %s" % (hex(label) if isinstance(label,int) else label)
                print "\n".join("  " + l for l in code[label].splitlines())
            try:
                exec(code[label])
            except Goto, g:
                if g.label == '__end__': break
                else: label = g.label
            except Exception, e:
                print ">>>> ERROR:",e,"<<<<"
                print code[label]
                import pdb
                pdb.post_mortem(sys.exc_info()[2])
        t2 = time.time()
        if self.opts.debug:
            print
            print "Time taken: %f ms" % ((t2-t1)*1000)

        print

        # TODO: multiple outputs
        data = out.get_output('out')
        print "Output:", data.encode('hex')

        print

        # Run the translation function
        if self.opts.debug:
            print "Decoding function:"
            if self.opts.interp:
                s = self.opts.interp.decode('string_escape')
                print "\n".join("  " + l for l in s.splitlines())
            else:
                print "(none)"
            print

        if self.opts.interp:
            print "Decoded data:"
            exec(self.opts.interp.decode('string_escape'))
            f(data)

        print 
