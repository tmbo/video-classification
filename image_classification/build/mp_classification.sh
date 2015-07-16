#!/bin/sh

make

rm ../resources/result_mp.txt 2> /dev/null

LC_ALL=C ./imageclassification \
	/home/mpss2015m_1/video-classification/nets/activity_recognition/experiments/20150701-133744_uncropped_10fps_full_dr7/_iter_70000.caffemodel \
    /home/mpss2015m_1/video-classification/nets/activity_recognition/experiments/20150701-133744_uncropped_10fps_full_dr7/deploy.prototxt \
    /extra/data_sets/UCF-101/frames_cropped/txt_files/lstm/test_lstm_head_605248.txt \
    16 \
    64 \
    argmax \
    101 \
    ../resources/result_mp.txt

vim ../resources/result_mp.txt
