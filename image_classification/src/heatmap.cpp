#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <caffe/caffe.hpp>
#include <caffe/proto/caffe.pb.h>
#include <caffe/util/io.hpp>
#include <glog/logging.h>
#include "io/file_reader.hpp"
#include "io/file_writer.hpp"
#include "classification/caffe_classifier.hpp"
#include "classification/evaluation.hpp"
#include "util.hpp"
#include <boost/format.hpp>
#include <boost/bind.hpp>

using namespace ic;

int main(int argc, char** argv) {
    // Disable logging (1: log warnings, 3: log nothing)
    FLAGS_minloglevel = 1;

    // Initialize classifier
    std::cout << "Initialize classifier ..." << std::endl;
    std::string preModel = "/home/knub/Repositories/video-classification/nets/activity_recognition/caffenet/_iter_100000.caffemodel";
    std::string protoFile = "/home/knub/Repositories/video-classification/nets/activity_recognition/caffenet/deploy.prototxt";
//    std::string preModel = "/home/mpss2015/video-classification/nets/activity_recognition/snapshots/conv4-0.01_after-0.1_lstm-1024-512/_iter_100000.caffemodel";
//    std::string protoFile = "/home/mpss2015/video-classification/nets/activity_recognition/caffenet/deploy.prototxt";
    cv::Size size(227, 227);
    int channels = 3;
    bool isDebug = false;

    CaffeClassifier classifier(true, preModel, protoFile, size, channels, isDebug);

    cv::Mat inputImage = cv::imread("/home/knub/Repositories/video-classification/nets/activity_recognition/caffenet/8.jpg", CV_LOAD_IMAGE_COLOR);

    cv::Mat heatMap;
    classifier.predictHeatMap(inputImage, 18, "loss", "data", heatMap);

    double min;
    double max;
    cv::minMaxIdx(heatMap, &min, &max);
    cv::Mat color;
    heatMap.convertTo(color, CV_8UC1, 255 / (max - min), -min);
    std::cout << "Min: " << min << ", Max: " << max << std::endl;
    std::cout << color << std::endl;

    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    // this is great. It converts your grayscale image into a tone-mapped one,
    // much more pleasing for the eye
    // function is found in contrib module, so include contrib.hpp
    // and link accordingly
    cv::Mat falseColorsMap;
    applyColorMap(color, falseColorsMap, cv::COLORMAP_AUTUMN);

    cv::namedWindow("Out", cv::WINDOW_AUTOSIZE);
    cv::imshow("Out", falseColorsMap);
    cv::waitKey(0);


    cv::imwrite("/home/knub/Repositories/video-classification/nets/activity_recognition/caffenet/heatmap.png", falseColorsMap, compression_params);
    return 0;
}
