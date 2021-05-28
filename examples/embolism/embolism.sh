#!/bin/sh
NODES=`cat $COBALT_NODEFILE | wc -l`
PROCS=$((NODES * 12))
#CONNOR: modify this file path
cd /home/murphyc/BloodFlow/examples/embolism 
mpirun -f $COBALT_NODEFILE -n $PROCS ./embolism in.embolism
