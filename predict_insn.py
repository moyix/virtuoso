#!/usr/bin/env python

import pydasm
from fixedint import *

class PredictionFailure(Exception):
    pass

PREFIX_REP = 0x03000000

x86_branches = set([pydasm.INSTRUCTION_TYPE_RET,
                    pydasm.INSTRUCTION_TYPE_JMP,
                    pydasm.INSTRUCTION_TYPE_JMPC,
                    pydasm.INSTRUCTION_TYPE_CALL])

def is_branch(insn):
    # One of the branch types or an IRET
    return insn.type in x86_branches or insn.opcode == 0xcf

def predict_next(addr, insn):
    fall_through = addr+insn.length
    
    next = []
    
    if insn.type == pydasm.INSTRUCTION_TYPE_RET:
        next = [-1]
    elif insn.type == pydasm.INSTRUCTION_TYPE_JMPC:
        jmp_target = int(UInt(fall_through)+UInt(insn.op1.immediate))
        next = [fall_through, jmp_target]
    elif (insn.type == pydasm.INSTRUCTION_TYPE_JMP or
          insn.type == pydasm.INSTRUCTION_TYPE_CALL):
        if insn.op1.type != pydasm.OPERAND_TYPE_IMMEDIATE:
            next = [-1]
        else:
            jmp_target = int(UInt(fall_through)+UInt(insn.op1.immediate))
            next = [jmp_target]
    else:
        next = [fall_through]
    
    # If this instruction uses the REP prefix, we might
    # just go back to the current instruction instead.
    if insn.flags & PREFIX_REP:
        next.append(addr)

    return next

def get_next_from_trace(trace, i):
    # Find the most recent instruction
    while trace[i].op != 'IFLO_INSN_BYTES':
        i -= 1
    
    addr, insn = trace[i].args
    insn = pydasm.get_instruction(insn.decode('hex'), pydasm.MODE_32)
    next = predict_next(addr, insn)

    if next == [-1]:
        # Based on dynamic information
        while trace[i].op != 'IFLO_JMP_T0':
            i += 1
            if trace[i].op == 'IFLO_INSN_BYTES':
                # We reached the next instruction before finding
                # a JMP_T0 -- utter failure
                raise PredictionFailure("Missing JMP_T0 before next instruction")
        next = [trace[i].args[0]]

    return next
