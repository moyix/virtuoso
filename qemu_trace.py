from translate_uop import uop_to_py,uop_to_py_out
import csv

"""Stuff to parse instruction traces produced by oiferret """

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

arg_handler = {
    "ui64": lambda x: int(x,16),
    "ui32": lambda x: int(x,16),
    "ui16": lambda x: int(x,16),
    "ui8": lambda x: int(x,16),
}

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

def get_trace(infile):
    trace = get_insns(infile)
    return list(enumerate(TraceEntry(t) for t in trace))
