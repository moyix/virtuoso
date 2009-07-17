#!/usr/bin/env python

from qemu_trace import get_insns
from translate_uop import uop_to_py,uop_to_py_out
from qemu_data import defines,uses,is_jcc,memrange
from pprint import pprint
import sys,csv

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

# Objects to represent translation blocks, single instructions,
# and loops.
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
            self.taken = False
            #self.dump()
            #raise RuntimeError("Unable to determine taken branch")

    def prune(self, slice):
        """Prune the exemplar so that it only includes instructions
           in slice. Do not prune the loop condition."""
        newexemp = []
        for i in self.exemplar.body:
            if i in slice or i[1] == self.condition:
                newexemp.append(i)
        self.exemplar.body = newexemp

    def pos(self):
        """Returns the position where this loop goes in the trace"""
        return self.tbs[0].body[0][0]

    def to_py(self):
        loop_statements = [ "while 1:" ]
        for i,insn in self.exemplar.body:
            if insn == self.condition:
                loop_statements.append(
                    "    if %s (%s): break" % ("" if self.taken else "not", uop_to_py(self.condition))
                )
            else:
                for line in str(insn).splitlines():
                    loop_statements.append("    " + line + " # %s" % repr(insn))
        return "\n".join(loop_statements)

    def range(self):
        return (self.tbs[0].body[0][0], self.tbs[-1].body[-1][0])

    def dump(self):
        print "Condition:",repr(self.condition)
        print "Range:",self.range()
        for tb in self.tbs:
            print tb

    def __str__(self):
        return self.to_py()

def make_tbs(trace):
    """Splits the trace into translation blocks.

    Splits the trace into translation blocks by creating a new TB
    every time the HEAD_EIP micro-op is seen.

    Returns a list of TB objects.
    """
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
    """Dectect loops in the trace.
    
    Detects simple loops in the trace by finding adjacent translation
    blocks that have the same EIP.

    Returns: list of [ t_1, t_2, ... ] where each t_i is a list of TBs
        that make up the (unrolled) loop.
    """
    
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

def reroll_loops(trace, slice, debug=False):
    slice_indices = set(s[0] for s in slice)
    loops = detect_loops(trace)
    
    # Filter the loops -- ignore any that aren't part of the slice
    loops_in_slice = []
    for loop in loops:
        # Filter: For all TBs in the loop, there exists an insn in the
        # TB that is in the slice. We ignore the last loop body since
        # it just contains insns to exit the loop.
        if all( any(insn[0] in slice_indices for insn in tb.body) for tb in loop[:-1] ):
            print "Found a loop of length",len(loop),"which has data flow through it"
            loops_in_slice.append(Loop(loop))
    
    if debug:
        for i,loop in enumerate(loops_in_slice):
            print "----- Begin Loop %d -----" % i
            loop.dump()
            print "-----  End Loop %d  -----" % i

    slice_dict = dict(slice)
    for loop in loops_in_slice:
        # We need to do this for every TB because we need to specify
        # explicitly the data flow for the loop condition -- it has no
        # defines, only uses.
        loop_slice = {}
        for tb in loop.tbs:
            loop_start, loop_end = tb.range()
            tb_slice = dynslice(trace, uses(loop.condition), start=loop_end)
            loop_slice.update(tb_slice)
        if debug:
            print "Loop slice:"
            for l in sorted(loop_slice.items()): print l
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
    for s in slice:
        print repr(s)
    newslice = reroll_loops(trace, slice, debug=True)
    print "# Slice went from %d instructions to %d (loops are counted as one insn)" % (len(slice), len(newslice))
    
    print "######## BEGIN GENERATED CODE ########"
    for _, insn in newslice:
        print insn
    print "########  END GENERATED CODE  ########"
