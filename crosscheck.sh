#!/bin/bash
# ©2011 Massachusetts Institute of Technology
for f in $@; do 
    bd=`dirname $f`
    samp=`echo ${f} | sed 's/.*winps_crossval_//;s/-/ /g;s/.pkl//'`
    samppy=`echo $samp | sed 's/ /,/g'`
    sampfn=`echo $samp | sed 's/ /-/g'`
    totest=`python -c "print ' '.join(str(x) for x in set(range(24))-set([$samppy]))"`
    echo "---------- $sampfn ----------"
    for i in $totest; do
        cur=${bd}/winps_crossval.${i}
        echo "Testing on sample: ${i}"
        ./volatility newmicrodo -f ${cur}.mem -e ${cur}.env -n '[ mem.alloc(1024) ]' -m ${f} | grep Output | awk '{ print $2 }' | tee ${cur}.${sampfn}.out
    done
done
