#!/usr/bin/env python
# ©2011 Massachusetts Institute of Technology

import os.path
import sys, time
from fixedint import *
from telnetlib import Telnet
from cStringIO import StringIO

try:
    from collections import namedtuple
except ImportError:
    from namedtuple import namedtuple

Segment = namedtuple("Segment", "selector base limit flags")

def load_env(reglines):
    env_file = StringIO("\n".join(reglines))
    regs = {}
    # GP registers
    for i in range(2):
        current = env_file.readline().strip().split()
        for c in current:
            var,val = c.split('=')
            regs[var] = UInt(int(val, 16))
    # EIP and EFLAGS
    current = env_file.readline().strip()
    eip,efl,_,cpl,_ = current.split(" ", 4)
    for c in [eip,efl,cpl]:
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

    # Check for the sysenter MSRs and DR regs
    current = env_file.readline().strip().split()
    while current:
        if current[0].startswith('sysenter') or current[0].startswith('DR'):
            for c in current:
                var,val = c.split('=')
                regs[var] = UInt(int(val, 16))
        current = env_file.readline().strip().split()

    return regs

class QEMU:
    def __init__(self, host='localhost', port=3457):
        self.qemu = Telnet(host, port)
        self.qemu.read_until("(qemu)")

    def get_response(self):
        data = self.qemu.read_until("(qemu)")
        return data.splitlines()[1:-1]

    def get_regs(self):
        self.qemu.write("info registers\n")
        return load_env(self.get_response())

    def cmd(self, c):
        self.qemu.write(c + "\n")
        self.get_response()
        

host = 'localhost'
port = 3457
base_dir = sys.argv[1]
interval = int(sys.argv[2])

q = QEMU(host, port)

while True:
    regs = {}
    # Get a userland snapshot
    while True:
        q.cmd('stop')
        q.qemu.write('info registers\n')
        reglines = q.get_response()
        regs = load_env(reglines)
        if regs['CPL'] == 3:
            print "Found an appropriate context"
            break
        q.cmd('c')

    ts = str(int(time.time()))
    fname_base = os.path.join(base_dir, ts)

    print "Saving memory/CPU to %s.{env,mem}" % fname_base

    memfile = os.path.abspath(fname_base+'.mem')
    envfile = os.path.abspath(fname_base+'.env')
    q.cmd('memsavep %s' % memfile)
    q.cmd('c')
    f = open(envfile, 'w')
    f.write("\n".join(reglines))
    f.close()


    time.sleep(interval)
