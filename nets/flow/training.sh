#!/bin/bash

cd ~/caffe-tmbo

./build/tools/caffe train --solver=examples/flow/solver.prototxt --weights=models/bvlc_reference_caffenet/bvlc_reference_caffenet.caffemodel > examples/flow/output 2>&1

