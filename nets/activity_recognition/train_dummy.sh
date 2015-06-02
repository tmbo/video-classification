#!/usr/bin/env bash

export CAFFE_ROOT="$HOME/caffe-tmbo"

$CAFFE_ROOT/build/tools/caffe train \
    -solver $CAFFE_ROOT/examples/activity_recognition/dummy_solver.prototxt -gpu 0 2>&1 | less

