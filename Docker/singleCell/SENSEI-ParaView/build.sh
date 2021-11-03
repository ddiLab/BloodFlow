#!/bin/bash

# individual users must edit this
# environment variable to match their
# system or docker hub username
export IMAGENAME="srizzi/singlecell-sensei-paraview:latest"

docker build -t $IMAGENAME .
