#!/usr/bin/env bash

DATE=`date +%Y%m%d-%H%M%S`
if [ $# -ne 1 ]; then
    echo "Usage: $0 [experiment_name]"
	echo "       experiment_name: Name of the subfolder in ./experiments/ for the current experiment."
	echo "Exiting."
	exit 1
fi
FOLDER_NAME="${DATE}_$1"
TRAINING_LOG_NAME="uc101.tlog"
echo "Saving experiment in experiments/$FOLDER_NAME"

rm snapshots/*

mkdir experiments/$FOLDER_NAME
cp net.prototxt solver.prototxt deploy.prototxt train_activity_recognition.sh experiments/$FOLDER_NAME

export CAFFE_ROOT="$HOME/caffe-tmbo"

WEIGHTS=$CAFFE_ROOT/models/bvlc_reference_caffenet/bvlc_reference_caffenet.caffemodel

$CAFFE_ROOT/build/tools/caffe train \
    -solver $MP_HOME/nets/activity_recognition/solver.prototxt \
    -weights $WEIGHTS 2> $TRAINING_LOG_NAME
#    -snapshot snapshots/_iter_50000.solverstate

echo "Training finished."
echo "Copying snapshots .."
ls -v -1 snapshots/ | tail -n 2 | xargs -i mv snapshots/{} experiments/$FOLDER_NAME
echo "Parsing logs"
$CAFFE_ROOT/tools/extra/parse_log.sh $TRAINING_LOG_NAME
echo "Building plots"
gnuplot -e "filename='$TRAINING_LOG_NAME'" -p plot_log.gnuplot

mv *.png experiments/$FOLDER_NAME
rm ${TRAINING_LOG_NAME}.test ${TRAINING_LOG_NAME}.train
