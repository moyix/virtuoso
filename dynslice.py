#!/usr/bin/env python

from translate_uop import uop_to_py,uop_to_py_out
from qemu_data import qemu_regs,qemu_segs
from pprint import pprint
import sys,csv

arg_handler = {
    "ui64": lambda x: int(x,16),
    "ui32": lambda x: int(x,16),
    "ui16": lambda x: int(x,16),
    "ui8": lambda x: int(x,16),
}

# Some generic definitions to make life easier
IGNORE = [ lambda args: [], lambda args: [] ]
def OBLIT(x):
    return [ lambda args: [x], lambda args: [] ]
def IDENT(x):
    return [ lambda args: [x], lambda args: [x] ]
def LOAD(dest,size):
    return [
        lambda args: [dest],
        lambda args: ['A0'] + memrange(args[1],size),
    ]
def STORE(src,size):
    return [
        lambda args: memrange(args[1],size),
        lambda args: [src,'A0'],
    ]
def ARITH(dest, src):
    return [
        lambda args: [dest],
        lambda args: [dest, src],
    ]

# Transfer functions
defines_uses = {
    'IFLO_OPREG_TEMPL_MOVL_A0_R': [
        lambda args: ['A0'],
        lambda args: ["REGS_%d" % args[0]],
    ],
    'IFLO_OPREG_TEMPL_MOVL_R_A0': [
        lambda args: ["REGS_%d" % args[0]],
        lambda args: ['A0'],
    ],
    'IFLO_OPREG_TEMPL_MOVL_R_T0': [
        lambda args: ["REGS_%d" % args[0]],
        lambda args: ['T0'],
    ],
    'IFLO_OPREG_TEMPL_MOVL_R_T1': [
        lambda args: ["REGS_%d" % args[0]],
        lambda args: ['T1'],
    ],
    'IFLO_OPREG_TEMPL_MOVL_T0_R': [
        lambda args: ['T0'],
        lambda args: ["REGS_%d" % args[0]],
    ],
    'IFLO_OPS_TEMPLATE_SBB_T0_T1_CC': [
        lambda args: ['T0'],
        lambda args: ['T0', 'T1'],
    ],
    'IFLO_OPS_TEMPLATE_SHR_T0_T1_CC': [
        lambda args: ['T0'],
        lambda args: ['T0', 'T1'],
    ],
    'IFLO_OPREG_TEMPL_MOVL_T1_R': [
        lambda args: ['T1'],
        lambda args: ["REGS_%d" % args[0]],
    ],
    'IFLO_SETL_T0_CC': [
        lambda args: ['T0'],
        lambda args: ['CC'],
    ],
    'IFLO_TESTL_T0_T1_CC': [
        lambda args: ['CC'],
        lambda args: ['T0', 'T1'],
    ],
    'IFLO_OPREG_TEMPL_MOVB_R_T0': [
        lambda args: ["REGS_%d" % args[0]],
        lambda args: ['T0'],
    ],
    'IFLO_OPS_TEMPLATE_MOVL_T0_DSHIFT': [
        lambda args: ['T0'],
        lambda args: ['DF'],
    ],

    # Arithmentic operations, e.g. T0 += T1
    # Define the dest, use the dest and the src
    'IFLO_XORL_T0_T1':               ARITH('T0', 'T1'),
    'IFLO_OPS_TEMPLATE_SHR_T0_T1':   ARITH('T0', 'T1'),
    'IFLO_OPS_TEMPLATE_SHL_T0_T1':   ARITH('T0', 'T1'),
    'IFLO_SUBL_T0_T1':               ARITH('T0', 'T1'),
    'IFLO_ORL_T0_T1':                ARITH('T0', 'T1'),
    'IFLO_ADDL_T0_T1':               ARITH('T0', 'T1'),
    'IFLO_ANDL_T0_T1':               ARITH('T0', 'T1'),
    'IFLO_ADDL_EDI_T0':              ARITH("REGS_%d" % qemu_regs["EDI"], 'T0'),
    'IFLO_ADDL_ESI_T0':              ARITH("REGS_%d" % qemu_regs["ESI"], 'T0'),
    
    # These can't be called with ARITH because they use args :(
    'IFLO_ADDL_A0_SEG': [
        lambda args: ['A0'],
        lambda args: ['A0', 'SEGS_%d' % args[1]],
    ],
    'IFLO_OPREG_TEMPL_ADDL_A0_R_S2': [
        lambda args: ['A0'],
        lambda args: ['A0', "REGS_%d" % args[0]],
    ],

    'IFLO_OPS_MEM_LDL_T0_A0':  LOAD('T0', 4),
    'IFLO_OPS_MEM_LDL_T1_A0':  LOAD('T1', 4),
    'IFLO_OPS_MEM_LDUW_T0_A0': LOAD('T0', 2),
    'IFLO_OPS_MEM_LDUB_T0_A0': LOAD('T0', 1),
    'IFLO_OPS_MEM_STL_T0_A0':  STORE('T0', 4),
    'IFLO_OPS_MEM_STL_T1_A0':  STORE('T1', 4),
    'IFLO_OPS_MEM_STB_T0_A0':  STORE('T0', 1),

    'IFLO_UPDATE_INC_CC': IDENT('CC'),
    'IFLO_SUBL_A0_4': IDENT('A0'),
    'IFLO_ADDL_A0_IM': IDENT('A0'),
    'IFLO_NEGL_T0': IDENT('T0'),
    'IFLO_INCL_T0': IDENT('T0'),
    'IFLO_DECL_T0': IDENT('T0'),
    'IFLO_DECL_ECX': IDENT("REGS_%d" % qemu_regs["ECX"]),
    'IFLO_ADDL_ESP_4': IDENT("REGS_%d" % qemu_regs["ESP"]),
    'IFLO_ADDL_ESP_IM': IDENT("REGS_%d" % qemu_regs["ESP"]),

    'IFLO_MOVL_A0_IM': OBLIT('A0'),
    'IFLO_MOVL_T0_IM': OBLIT('T0'),
    'IFLO_MOVL_T1_IM': OBLIT('T1'),
    'IFLO_MOVL_T0_0': OBLIT('T0'),   
    'IFLO_ADDL_ESP_IM': OBLIT("REGS_%d" % qemu_regs["ESP"]),

    # Special "set the input" pseudo-op
    'IFLO_SET_INPUT': [
        lambda args: [args[0]],
        lambda args: [],
    ],

    # Evil conditionals. We hates them.    
    # Note: if you add one here, also add it to the list
    # in is_jcc()
    'IFLO_OPS_TEMPLATE_JNZ_ECX': [
        lambda args: [],
        lambda args: ["REGS_%d" % qemu_regs["ECX"]],
    ],
    'IFLO_OPS_TEMPLATE_JZ_ECX': [
        lambda args: [],
        lambda args: ["REGS_%d" % qemu_regs["ECX"]],
    ],

    # Punt on these; will have to handle eventually
    'IFLO_OPS_TEMPLATE_JB_SUB': IGNORE,
    'IFLO_OPS_TEMPLATE_JBE_SUB': IGNORE,
    'IFLO_OPS_TEMPLATE_JZ_SUB': IGNORE,

    'IFLO_CMPL_T0_T1_CC': IGNORE,
    'IFLO_LABEL_INPUT': IGNORE,
    'IFLO_LABEL_OUTPUT': IGNORE,
    'IFLO_MOVL_EIP_IM': IGNORE,
    'IFLO_UPDATE_NEG_CC': IGNORE,
    'IFLO_GOTO_TB0': IGNORE,
    'IFLO_GOTO_TB1': IGNORE,
    'IFLO_UPDATE1_CC': IGNORE,
    'IFLO_UPDATE2_CC': IGNORE,
    'IFLO_JMP_T0': IGNORE, 
    'IFLO_TB_HEAD_EIP': IGNORE,
    'IFLO_EXIT_TB': IGNORE,
    'IFLO_INSN_DIS': IGNORE,
    'IFLO_TB_ID': IGNORE,
    'IFLO_CPUID': IGNORE,
    'IFLO_JMP_T0': IGNORE,
    'IFLO_JNZ_T0_LABEL': IGNORE,
}

def is_jcc(op):
    return op in [
        'IFLO_OPS_TEMPLATE_JNZ_ECX',
        'IFLO_OPS_TEMPLATE_JB_SUB',
        'IFLO_OPS_TEMPLATE_JBE_SUB',
        'IFLO_OPS_TEMPLATE_JZ_ECX',
    ]

def defines(insn):
    try:
        return defines_uses[insn.op][0](insn.args)
    except KeyError:
        print insn.op, "not defined"
        sys.exit(1)

def uses(insn):
    try:
        return defines_uses[insn.op][1](insn.args)
    except KeyError:
        print insn.op, "not defined"
        sys.exit(1)

def memrange(start, length):
    return [("MEM_%08x" % i) for i in range(start, start+length)]

def dynslice(insns, bufs, start=-1, output_track=False, debug=False):
    """Perform a dynamic data slice.
       
       Perform a dynamic data slice of a trace with respect to a set of
       buffers. This is basically the algorithm described in the
       K-Tracer paper.

       insns: a list of tuples: (index, TraceEntry)
       bufs: a list of outputs to be tracked
       start: an optional point in the trace at which to begin analysis.
           By default, analysis begins at the last instruction in the
           trace.
       ouput_track: mark instructions that define data in the output set.
           This calls TraceEntry.set_output_label().
       debug: enable debugging information

       Returns: a list of tuples: (index, TraceEntry)
    """
    if start == -1: start = len(insns) - 1
    if output_track: outbufs = set(bufs)

    work = set(bufs)
    slice = []
    for i,insn in reversed(insns[:start+1]):
        defs_set = set(defines(insn))
        uses_set = set(uses(insn))

        if debug: print repr(insn)

        if defs_set & work:
            if debug: print "Overlap with working set: %s" % (defs_set & work)
            work = (work - defs_set) | uses_set
            if debug: print "Adding to slice: %s" % repr(insn)
            
            # TODO: allow multiple outputs by separating outbufs into
            # a dict of (label => memrange) pairs
            if output_track and defs_set & outbufs:
                outbufs -= defs_set
                insn.set_output_label("out")
        
            slice.insert(0, (i,insn))

    if debug: print "Working set at end:", work
    return slice

def get_arg(arg):
    typ,operand = arg
    return arg_handler[typ](operand)

def convert_args(args):
    return [get_arg(a) for a in args]

def get_insns(infile):
    insns = []
    reader = csv.reader(infile)
    for row in reader:
        op = row[0]
        args = convert_args([ a.split('.') for a in row[1:] ])
        insns.append( (op, args) )
    return insns

# Loop detection:
#   Go through the trace, and look for sets of TBs where
#   HEAD_EIP is repeated. If the instructions in the body
#   are contained in the naive dataflow slice, then we
#   want to re-roll this loop.
#
#   Simple loops look like:
#   HEAD_EIP x
#   JMPC COND_Z y
#   LOOP_BODY
#   GOTO_TB0
#   HEAD_EIP y
#
#   We want to translate this into:
#   while 1:
#       if COND_Z: break
#       uop_to_py(LOOP_BODY)

class TB(object):
    def __init__(self, eip):
        self.eip = eip
        self.body = []
    def range(self):
        return (min(ins[0] for ins in self.body),
                max(ins[0] for ins in self.body))
    def _eip_str(self):
        return hex(self.eip) if self.eip is not None else "0x????????"
    def __str__(self):
        return ("[TB @%s]\n" % self._eip_str()) + "\n".join(repr(i[1]) for i in self.body)
    def __repr__(self):
        return ("[TB @%s]" % self._eip_str())

class TraceEntry(object):
    """A single entry in the trace"""
    def __init__(self, insn):
        self.op, self.args = insn
        self.label = None
        self.is_output = False

    def set_output_label(self, label):
        """Mark this instruction as defining a labelled output."""
        self.label = label
        self.is_output = True

    def __str__(self):
        s = uop_to_py(self) + " # %s" % repr(self)
        if self.is_output:
            s += "\n" + uop_to_py_out(self, self.label)
        return s
    def __repr__(self):
        return "%s(%s)" % (self.op, ",".join(a if isinstance(a,str) else hex(a) for a in self.args))
    def __eq__(self,other):
        if not isinstance(other, type(self)):
            return False
        return self.op == other.op and self.args == other.args
    def __hash__(self):
        return hash(self.op) ^ hash(self.args)

class Loop(object):
    """Represents a loop.

    Members:
        tbs: list of translation blocks in the (unrolled) loop
        condition: the jump condition that the loop depends on
        taken: whether the loop body is on the taken or not-taken branch
        exemplar: an instance of the loop body that is representative
            of the whole
    """
    def __init__(self, tbs):
        self.tbs = tbs
        self.exemplar = self.tbs[1]
        
        for i,insn in tbs[0].body:
            if is_jcc(insn.op):
                self.condition = insn
                break
        else:
            raise RuntimeError("Unable to determine loop condition")
        
        for i,insn in reversed(tbs[0].body):
            if insn.op == 'IFLO_MOVL_T0_IM':
                self.taken = bool(insn.args[0] & 1)
                break
        else:
            raise RuntimeError("Unable to determine taken branch")

    def prune(self, slice):
        """Prune the exemplar so that it only includes instructions
           in slice."""
        self.exemplar.body = filter(lambda i: i in slice, self.exemplar.body)

    def pos(self):
        """Returns the position where this loop goes in the trace"""
        return self.tbs[0].body[0][0]

    def to_py(self):
        s =  "while 1:\n"
        s += "    if %s (%s): break\n" % ("" if self.taken else "not",
                                          uop_to_py(self.condition))
        body = [ i for i in self.exemplar.body if i[1] != self.condition ]
        loop_statements = []
        for i,insn in body:
            for line in str(insn).splitlines():
                loop_statements.append("    " + line + " # %s" % repr(insn))
        s += "\n".join(loop_statements)
        return s

    def range(self):
        return (self.tbs[0].body[0][0], self.tbs[-1].body[-1][0])

    def __str__(self):
        return self.to_py()

def make_tbs(trace):
    tbs = []
    current_tb = TB(None)
    for i,insn in trace:
        if insn.op == "IFLO_TB_HEAD_EIP":
            if current_tb: tbs.append(current_tb)
            current_tb = TB(insn.args[0])
            current_tb.body.append((i,insn))
        else:
            if current_tb:
                current_tb.body.append((i,insn))
    tbs.append(current_tb)
    return tbs

def detect_loops(trace):
    tbs = make_tbs(trace)

    prev_tb = None
    loops = []
    current_loop = []

    for tb in tbs:
        if prev_tb:
            if prev_tb.eip == tb.eip:
                if not current_loop:
                    current_loop.append(prev_tb)
                current_loop.append(tb)
            else:
                if current_loop:
                    loops.append(current_loop)
                    current_loop = []
        prev_tb = tb

    return loops

def del_slice_range(start, end, slice):
    to_excise = range(start, end+1)
    return dict( (k, slice[k]) for k in slice if not k in to_excise )

def reroll_loops(trace, slice):
    slice_indices = set(s[0] for s in slice)
    loops = detect_loops(trace)
    
    # Filter the loops -- ignore any that aren't part of the slice
    loops_in_slice = []
    for loop in loops:
        # Filter: For all TBs in the loop, there exists an insn in the
        # TB that is in the slice. We ignore the last loop body since
        # it just contains insns to exit the loop.
        if all( any(insn[0] in slice_indices for insn in tb.body) for tb in loop[:-1] ):
            loops_in_slice.append(Loop(loop))

    slice_dict = dict(slice)
    for loop in loops_in_slice:
        loop_start, loop_end = loop.range()
        loop_slice = dynslice(trace, uses(loop.condition), start=loop_end)
        slice_dict.update(loop_slice)

    # Now prune the loop exemplars so they only contain sliced instructions
    newslice = sorted(slice_dict.items())
    for loop in loops_in_slice:
        loop.prune(newslice)

    # Slice surgery: get rid of the unrolled loops, and insert the
    # loop object in their place
    for loop in loops_in_slice:
        loop_start, loop_end = loop.range()
        slice_dict = del_slice_range(loop_start, loop_end, slice_dict)
        slice_dict[loop.pos()] = loop

    newslice = sorted(slice_dict.items())
    return newslice

def set_input(trace, inbufs):
    inbufs = set(inbufs)
    for i,te in trace:
        uses_set = set(uses(te))
        if uses_set & inbufs:
            defs = defines(te)
            assert len(defs) == 1
            te.op = "IFLO_SET_INPUT"
            te.args = [defs[0], 0]

if __name__ == "__main__":
    if len(sys.argv) < 2:
        infile = sys.stdin
    else:
        infile = open(sys.argv[1])

    trace = get_insns(infile)

    # TODO: both output and input should probably not be thrown into
    # the same big buffer; instead we should do something like
    #   outbufs[label] = memrange(...)
    # This would allow us to handle multiple ins/outs

    # Find the outputs
    outbufs = []
    output_insns = []
    last_op, last_args = trace.pop()
    while last_op != 'IFLO_LABEL_OUTPUT':
        last_op, last_args = trace.pop()
    while last_op == 'IFLO_LABEL_OUTPUT':
        outbufs += memrange(last_args[0], last_args[1])
        output_insns.append( (last_op, last_args) )
        last_op, last_args = trace.pop()
    trace.append( (last_op, last_args) )

    # Find the inputs
    inbufs = []
    input_insns = []
    last_op, last_args = trace.pop(0)
    while last_op != 'IFLO_LABEL_INPUT':
        last_op, last_args = trace.pop(0)
    while last_op == 'IFLO_LABEL_INPUT':
        inbufs += memrange(last_args[0], last_args[1])
        input_insns.append( (last_op, last_args) )
        last_op, last_args = trace.pop(0)
    trace.insert(0, (last_op, last_args) )

    # Turn the trace into a list of TraceEntry objects
    trace = list(enumerate(TraceEntry(t) for t in trace))

    # Set up the inputs
    set_input(trace, inbufs)

    slice = dynslice(trace, outbufs, output_track=True)
    print "# Sliced %d instructions down to %d" % (len(trace), len(slice))
    print "# Re-rolling loops..."
    newslice = reroll_loops(trace, slice)
    print "# Slice went from %d instructions to %d (loops are counted as one insn)" % (len(slice), len(newslice))
    
    print "######## BEGIN GENERATED CODE ########"
    for _, insn in newslice:
        print insn
    print "########  END GENERATED CODE  ########"
