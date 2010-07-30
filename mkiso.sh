#!/bin/bash

nm=`basename ${1}`.iso
genisoimage -J -r -o ${nm} $1
echo "Saved to ${nm}"
