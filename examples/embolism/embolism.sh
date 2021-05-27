#!/bin/sh
NODES=`cat $COBALT_NODEFILE | wc -l`
PROCS=$((NODES * 12))
#CONNOR: modify this file path
set(EMBOLISM_ROOT "/home/murphyc/BloodFlow/examples/embolism")
cd ${EMBOLISM_ROOT}  
mpirun -f $COBALT_NODEFILE -n $PROCS ./embolism in.embolism
