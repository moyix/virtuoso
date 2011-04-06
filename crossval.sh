#!/bin/bash
# ©2011 Massachusetts Institute of Technology

BATCHSIZE=4

for j in `seq 1 11`; do
echo ">>>>>>>>>> $j <<<<<<<<<<"
    for k in `seq 1 50`; do
        subset=`python -c "import random; print ' '.join(str(x) for x in random.sample(range(24), $j))"`
        fname=`echo $subset | sed 's/ /-/g'`
        echo "---------- $fname ----------"
        (./newslice.py -o xpsp2 `for i in $subset ; do echo -n "../experiments/crossval/winps_crossval.$i-22344 "; done` -O ../experiments/crossval/winps_crossval_$fname.pkl > ../experiments/crossval/output_$fname.txt ) &
        count=$[ $k % $BATCHSIZE ]
        if [ $count -eq 0 ]; then wait; fi
    done
done
