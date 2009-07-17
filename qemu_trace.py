# Stuff to parse instruction traces produced by oiferret 

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
