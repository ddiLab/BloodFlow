#!/bin/sh
NODES=`cat $COBALT_NODEFILE | wc -l`
PROCS=$((NODES * 12))
#CONNOR: modify this file path
EMB_PATH=/home/mectro/BloodFlow/examples/Shear
mpirun -f $COBALT_NODEFILE -n $PROCS $EMB_PATH/shear $EMB_PATH/in.lmp4cell
