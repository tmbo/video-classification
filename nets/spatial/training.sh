#!/bin/bash

cd ~/caffe-tmbo

./build/tools/caffe train --solver=examples/spatial/solver.prototxt --weights=models/bvlc_reference_caffenet/bvlc_reference_caffenet.caffemodel > examples/spatial/output 2>&1

