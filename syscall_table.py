from collections import defaultdict

syscalls = [
    [line.strip() for line in open("nt.txt")],
    [line.strip() for line in open("gdi.txt")],
]

ordinal_to_name = {}
for i,table in enumerate(syscalls):
    for j,func in enumerate(table):
        ordinal_to_name[(i << 12) | j] = func

name_to_ordinal = defaultdict(list)
for i in ordinal_to_name:
    name_to_ordinal[ordinal_to_name[i]].append(i)
