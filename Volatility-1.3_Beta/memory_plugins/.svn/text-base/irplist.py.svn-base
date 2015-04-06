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
from forensics.object import obj_size
from forensics.win32.tasks import process_list
from forensics.win32.lists import list_entry
from forensics.win32.obj import obj_name
from vutils import *

IRP_MAJORS = [
    'IRP_MJ_CREATE',
    'IRP_MJ_CREATE_NAMED_PIPE',
    'IRP_MJ_CLOSE',
    'IRP_MJ_READ',
    'IRP_MJ_WRITE',
    'IRP_MJ_QUERY_INFORMATION',
    'IRP_MJ_SET_INFORMATION',
    'IRP_MJ_QUERY_EA',
    'IRP_MJ_SET_EA',
    'IRP_MJ_FLUSH_BUFFERS',
    'IRP_MJ_QUERY_VOLUME_INFORMATION',
    'IRP_MJ_SET_VOLUME_INFORMATION',
    'IRP_MJ_DIRECTORY_CONTROL',
    'IRP_MJ_FILE_SYSTEM_CONTROL',
    'IRP_MJ_DEVICE_CONTROL',
    'IRP_MJ_INTERNAL_DEVICE_CONTROL',
    'IRP_MJ_SHUTDOWN',
    'IRP_MJ_LOCK_CONTROL',
    'IRP_MJ_CLEANUP',
    'IRP_MJ_CREATE_MAILSLOT',
    'IRP_MJ_QUERY_SECURITY',
    'IRP_MJ_SET_SECURITY',
    'IRP_MJ_POWER',
    'IRP_MJ_SYSTEM_CONTROL',
    'IRP_MJ_DEVICE_CHANGE',
    'IRP_MJ_QUERY_QUOTA',
    'IRP_MJ_SET_QUOTA',
    'IRP_MJ_PNP',
    'IRP_MJ_MAXIMUM_FUNCTION',
]

IRP_MAJORS = dict(enumerate(IRP_MAJORS))
IRP_MAJORS.update((v,k) for k,v in IRP_MAJORS.items())

IRP_MJ_FILE_SYSTEM_CONTROL_MINORS = {
    'IRP_MN_USER_FS_REQUEST': 0x00,
    'IRP_MN_MOUNT_VOLUME': 0x01,
    'IRP_MN_VERIFY_VOLUME': 0x02,
    'IRP_MN_LOAD_FILE_SYSTEM': 0x03,
}
IRP_MJ_FILE_SYSTEM_CONTROL_MINORS.update((v,k) for k,v in IRP_MJ_FILE_SYSTEM_CONTROL_MINORS.items())

IRP_MJ_POWER_MINORS = {
    'IRP_MN_WAIT_WAKE': 0x00,
    'IRP_MN_POWER_SEQUENCE': 0x01,
    'IRP_MN_SET_POWER': 0x02,
    'IRP_MN_QUERY_POWER': 0x03,
}
IRP_MJ_POWER_MINORS.update((v,k) for k,v in IRP_MJ_POWER_MINORS.items())

IRP_MJ_PNP_MINORS = {
    'IRP_MN_START_DEVICE': 0x00,
    'IRP_MN_QUERY_REMOVE_DEVICE': 0x01,
    'IRP_MN_REMOVE_DEVICE': 0x02,
    'IRP_MN_CANCEL_REMOVE_DEVICE': 0x03,
    'IRP_MN_STOP_DEVICE': 0x04,
    'IRP_MN_QUERY_STOP_DEVICE': 0x05,
    'IRP_MN_CANCEL_STOP_DEVICE': 0x06,
    'IRP_MN_QUERY_DEVICE_RELATIONS': 0x07,
    'IRP_MN_QUERY_INTERFACE': 0x08,
    'IRP_MN_QUERY_CAPABILITIES': 0x09,
    'IRP_MN_QUERY_RESOURCES': 0x0A,
    'IRP_MN_QUERY_RESOURCE_REQUIREMENTS': 0x0B,
    'IRP_MN_QUERY_DEVICE_TEXT': 0x0C,
    'IRP_MN_FILTER_RESOURCE_REQUIREMENTS': 0x0D,
    'IRP_MN_READ_CONFIG': 0x0F,
    'IRP_MN_WRITE_CONFIG': 0x10,
    'IRP_MN_EJECT': 0x11,
    'IRP_MN_SET_LOCK': 0x12,
    'IRP_MN_QUERY_ID': 0x13,
    'IRP_MN_QUERY_PNP_DEVICE_STATE': 0x14,
    'IRP_MN_QUERY_BUS_INFORMATION': 0x15,
    'IRP_MN_DEVICE_USAGE_NOTIFICATION': 0x16,
    'IRP_MN_SURPRISE_REMOVAL': 0x17,
    'IRP_MN_QUERY_LEGACY_BUS_INFORMATION': 0x18,
}
IRP_MJ_PNP_MINORS.update((v,k) for k,v in IRP_MJ_PNP_MINORS.items())

IRP_MINORS = {
    'IRP_MJ_FILE_SYSTEM_CONTROL': IRP_MJ_FILE_SYSTEM_CONTROL_MINORS,
    'IRP_MJ_POWER': IRP_MJ_POWER_MINORS,
    'IRP_MJ_PNP': IRP_MJ_PNP_MINORS,
}
IRP_MINORS.update((IRP_MAJORS[k],v) for k,v in IRP_MINORS.items())

IRP_FLAGS = {
    'IRP_NOCACHE': 0x1,
    'IRP_PAGING_IO': 0x2,
    'IRP_MOUNT_COMPLETION': 0x2,
    'IRP_SYNCHRONOUS_API': 0x4,
    'IRP_ASSOCIATED_IRP': 0x8,
    'IRP_BUFFERED_IO': 0x10,
    'IRP_DEALLOCATE_BUFFER': 0x20,
    'IRP_INPUT_OPERATION': 0x40,
    'IRP_SYNCHRONOUS_PAGING_IO': 0x40,
    'IRP_CREATE_OPERATION': 0x80,
    'IRP_READ_OPERATION': 0x100,
    'IRP_WRITE_OPERATION': 0x200,
    'IRP_CLOSE_OPERATION': 0x400,
    'IRP_DEFER_IO_COMPLETION': 0x800,
    'IRP_OB_QUERY_NAME': 0x1000,
    'IRP_HOLD_DEVICE_QUEUE': 0x2000,
    'IRP_RETRY_IO_COMPLETION': 0x4000
}

SL_FLAGS = {
    'SL_PENDING_RETURNED': 0x01,
    'SL_INVOKE_ON_CANCEL': 0x20,
    'SL_INVOKE_ON_SUCCESS': 0x40,
    'SL_INVOKE_ON_ERROR': 0x80
}

SL_PARAMS = {
    'IRP_MJ_CREATE': 'Create',
    'IRP_MJ_READ': 'Read',
    'IRP_MJ_WRITE': 'Write',
    'IRP_MJ_QUERY_INFORMATION': 'QueryFile',
    'IRP_MJ_SET_INFORMATION': 'SetFile',
    'IRP_MJ_QUERY_VOLUME_INFORMATION': 'QueryVolume',
    'IRP_MJ_DEVICE_CONTROL': 'DeviceIoControl',
    'IRP_MJ_INTERNAL_DEVICE_CONTROL': 'DeviceIoControl',
    'IRP_MN_MOUNT_VOLUME': 'MountVolume',
    'IRP_MN_VERIFY_VOLUME': 'VerifyVolume',
    'IRP_MJ_INTERNAL_DEVICE_CONTROL': 'Scsi',
    'IRP_MN_QUERY_DEVICE_RELATIONS': 'QueryDeviceRelations',
    'IRP_MN_QUERY_INTERFACE': 'QueryInterface',
    'IRP_MN_QUERY_CAPABILITIES': 'DeviceCapabilities',
    'IRP_MN_FILTER_RESOURCE_REQUIREMENTS': 'FilterResourceRequirements',
    'IRP_MN_READ_CONFIG and IRP_MN_WRITE_CONFIG': 'ReadWriteConfig',
    'IRP_MN_SET_LOCK': 'SetLock',
    'IRP_MN_QUERY_ID': 'QueryId',
    'IRP_MN_QUERY_DEVICE_TEXT': 'QueryDeviceText',
    'IRP_MN_DEVICE_USAGE_NOTIFICATION': 'UsageNotification',
    'IRP_MN_WAIT_WAKE': 'WaitWake',
    'IRP_MN_POWER_SEQUENCE': 'PowerSequence',
    'IRP_MN_SET_POWER and IRP_MN_QUERY_POWER': 'Power',
    'IRP_MN_START_DEVICE': 'StartDevice',
}

def get_flags(flags, val):
    return [f for f in flags if flags[f] & val]

def irp_stacks(irp):
    stack_start = irp.offset + irp.size()
    stack_loc_size = obj_size(types, '_IO_STACK_LOCATION')
    return [ Object('_IO_STACK_LOCATION', stack_start + i*stack_loc_size,
             irp.vm, irp.parent, irp.profile) for i in range(irp.StackCount) ]

def print_irp_info(irp):
    if irp.Type != 6:
        print "Invalid IRP type, probably corrupt. Skipping."
        return

    print "IRP @%x MDL @%x with %d stacks (current: %d)" % (irp.offset,
                                                            irp.MdlAddress.offset,
                                                            irp.StackCount,
                                                            irp.CurrentLocation)
    print "Flags: %s" % (", ".join(get_flags(IRP_FLAGS, irp.Flags)))
    for i,s in enumerate(irp_stacks(irp)):
        if s.MajorFunction in IRP_MAJORS:
            major = IRP_MAJORS[s.MajorFunction]
            if (s.MajorFunction in IRP_MINORS and
                s.MinorFunction in IRP_MINORS[s.MajorFunction]):
                minor = IRP_MINORS[s.MajorFunction][s.MinorFunction]
            else:
                minor = s.MinorFunction
        else:
            major = s.MajorFunction
            minor = s.MinorFunction
        
        print "  (%s,%s)" % (major, minor),

        if minor in SL_PARAMS:
            params = SL_PARAMS[minor]
        elif major in SL_PARAMS:
            params = SL_PARAMS[major]
        else:
            params = 'Others'
        parmobj = getattr(s.Parameters, params)
        parmstrs = [ "%s: %s" % (m,str(getattr(parmobj,m))) for m in parmobj.get_member_names() ]
        print "    Flags: %d Control: %s" % (s.Flags, ", ".join(get_flags(SL_FLAGS, s.Control)))
        print "    " + " ".join(parmstrs)

        print "   ",
        if s.FileObject.is_valid():
            print "File: %s" % s.FileObject.FileName.Buffer,
        if s.DeviceObject.is_valid():
            print "Device: %s" % obj_name(s.vm, types, s.DeviceObject.v()),
        print

def each_thread(proc):
    return list_entry(proc.vm, types, proc.profile,
                      proc.ThreadListHead.v(), '_ETHREAD',
                      fieldname='ThreadListEntry')

def each_irp(thread):
    return list_entry(thread.vm, types, thread.profile,
                      thread.IrpList.v(), '_IRP',
                      fieldname='ThreadListEntry')

class irplist(forensics.commands.command):

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
        return  "Print process virtual sizes"
    
    def execute(self):
	(addr_space, symtab, types) = load_and_identify_image(self.op,
            self.opts)

        theProfile = Profile()

        pslist = [ Object("_EPROCESS", p, addr_space, profile=theProfile)
                   for p in process_list(addr_space, types, symtab) ]

        for p in pslist:
            for t in each_thread(p):
                print "********** %s (%x:%x) **********" % (p.ImageFileName,
                                                            t.Cid.UniqueProcess.v(),
                                                            t.Cid.UniqueThread.v())
                for irp in each_irp(t):
                    print_irp_info(irp)
