#!/usr/bin/env python

import pydasm

def disas(tb, until=None):
    if until:
        while True:
            insns = [ e for i,e in tb.body if e.op == 'IFLO_INSN_BYTES' ]
            dis = [ (e.args[0],pydasm.get_instruction(e.args[1].decode('hex'), pydasm.MODE_32))
                for e in insns ]
            for addr, insn in dis:
                print "%08x   %s" % (addr, pydasm.get_instruction_string(insn, pydasm.FORMAT_INTEL, addr))
            if until(tb): return
            tb = tb.next
    else:
        insns = [ e for i,e in tb.body if e.op == 'IFLO_INSN_BYTES' ]
        dis = [ (e.args[0],pydasm.get_instruction(e.args[1].decode('hex'), pydasm.MODE_32))
                for e in insns ]
        for addr, insn in dis:
            print "%08x   %s" % (addr, pydasm.get_instruction_string(insn, pydasm.FORMAT_INTEL, addr))

