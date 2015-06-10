#!/usr/bin/env bash

export CAFFE_ROOT="$HOME/caffe-tmbo"

WEIGHTS=$CAFFE_ROOT/models/VGG_ILSVRC_19_layers/VGG_ILSVRC_19_layers.caffemodel

$CAFFE_ROOT/build/tools/caffe train \
    -solver $MP_HOME/nets/activity_recognition/solver.prototxt \
    -weights $WEIGHTS 2>&1 | tee uc101.tlog | less
#    -snapshot snapshots/_iter_50000.solverstate

