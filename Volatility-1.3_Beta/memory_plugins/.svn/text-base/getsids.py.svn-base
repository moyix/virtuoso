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
from forensics.object2 import *
from forensics.win32.tasks import process_list, process_imagename, process_pid
from vutils import *
from struct import unpack

def find_sid_re(sid_string, sid_re_list):
    for reg, name in sid_re_list:
        if reg.search(sid_string):
            return name

well_known_sid_re = [
  (re.compile(r'S-1-5-[0-9-]+-500'), 'Administrator'),
  (re.compile(r'S-1-5-[0-9-]+-501'), 'Guest'),
  (re.compile(r'S-1-5-[0-9-]+-502'), 'KRBTGT'),
  (re.compile(r'S-1-5-[0-9-]+-512'), 'Domain Admins'),
  (re.compile(r'S-1-5-[0-9-]+-513'), 'Domain Users'),
  (re.compile(r'S-1-5-[0-9-]+-514'), 'Domain Guests'),
  (re.compile(r'S-1-5-[0-9-]+-515'), 'Domain Computers'),
  (re.compile(r'S-1-5-[0-9-]+-516'), 'Domain Controllers'),
  (re.compile(r'S-1-5-[0-9-]+-517'), 'Cert Publishers'),
  (re.compile(r'S-1-5-[0-9-]+-520'), 'Group Policy Creator Owners'),
  (re.compile(r'S-1-5-[0-9-]+-533'), 'RAS and IAS Servers'),
  (re.compile(r'S-1-5-5-[0-9]+-[0-9]+'), 'Logon Session'),
]

well_known_sids = {
  'S-1-0': 'Null Authority',
  'S-1-0-0': 'Nobody',
  'S-1-1': 'World Authority',
  'S-1-1-0': 'Everyone',
  'S-1-2-0': 'Users with the ability to log in locally',
  'S-1-2': 'Local Authority',
  'S-1-3': 'Creator Authority',
  'S-1-3-0': 'Creator Owner',
  'S-1-3-1': 'Creator Group',
  'S-1-3-2': 'Creator Owner Server',
  'S-1-3-3': 'Creator Group Server',
  'S-1-4': 'Non-unique Authority',
  'S-1-5': 'NT Authority',
  'S-1-5-1': 'Dialup',
  'S-1-5-2': 'Network',
  'S-1-5-3': 'Batch',
  'S-1-5-4': 'Interactive',
  'S-1-5-6': 'Service',
  'S-1-5-7': 'Anonymous',
  'S-1-5-8': 'Proxy',
  'S-1-5-9': 'Enterprise Domain Controllers',
  'S-1-5-10': 'Principal Self',
  'S-1-5-11': 'Authenticated Users',
  'S-1-5-12': 'Restricted Code',
  'S-1-5-13': 'Terminal Server Users',
  'S-1-5-18': 'Local System',
  'S-1-5-19': 'NT Authority',
  'S-1-5-20': 'NT Authority',
  'S-1-5-32-544': 'Administrators',
  'S-1-5-32-545': 'Users',
  'S-1-5-32-546': 'Guests',
  'S-1-5-32-547': 'Power Users',
  'S-1-5-32-548': 'Account Operators',
  'S-1-5-32-549': 'Server Operators',
  'S-1-5-32-550': 'Print Operators',
  'S-1-5-32-551': 'Backup Operators',
  'S-1-5-32-552': 'Replicators',
  'S-1-5-32-554': 'BUILTIN\Pre-Windows 2000 Compatible Access',
  'S-1-5-32-555': 'BUILTIN\Remote Desktop Users',
  'S-1-5-32-556': 'BUILTIN\Network Configuration Operators',
  'S-1-5-32-557': 'BUILTIN\Incoming Forest Trust Builders',
  'S-1-5-32-557': 'BUILTIN\Incoming Forest Trust Builders',
  'S-1-5-32-558': 'BUILTIN\Performance Monitor Users',
  'S-1-5-32-559': 'BUILTIN\Performance Log Users',
  'S-1-5-32-560': 'BUILTIN\Windows Authorization Access Group',
  'S-1-5-32-561': 'BUILTIN\Terminal Server License Servers',
  'S-1-5-32-562': 'BUILTIN\Distributed COM Users',
}

token_types = {
  '_TOKEN' : [ 0xa8, {
    'UserAndGroupCount' : [ 0x4c, ['unsigned long']],
    'UserAndGroups' : [ 0x68, ['pointer', ['_SID_AND_ATTRIBUTES']]],
} ],
# Pre-SP2 values
#  '_TOKEN' : [ 0x90, {
#    'UserAndGroupCount' : [ 0x40, ['unsigned long']],
#    'UserAndGroups' : [ 0x5c, ['pointer', ['_SID_AND_ATTRIBUTES']]],
#} ],
  '_SID_AND_ATTRIBUTES' : [ 0x8, {
    'Sid' : [ 0x0, ['pointer', ['void']]],
    'Attributes' : [ 0x4, ['unsigned long']],
} ],
  '_SID' : [ 0xc, {
    'Revision' : [ 0x0, ['unsigned char']],
    'SubAuthorityCount' : [ 0x1, ['unsigned char']],
    'IdentifierAuthority' : [ 0x2, ['_SID_IDENTIFIER_AUTHORITY']],
    'SubAuthority' : [ 0x8, ['array', 1, ['unsigned long']]],
} ],
  '_SID_IDENTIFIER_AUTHORITY' : [ 0x6, {
    'Value' : [ 0x0, ['array', 6, ['unsigned char']]],
} ],
  '_EX_FAST_REF' : [ 0x4, {
    'Object' : [ 0x0, ['pointer', ['void']]],
    'Value' : [ 0x0, ['unsigned long']],
} ],
}

class getsids(forensics.commands.command):

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
        return  "Print the SIDs owning each process"
    
    def execute(self):
        from vtypes import xpsp2types
        xpsp2types['_EPROCESS'][1]['Token'] = [ 0xc8, ['_EX_FAST_REF']]
        theProfile = Profile()
        theProfile.add_types(token_types)
	
        (addr_space, symtab, types) = load_and_identify_image(self.op,
            self.opts)

        pslist = process_list(addr_space, types, symtab)

        for proc_addr in pslist:
            proc = Object("_EPROCESS", proc_addr, addr_space, profile=theProfile)
            if not proc.Token.is_valid():
                name = process_imagename(addr_space, types, proc_addr)
                pid = process_pid(addr_space, types, proc_addr)
                print "%s (%d): Token unreadable" % (name,pid)
                continue
            tok_addr = proc.Token.Value & ~0x7
            tok = Object('_TOKEN', tok_addr, proc.vm, None, profile=theProfile)

            sids = []
            sid_addr = tok.UserAndGroups.offset
            sid_size = obj_size(token_types, '_SID_AND_ATTRIBUTES')
            for i in range(tok.UserAndGroupCount):
                sids.append(Object('_SID_AND_ATTRIBUTES', sid_addr, proc.vm, None, profile=theProfile))
                sid_addr += sid_size

            for sa in sids:
                sid = sa.Sid.dereference_as('_SID')
                subauth_addr = sid.get_member_offset('SubAuthority')
                subauths = unpack("<%dI" % sid.SubAuthorityCount, proc.vm.read(subauth_addr, sid.SubAuthorityCount*4))
                sid_string = "S-" + "-".join(str(i) for i in (sid.Revision,sid.IdentifierAuthority.Value[-1])+subauths)
                if sid_string in well_known_sids:
                    sid_name = " (%s)" % well_known_sids[sid_string]
                else:
                    sid_name_re = find_sid_re(sid_string, well_known_sid_re)
                    if sid_name_re:
                        sid_name = " (%s)" % sid_name_re
                    else:
                        sid_name= ""
                
                print "%s (%d): %s%s" % (proc.ImageFileName, proc.UniqueProcessId.v(), sid_string, sid_name)
