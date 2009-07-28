#!/usr/bin/env python

import re
opt = re.compile(r' ?OPTIONAL ?')
import sys,csv
from syscall_table import name_to_ordinal,ordinal_to_name

if len(sys.argv) > 1:
    reader = csv.reader(open(sys.argv[1]))
else:
    reader = csv.reader(sys.stdin)

syscalls = {}

for row in reader:
    rets,func = row[:2]
    args = [r.strip() for r in row[2:]]
    ordinals = name_to_ordinal[func]

    for o in ordinals:
        syscalls[o] = "NTSTATUS %s (%s)" % (func, opt.sub("", ", ".join(args)))

for o in sorted(ordinal_to_name):
    if o in syscalls:
        print "%d %s;" % (o, syscalls[o])
    else:
        print "%d missing %s" % (o, ordinal_to_name[o])
