#!/usr/bin/env bash

export CAFFE_ROOT="$HOME/caffe-tmbo"

#WEIGHTS=$CAFFE_ROOT/models/bvlc_reference_caffenet/bvlc_reference_caffenet.caffemodel

$CAFFE_ROOT/build/tools/caffe train \
    -solver /home/mpss2015/video-classification/nets/flow/solver.prototxt 2>&1 | tee uc101-flow.tlog | grep -v --line-buffered "Gradient clipping: scaling down gradients" | less 
#    -weights $WEIGHTS

