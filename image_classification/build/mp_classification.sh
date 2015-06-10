#!/bin/sh

make

rm ../resources/result_mp.txt 2> /dev/null

./imageclassification \
    /home/mpss2015/video-classification/nets/activity_recognition/snapshots/conv4-0.01_after-0.1_lstm-1024-512/_iter_100000.caffemodel \
    /home/mpss2015/video-classification/nets/activity_recognition/caffenet/deploy.prototxt \
    /opt/data_sets/UCF-101/resized_frames/test.txt \
    16 \
    64 \
    argmax \
    101 \
    ../resources/result_mp.txt

vim ../resources/result_mp.txt
