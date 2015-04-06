#!/bin/bash
# ©2011 Massachusetts Institute of Technology
for f in $@; do 
    bd=`dirname $f`
    samp=`echo ${f} | sed 's/.*winps_crossval_//;s/-/ /g;s/.pkl//'`
    samppy=`echo $samp | sed 's/ /,/g'`
    sampfn=`echo $samp | sed 's/ /-/g'`
    echo "---------- $sampfn ----------"
    for i in $samp; do
        cur=${bd}/winps_crossval.${i}
        out=${bd}/testontrain/winps_crossval.${i}.${sampfn}.out
        echo "Testing on sample: ${i}"
        ./volatility newmicrodo -f ${cur}.mem -e ${cur}.env -n '[ mem.alloc(1024) ]' -m ${f} | grep Output | awk '{ print $2 }' | tee ${out}
    done
done
