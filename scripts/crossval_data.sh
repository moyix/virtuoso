#!/bin/bash
# ©2011 Massachusetts Institute of Technology

for f in winps_crossval.*.*.out; do
    k=`echo ${f} | cut -d. -f3 | awk -F- '{ print NF }'`
    gt=`echo ${f} | cut -d. -f 1-2`.out 
    g=`echo ${f} | cut -d. -f 2`
    samp=`echo ${f} | cut -d. -f 3`
    pkl=winps_crossval_${samp}.pkl 
    if cmp -s ${f} ${gt} ; then
        res=Success
    else
        res=Fail
    fi
    echo ${g} ${k} ${samp} `../../scripts/pklblocks.py ${pkl}` ${res}
done
