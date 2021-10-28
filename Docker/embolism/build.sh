#!/bin/bash

export IMAGENAME="srizzi/embolism:latest"

docker build -t $IMAGENAME .
