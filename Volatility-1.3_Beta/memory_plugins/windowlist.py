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
from forensics.win32.lists import list_entry
from forensics.object import get_obj_offset
from vutils import *

gdi_types = {
  '_W32THREAD' : [ 0x28, { # ?? Found in win32k.pdb
    'pEThread' : [ 0x0, ['pointer', ['_ETHREAD']]],
    'RefCount' : [ 0x4, ['unsigned long']],
    'ptlW32' : [ 0x8, ['pointer', ['_TL']]],
    'pgdiDcattr' : [ 0xc, ['pointer', ['void']]],
    'pgdiBrushAttr' : [ 0x10, ['pointer', ['void']]],
    'pUMPDObjs' : [ 0x14, ['pointer', ['void']]],
    'pUMPDHeap' : [ 0x18, ['pointer', ['void']]],
    'dwEngAcquireCount' : [ 0x1c, ['unsigned long']],
    'pSemTable' : [ 0x20, ['pointer', ['void']]],
    'pUMPDObj' : [ 0x24, ['pointer', ['void']]],
    'pUnknownDesktopThing' : [0x40, ['pointer', ['_UNK_DESK']]],
    'MsgQueue' : [0xD0, ['_MSG_QUEUE']],
} ],
  '_UNK_DESK' : [ 0x10, {
    'pDesktopWindow' : [ 0x8, ['pointer', ['_WINDOW']]],
} ],
  '_LARGE_UNICODE_STRING' : [ 0xC, {
    'Length' : [ 0x0, ['unsigned long']],
    'MaximumLength' : [ 0x4, ['unsigned long']],
    'Buffer' : [ 0x8, ['pointer', ['unsigned short']]],
} ],
  '_WINDOW' : [ 0x90, {
    'handleId' : [ 0x0, ['unsigned long']],
    'Win32Thread' : [ 0x8, ['pointer', ['_W32THREAD']]],
    'pWndProc' : [ 0x60, ['pointer', ['void']]],
    'pNextWindow' : [ 0x2c, ['pointer', ['_WINDOW']]],
    'pChildWindow' : [ 0x38, ['pointer', ['_WINDOW']]],
    'WindowText' : [0x80, ['_LARGE_UNICODE_STRING']],
} ],
  '_MSG_QUEUE' : [ 0xC, {
    'NumberOfMessages' : [ 0x8, ['unsigned long']],
} ],
  '_TL' : [ 0xc, {
    'next' : [ 0x0, ['pointer', ['_TL']]],
    'pobj' : [ 0x4, ['pointer', ['void']]],
} ],
}

def get_threads(proc):
    return list_entry(proc.vm, types, proc.profile, 
                      proc.ThreadListHead.v(), "_ETHREAD",
                      fieldname="ThreadListEntry")

class window_list(forensics.commands.command):

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
        return  "Print window hierarchy"
    
    def print_windows(self, win, level=0):
        if not (win.is_valid() and win.v() != 0): return
        try:
            img_name = win.Win32Thread.pEThread.ThreadsProcess.ImageFileName
        except:
            img_name = None

        print "  "*level, hex(win.handleId), repr(win.WindowText.Buffer), "(%s)" % img_name, hex(win.offset)
        if win.pChildWindow.v() != 0:
            self.print_windows(win.pChildWindow, level+1)
        head = win
        win = win.pNextWindow
        while win.is_valid() and win.v() != 0 and win != head:
            try:
                img_name = win.Win32Thread.pEThread.ThreadsProcess.ImageFileName
            except:
                img_name = None
            
            print "  "*level, hex(win.handleId), repr(win.WindowText.Buffer), "(%s)" % img_name, hex(win.offset)
            if win.pChildWindow.v() != 0:
                self.print_windows(win.pChildWindow, level+1)
            win = win.pNextWindow

    def get_desktop_window(self, thrd):
        w32thread = thrd.Tcb.Win32Thread
        if not w32thread.is_valid() or w32thread.offset < 0x80000000:
            return None
        if w32thread.pUnknownDesktopThing.pDesktopWindow.v() < 0x80000000:
            return None
        print "Thread %x:%x (%s) -- %d messages" % (thrd.Cid.UniqueProcess.v(),
                                                    thrd.Cid.UniqueThread.v(),
                                                    thrd.ThreadsProcess.ImageFileName,
                                                    w32thread.MsgQueue.NumberOfMessages)
        return w32thread.pUnknownDesktopThing.pDesktopWindow

    def find_top_window(self, process_address_space, types, symtab, theProfile):
        top_windows = []
        windows = set()
        pslist = process_list(process_address_space, types, symtab)
        for p in pslist:
            proc = Object("_EPROCESS", p, process_address_space, profile=theProfile)
            win = None
            for thrd in get_threads(proc):
                win = self.get_desktop_window(thrd)
                if win: break

            if win and win not in windows:
                top_windows.append( (win, thrd, proc) )
                windows.add(win)

        return top_windows

    # Finally we override the forensics.commands.command.execute() method
    # which provides the plugins core functionality. Command line options
    # are accessed as attributes of self.opts. For example, the options 
    # provided by the standard parse would would provide the following
    # attributes: self.opts.filename, self.opts.base, self.opts.type.

    def execute(self):
        from vtypes import xpsp2types
        
        xpsp2types['_ETHREAD'][1]['ThreadListEntry'] = [ 0x22c, ['_LIST_ENTRY']]
        xpsp2types['_KTHREAD'][1]['Win32Thread'] = [ 0x130, ['pointer', ['_W32THREAD']]]

        theProfile = Profile()
        theProfile.add_types(gdi_types)

	(addr_space, symtab, types) = load_and_identify_image(self.op,
            self.opts)
        wins = self.find_top_window(addr_space,types,symtab,theProfile)
        for win, thrd, proc in wins:
            print "********** PID: %d Name: %s **********" % (proc.UniqueProcessId.v(), proc.ImageFileName)
            self.print_windows(win)

class _LARGE_UNICODE_STRING(Object):
    hasMembers = True
    name = "LARGE_UNICODE_STRING"
    
    def __new__(typ, *args, **kwargs):
        obj = object.__new__(typ)
        return obj

    def __str__(self):
        return self.Buffer

    # Custom Attributes
    def getBuffer(self):
        buf_ptr = self.get_member_offset('Buffer')
        buf_address = read_value(self.vm, 'pointer', buf_ptr)
        buf = self.vm.read(buf_address, self.Length)
        
        if buf is None: return None

        try:
            buf_sz = buf.decode('utf-16-le').encode('ascii')
        except UnicodeEncodeError:
            return buf.decode('utf-16-le')

        return buf_sz

    Buffer = property(fget=getBuffer)
