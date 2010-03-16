#!/usr/bin/env python

import IPython
from collections import defaultdict
import cPickle as pickle

import time, datetime

import flow
import networkx

from qemu_trace import get_insns, TraceEntry
from translate_uop import uop_to_py,uop_to_py_out
from qemu_data import defines,uses,is_jcc,is_dynjump,is_memop,memrange
from pprint import pprint
from optparse import OptionParser
from gzip import GzipFile

import sys,csv

# Domain knowledge: a list of memory allocation functions
# Format: address, number of argument bytes, name
mallocs = [
    (0x7c9105d4, 12, 'RtlAllocateHeap'),
]

# Some small utility functions

def first(cond, l):
    """Return the first item in L for which cond is true.
       Return None if cond is not true for any item"""
    for e in l:
        if cond(e): return e
    return None

def counter():
    i = 0
    while True:
        yield i
        i += 1

class NotFoundException(Exception):
    pass

# The slicing functions. Note that dynslice is just a wrapper around
# multislice; if you need to slice with respsect to a bunch of things
# at once, it's much faster to use multislice rather than multiple
# calls to dynslice.

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

    multislice(insns, [ (start, bufs) ], output_track, debug)

def multislice(insns, worklist, output_track=False, debug=False):
    wlist = worklist[:]
    wlist.sort()
    start, bufs = wlist.pop()
    
    if wlist:
        next_i, next_bufs = wlist.pop()
    else:
        next_i = -1

    if start == -1: start = len(insns) - 1
    if output_track: outbufs = set(bufs)
    
    work = set(bufs)
    if debug: print "Initial working set:",work
    for i,insn in reversed(insns[:start+1]):
        if i == next_i:
            work = work | set(next_bufs)
            if wlist:
                next_i, next_bufs = wlist.pop()

        defs_set = set(defines(insn))
        uses_set = set(uses(insn))

        if debug: print repr(insn)

        if defs_set & work:
            if debug: print "Overlap with working set: %s" % (defs_set & work)
            work = (work - defs_set) | uses_set
            if debug: print "Adding to slice: %s" % repr(insn)
            if debug: print "Current WS:",work
            
            # TODO: allow multiple outputs by separating outbufs into
            # a dict of (label => memrange) pairs
            if output_track and defs_set & outbufs:
                if debug: print "Accounted for %d of %d output bytes" % (len(defs_set & outbufs), len(outbufs))
                if debug: print "Instruction: %s" % repr(insn)
                outbufs -= defs_set
                insn.set_output_label("out")
        
            insn.mark()

    if debug: print "Working set at end:", work
    #return slice

def linked_vars(insns, sink, source, start=-1, end=0, debug=False):
    if debug: print "Linking vars sink: %s source: %s between trace positions %d and %d" % (sink, source, end, start)
    if start == -1: start = len(insns) - 1

    work = set([sink])
    for i,insn in reversed(insns[end:start+1]):
        defs_set = set(defines(insn))
        uses_set = set(uses(insn))
        # For this one special case we DON'T want to track
        # the derivation of the address of a buffer.
        if is_memop(insn):
            uses_set -= set(["A0"])

        if defs_set & work:
            work = (work - defs_set) | uses_set
            if debug: print i,repr(insn)

    if debug: print "Working set at end:", work
    return source in work

class TB(object):
    """Represents a Translation Block"""
    def __init__(self, label):
        self.label = label
        self.body = []
    def range(self):
        return (self.start(), self.end())
    def start(self):
        return self.body[0][0]
    def end(self):
        return self.body[-1][0]
    def has_slice(self):
        return any(ins.in_slice for _,ins in self.body)
    def has_dynjump(self):
        return any(is_dynjump(ins.op) for _, ins in self.body)
    def has_jcc(self):
        return any(is_jcc(ins.op) for _, ins in self.body)
    def get_branch(self):
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

# Some utility functions for dealing with the trace:
# Loading it from a file, splitting it into TBs, etc.

def make_tbs(trace):
    """Splits the trace into translation blocks.

    Splits the trace into translation blocks by creating a new TB
    every time the HEAD_EIP micro-op is seen.

    Returns a list of TB objects.
    """
    tbs = []
    current_tb = TB("START")
    current_tb.prev = None
    for i,insn in trace:
        if insn.op == "IFLO_TB_HEAD_EIP":
            if current_tb: tbs.append(current_tb)
            current_tb = TB(insn.args[0])
            
            # Maintain forward and back pointers
            current_tb.prev = tbs[-1]
            tbs[-1].next = current_tb

            current_tb.body.append((i,insn))
        else:
            if current_tb:
                current_tb.body.append((i,insn))
    tbs.append(current_tb)
    return tbs

def make_tbdict(tbs):
    tbdict = defaultdict(list)
    for t in tbs:
        tbdict[t.label].append(t)
    tbdict = dict(tbdict)
    return tbdict

def remake_trace(trace):
    trace = list(enumerate(t for i,t in trace))
    tbs = make_tbs(trace)
    tbdict = make_tbdict(tbs)
    cfg = make_cfg(tbs)
    return trace, tbs, tbdict, cfg

def make_cfg(tbs):
    cfg = defaultdict(set)
    for i in range(len(tbs)-1):
        cfg[tbs[i].label].add(tbs[i+1].label)
    cfg[tbs[-1].label] = set()
    return dict(cfg)

def set_input(trace, inbufs):
    inbufs = set(inbufs)
    for i,te in trace:
        uses_set = set(uses(te))
        if uses_set & inbufs:
            defs = defines(te)
            assert len(defs) == 1
            te.op = "IFLO_SET_INPUT"
            te.args = [defs[0], 0]

def load_trace(infile):
    print "Loading trace into memory..."
    trace = get_insns(infile)

    # TODO: both output and input should probably not be thrown into
    # the same big buffer; instead we should do something like
    #   outbufs[label] = memrange(...)
    # This would allow us to handle multiple ins/outs

    print "Finding output buffers"
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

    print "Finding input buffers"
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

    print "Converting trace to TraceEntrys"
    # Turn the trace into a list of TraceEntry objects
    trace = list(enumerate(TraceEntry(t) for t in trace))

    # Set up the inputs
    set_input(trace, inbufs)

    return trace, inbufs, outbufs

# Helper function to determine the return address
# given a TB containing a CALL instruction.
def find_retaddr(calltb):
    for i, insn in calltb.body[::-1]:
        if insn.op.startswith('IFLO_OPS_MEM_STL'):
            return insn.args[-1]

# All this junk is for the apparently simple task of trying to
# translate a translation block (TB) into a Python code block.

def find_last_jcc(tbs):
    for i, insn in tbs[-1].body:
        if is_jcc(insn.op) or is_dynjump(insn.op):
            return i,insn
    raise NotFoundException

def find_whole_tb(tbs):
    """Find a TB that contains all the uOps; this means one where 
    the conditional jump is not taken."""
    for t in tbs:
        for i,insn in t.body:
            if is_jcc(insn.op) and not insn.args[-1]: # last arg of a jcc is the taken/not taken flag
                return t
    return None

def get_taken_tb(tbs):
    return [t for t in tbs if t.has_jcc() and t.get_branch().args[-1]][0]

def get_not_taken_tb(tbs):
    return [t for t in tbs if t.has_jcc() and not t.get_branch().args[-1]][0]

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

    # Note: may not hold in direct block chaining case
    #assert all( len(t.body) == len(tbs[0].body) for t in tbs )

    for insns in zip(*[t.body for t in tbs]):
        if any( insn.in_slice for _,insn in insns ):
            s.append(str(insns[0][1]))
    return s

def slice_trace(trace, inbufs, outbufs):
    print "Doing initial slice..."
    start_ts = time.time()
    dynslice(trace, outbufs, output_track=True)
    end_ts = time.time()
    print "Slicing done in %s, calculating control dependencies..." % datetime.timedelta(seconds=end_ts-start_ts)

    tbs = make_tbs(trace)
    tbdict = make_tbdict(tbs)

    # Calculate control dependencies
    cfg = make_cfg(tbs)

    # Post-dominator algo needs this dummy node
    cfg['ENTRY'] = ['START', tbs[-1].label]

    G = networkx.DiGraph()
    for s in cfg:
        for d in cfg[s]:
            G.add_edge(s,d)
    cdeps = flow.control_deps(G, start='ENTRY', stop=tbs[-1].label)
    
    # Remove the dummy node
    del cdeps['ENTRY']
    del cfg['ENTRY']
    
    times = []
    iteration = 0
    changed = True
    while changed:
        iteration += 1
        print "Adding control flow deps, iteration %d" % iteration
        start_ts = time.time()
        changed = False

        # Build up a list of branches we want to add
        to_add = []
        for branch in cdeps:
            if any(any(t.has_slice() for t in tbdict[dep]) for dep in cdeps[branch]):
                if not all(tb.get_branch().in_slice for tb in tbdict[branch]):
                    to_add.append(branch)
                    changed = True

        if not to_add: break

        # Now add them
        wlist = []
        for br in to_add:
            for t in tbdict[br]:
                for i,isn in t.body:
                    if is_jcc(isn.op) or is_dynjump(isn.op):
                        wlist.append( (i, uses(isn)) )
                        #dynslice(trace, uses(isn), start=i)
                        isn.mark()
                        break

        multislice(trace, wlist)

        end_ts = time.time()
        elapsed = datetime.timedelta(seconds = (end_ts - start_ts ))
        times.append(elapsed)
        print "Iteration %d took" % iteration, elapsed, "to add %d branches" % len(to_add)
    print "Fixed point found, control dependency analysis finished in %s" % sum(times,datetime.timedelta(0))

    return trace, tbs, tbdict, cfg

def detect_mallocs(trace, tbs, tbdict, cfg):
    EAX = "REGS_0"
    
    # Replace the mallocs with summary functions
    alloc_ctr = counter()
    alloc_calls = defaultdict(list)
    alloc_rets = []
    for m,argbytes,name in mallocs:
        for idx in range(len(tbdict[m])):  # This allows us to change the tbdict during
            tb = tbdict[m][idx]       # iteration without getting stale data.

            i = tbs.index(tb)
            callsite = tbs[i-1]
            
            # Find the return address and get its TB object
            # Note: this might fail if we have nested calls, but worry
            # about that later. The "right" solution is a shadow stack
            retaddr = find_retaddr(callsite)
            for t in tbs[i:]:
                if t.label == retaddr:
                    retsite = t
                    break

            print "Call to %#x (%s) at callsite %s returns to %s" % (m, name, callsite._label_str(), retsite._label_str())
            remove_start, remove_end = callsite.end() + 1, retsite.start()

            # We replace each call to malloc with a summary function that
            # fixes the stack (since the Windows API uses callee-cleanup)
            # and inserts an IFLO_MALLOC pseudo-op into the trace.
            alloc_label = "ALLOC_%d" % alloc_ctr.next()
            alloc_var = "VAR_" + alloc_label
            trace[remove_start:remove_end] = [
                (None, TraceEntry(('IFLO_TB_HEAD_EIP',  [alloc_label]))),
                (None, TraceEntry(('IFLO_ADDL_ESP_IM',  [argbytes+4]))), # 4 extra for retaddr
                (None, TraceEntry(('IFLO_MALLOC',       [alloc_var, alloc_label, None]))),
            ]
            
            trace, tbs, tbdict, cfg = remake_trace(trace)
            alloc_rets.append((alloc_label,retsite.label))
            alloc_calls[(m,argbytes)].append(callsite.label)

    alloc_calls = dict(alloc_calls)

    # Find the memory operations that will need to be redirected
    # to a dynamic buffer
    allocs = defaultdict(list)
    for alloc_name, site in alloc_rets:
        tb = tbdict[site][0]
        tbstart = tb.start()
        for c in cfg:
            curtb = tbdict[c][0]
            if not curtb.start() > tb.end(): continue
            for i,insn in curtb.body:
                if is_memop(insn) and linked_vars(trace, "A0", EAX, start=i, end=tbstart):
                    off = i - curtb.start()
                    to_add = [t.body[off] for t in tbdict[c]]
                    print "Adding memop %s in %s (%d instances)" % (repr(insn), repr(curtb), len(to_add))
                    allocs[alloc_name] += to_add
    allocs = dict(allocs)

    # Figure out what the "base" addr is
    for a in allocs:
        base = min(e.args[2] for i,e in allocs[a])
        # This means "Set the third argument of the IFLO_MALLOC"
        tbdict[a][0].body[2][1].args[2] = base

    for a in allocs:
        for i,insn in allocs[a]:
            insn.set_buf_label("VAR_" + a)

    return trace, tbs, tbdict, cfg

if __name__ == "__main__":
    parser = OptionParser()
    options, args = parser.parse_args()
    if not args:
        parser.error('Trace file is required')
    
    infile = open(args[0])
    trace, inbufs, outbufs = load_trace(infile)
    tbs = make_tbs(trace)
    tbdict = make_tbdict(tbs)
    cfg = make_cfg(tbs)

    embedshell = IPython.Shell.IPShellEmbed(argv=[])
    embedshell()

    # Replace mallocs with summary functions, and find memops
    # we will need to have special handling for.
    trace, tbs, tbdict, cfg = detect_mallocs(trace, tbs, tbdict, cfg)

    # Perform slicing and control dependency analysis
    trace, tbs, tbdict, cfg = slice_trace(trace, inbufs, outbufs)

    # Make sure if an insn defines output, it adds it to the output
    # space for every instance of that insn
    for t in tbs:
        if any(insn.is_output for i,insn in t.body):
            for i,x in t.body:
                if x.is_output: break
            off = i - t.start()
            label = x.label
            for tb in tbdict[t.label]:
                tb.body[off][1].set_output_label(label)

    # It's translation time!
    transdict = {}
    for i in range(len(tbs)-1):
        transdict[tbs[i].label] = "raise Goto(%s)" % tbs[i+1]._label_str()
    transdict[tbs[-1].label] = "raise Goto('__end__')"

    # For label generation
    labels = 0
    # Iterate over the finished CFG and translate each TB
    for c in cfg:
        cur = tbdict[c]
        next = list(cfg[c])

        # No need to bother with these
        if not any(t.has_slice() for t in cur):
            print "Skipping %s" % repr(cur[0])
            continue

        print "Translating %s" % repr(first(lambda x: x.has_slice(),cur))

        # For TB splitting
        gen_new_tb = False
        remainder = []

        if not next:
            # Last node (woo special cases)
            s = simple_translate(cur)
            s.append("raise Goto('__end__')")
            transdict[cur[0].label] = "\n".join(s)
        elif len(next) == 1:
            #s =  "\n".join("%s" % insn for _,insn in cur[0].body if insn.in_slice)
            s = simple_translate(cur)
            next_tb = tbdict[next[0]][0]
            s.append("raise Goto(%s)" % next_tb._label_str())
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

            # Find the fall-through successor.
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

    fname, ext = args[0].rsplit('.',1)
    fname = fname + '.pkl'
    print "Saving translated code to", fname
    f = open(fname,'w')
    pickle.dump(transdict, f)
    f.close()

