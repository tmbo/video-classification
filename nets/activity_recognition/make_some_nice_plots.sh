#!/usr/bin/env bash

if [ $# -ne 1 ]; then
    echo "Need exactly one parameter. Exiting."
	exit 1
fi

TRAINING_LOG=$1
TRAINING_LOG_NAME=`basename $TRAINING_LOG`

if [ `whoami` == "mpss2015" ]; then
	echo "Please run from a local user."
	exit 1
else
	scp mpss2015@172.16.23.193:~/video-classification/nets/activity_recognition/$TRAINING_LOG $TRAINING_LOG_NAME &> /dev/null

	if [ $? -ne 0 ]; then
		echo "Could not download training log file '$TRAINING_LOG'"
		exit 2
	fi
	$CAFFE_ROOT/tools/extra/parse_log.sh $TRAINING_LOG_NAME
	gnuplot -e "filename='$TRAINING_LOG_NAME'" -p plot_log.gnuplot
fi
