#!/usr/bin/env bash

export CAFFE_ROOT="$HOME/caffe-tmbo"

WEIGHTS=$CAFFE_ROOT/models/bvlc_reference_caffenet/bvlc_reference_caffenet.caffemodel

$CAFFE_ROOT/build/tools/caffe train \
    -solver $MP_HOME/nets/activity_recognition/solver.prototxt \
    -weights $WEIGHTS 2>&1 | tee uc101.tlog | less
#    -snapshot snapshots/_iter_50000.solverstate

