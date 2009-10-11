#!/usr/bin/env python

import IPython
from collections import defaultdict
import cPickle as pickle

from qemu_trace import get_insns, TraceEntry
from translate_uop import uop_to_py,uop_to_py_out
from qemu_data import defines,uses,is_jcc,is_dynjump,memrange
from pprint import pprint
import sys,csv

class NotFoundException(Exception):
    pass

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
    """
    if start == -1: start = len(insns) - 1
    if output_track: outbufs = set(bufs)

    work = set(bufs)
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
        
            insn.mark()

    if debug: print "Working set at end:", work
    #return slice

# Objects to represent translation blocks, single instructions,
# and loops.
class TB(object):
    def __init__(self, label):
        self.label = label
        self.body = []
    def range(self):
        return (min(ins[0] for ins in self.body),
                max(ins[0] for ins in self.body))
    def has_slice(self):
        return any(ins.in_slice for _,ins in self.body)
    def has_dynjump(self):
        return any(is_dynjump(ins.op) for _, ins in self.body)
    def has_jcc(self):
        return any(is_jcc(ins.op) for _, ins in self.body)
    def get_jcc(self):
        return [ins for _,ins in self.body if is_jcc(ins.op)][0]
    def _label_str(self):
        return hex(self.label) if isinstance(self.label,int) else "'%s'" % self.label
    def __str__(self):
        s = repr(self)
        for _,insn in self.body:
            s += "\n%s%s" % (" * " if insn.in_slice else "   ", repr(insn))
        return s
    def __repr__(self):
        return ("[TB @%s%s]" % (self._label_str(), " *" if self.has_slice() else ""))

def make_tbs(trace):
    """Splits the trace into translation blocks.

    Splits the trace into translation blocks by creating a new TB
    every time the HEAD_EIP micro-op is seen.

    Returns a list of TB objects.
    """
    tbs = []
    current_tb = TB("__start__")
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

def del_slice_range(start, end, slice):
    to_excise = range(start, end+1)
    return dict( (k, slice[k]) for k in slice if not k in to_excise )

def set_input(trace, inbufs):
    inbufs = set(inbufs)
    for i,te in trace:
        uses_set = set(uses(te))
        if uses_set & inbufs:
            defs = defines(te)
            assert len(defs) == 1
            te.op = "IFLO_SET_INPUT"
            te.args = [defs[0], 0]

def find_last_jcc(tbs):
    for i, insn in tbs[-1].body:
        if is_jcc(insn.op) or is_dynjump(insn.op):
            return i,insn
    raise NotFoundException

def make_slice_cfg(tbs):
    cfg = defaultdict(list)
    #sltbs = [t for t in tbs if t.has_slice()]
    sltbs = tbs
    for i in range(len(sltbs)-1):
        cfg[sltbs[i].label].append(sltbs[i+1])
    # Successor of the last one is "__end__"
    #cfg[sltbs[-1].label].append(TB("__end__"))
    #print hex(sltbs[-1].label),cfg[sltbs[-1].label]
    return cfg

def add_branch_deps(trace, cfg, tbdict):
    for c in cfg:
        if len(cfg[c]) > 1:
            #idx, insn = find_last_jcc(tbdict[c])
            # Mark the jumps so we can include them in the translation
            for t in tbdict[c]:
                for i,isn in t.body:
                    if is_jcc(isn.op) or is_dynjump(isn.op):
                        dynslice(trace, uses(isn), start=i)
                        isn.mark()

def num_branches(cfg):
    return sum(len(cfg[c]) > 1 for c in cfg)

def find_whole_tb(tbs):
    """Find a TB that contains all the uOps; this means one where 
    the conditional jump is not taken."""
    for t in tbs:
        for i,insn in t.body:
            if is_jcc(insn.op) and not insn.args[-1]: # last arg of a jcc is the taken/not taken flag
                return t
    return None

def get_taken_tb(tbs):
    return [t for t in tbs if t.has_jcc() and t.get_jcc().args[-1]][0]

def get_not_taken_tb(tbs):
    return [t for t in tbs if t.has_jcc() and not t.get_jcc().args[-1]][0]

def get_branch_target(tbs):
    for t in tbs:
        for i,(n,insn) in enumerate(t.body):
            if is_jcc(insn.op) and insn.args[-1]:
                # We found a taken branch. If it jumps to
                # another TB, then it should have the following insn seq:
                #    IFLO_GOTO_TB1
                #    IFLO_MOVL_EIP_IM [target]
                #    IFLO_MOVL_T0_IM [tbid | taken]
                #    IFLO_EXIT_TB
                # But the last 3 may be omitted if direct block chaining is in effect
                if i+2 >= len(t.body):
                    # TB ends too soon, return unknown target and let
                    # the caller handle it (it should be the TB that follows
                    # this one)
                    return "unknown"
                elif (t.body[i+1][1].op == 'IFLO_GOTO_TB1' and
                    t.body[i+2][1].op == 'IFLO_MOVL_EIP_IM'):
                    return t.body[i+2][1].args[0]
                else:
                    # There are more instructions in this TB; this means
                    # QEMU is jumping inside a single TB. The caller will
                    # need to split the TB for us
                    return "split"
    return None

def simple_translate(tbs):
    """Translate a set of TBs at once.
       Note: does not handle conditional jump case.
       
       Returns: a list of translated instructions (as strings)
    """
    s = []
    assert all( len(t.body) == len(tbs[0].body) for t in tbs )
    for insns in zip(*[t.body for t in tbs]):
        if any( insn.in_slice for _,insn in insns ):
            s.append(str(insns[0][1]))
    return s

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

    dynslice(trace, outbufs, output_track=True)
    tbs = make_tbs(trace)
    tbdict = defaultdict(list)
    for t in tbs:
        tbdict[t.label].append(t)

    # Make the initial slice CFG
    cfg = make_slice_cfg(tbs)

    # Look for branch dependencies and add them in
    # This may alter the slice CFG by adding new TBs and
    # even including new branches. So we iterate it until
    # we reach a fixed point.
    add_branch_deps(trace, cfg, tbdict)
    newcfg = make_slice_cfg(tbs)
    while num_branches(newcfg) != num_branches(cfg):
        print "Fixed point not reached, iterating again"
        cfg = newcfg
        add_branch_deps(trace, cfg, tbdict)
        newcfg = make_slice_cfg(tbs)
    cfg = newcfg
    print "Fixed point found."
    
    embedshell = IPython.Shell.IPShellEmbed()
    embedshell()

    transdict = {}
    for i in range(len(tbs)-1):
        transdict[tbs[i].label] = "raise Goto(%s)" % tbs[i+1]._label_str()
    transdict[tbs[-1].label] = "raise Goto('__end__')"

    # For label generation
    labels = 0
    # Iterate over the finished CFG and translate each TB
    for c in cfg:
        cur = tbdict[c]
        next = cfg[c]

        print "Translating %s" % repr(cur[0])

        # For TB splitting
        gen_new_tb = False
        remainder = []

        if len(next) == 1:
            #s =  "\n".join("%s" % insn for _,insn in cur[0].body if insn.in_slice)
            s = simple_translate(cur)
            s.append("raise Goto(%s)" % next[0]._label_str())
            transdict[cur[0].label] = "\n".join(s)
        elif cur[0].has_dynjump():
            transdict[cur[0].label] = "\n".join(simple_translate(cur))
        else:
            # There must be a jump to a variable target (ie conditional or dynamic jump)
            # We need to combine the known instances of this TB and resolve its targets
            
            # Get a "complete" specimen, or if that's not available
            # just pick the first one
            whole_tb = find_whole_tb(cur) or cur[0]

            taken = get_branch_target(cur)
            if taken == "unknown":
                # Pick the successor to any TB where the jump is taken
                tb = get_taken_tb(cur)
                i = tbs.index(tb)
                taken = tbs[i+1]._label_str()
            elif taken == "split":
                gen_new_tb = True
                tb = get_taken_tb(cur)
                taken = "'label%d'" % labels
                labels += 1
                for i, (n,insn) in enumerate(tb.body):
                    if is_jcc(insn.op):
                        remainder = tb.body[i+1:]
                        break

            # Find the natural successor. This is the TB that is next
            # when the jump is not taken
            tb = get_not_taken_tb(cur)
            i = tbs.index(tb)
            succ = tbs[i+1]._label_str()

            s = ''
            for i,ins in whole_tb.body:
                if not ins.in_slice: continue
                if is_jcc(ins.op):
                    s += "if (%s): raise Goto(%s)\n" % (ins, taken)
                else:
                    s += str(ins) + "\n"
            s += "raise Goto(%s)" % succ
            transdict[cur[0].label] = s
            
            if gen_new_tb:
                tak = get_taken_tb(cur)
                i = tbs.index(tak)
                succ = tbs[i+1]._label_str()
                print "Translating [TB @%s]" % taken
                s = "\n".join("%s" % r for _,r in remainder if r.in_slice)
                s += "\n" + "raise Goto(%s)" % succ
                transdict[eval(taken)] = s

    f = open(sys.argv[2],'w')
    pickle.dump(transdict, f)
    f.close()

