#!/bin/sh
NODES=`cat $COBALT_NODEFILE | wc -l`
PROCS=$((NODES * 12))
#CONNOR: modify this file path
EMB_PATH=/home/mectro/BloodFlow/examples/embolism
mpirun -f $COBALT_NODEFILE -n $PROCS $EMB_PATH/embolism $EMB_PATH/in.embolism
