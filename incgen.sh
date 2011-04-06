#!/bin/bash
# ©2011 Massachusetts Institute of Technology

BATCHSIZE=4

for j in `seq 0 23`; do
    subset=`seq -s " " 0 $j`
    fname=`echo $subset | sed 's/ /-/g'`
    echo "---------- $fname ----------"
    (./newslice.py -o xpsp2 `for i in $subset ; do echo -n "../experiments/crossval/winps_crossval.$i-22344 "; done` -O ../experiments/incremental/winps_incremental_$fname.pkl > ../experiments/incremental/output_$fname.txt ) &
    count=$[ ($j + 1) % $BATCHSIZE ]
    if [ $count -eq 0 ]; then wait; fi
done
