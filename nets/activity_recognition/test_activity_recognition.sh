#!/usr/bin/env bash

export CAFFE_ROOT="$HOME/caffe-tmbo"

WEIGHTS="/home/mpss2015/video-classification/nets/activity_recognition/snapshots/lstm-1024/_iter_70000.caffemodel"
MODEL="/home/mpss2015/video-classification/nets/activity_recognition/net.prototxt"

$CAFFE_ROOT/build/tools/caffe test \
    -model $MODEL \
    -weights $WEIGHTS \
    -gpu 0 -iterations 1000 2>&1 | tee uc101-test.tlog | less
#    -snapshot snapshots/_iter_50000.solverstate

