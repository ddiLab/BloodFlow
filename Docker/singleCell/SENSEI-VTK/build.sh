#!/bin/bash

# individual users must edit this
# environment variable to match their
# system or docker hub username
export IMAGENAME="srizzi/singlecell:latest"

docker build -t $IMAGENAME .
