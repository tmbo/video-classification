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
    std::string preModel = "/home/knub/Repositories/video-classification/nets/activity_recognition/caffenet/_iter_70000.caffemodel";
    std::string protoFile = "/home/knub/Repositories/video-classification/nets/activity_recognition/caffenet/deploy.prototxt";
//    std::string preModel = "/home/mpss2015/video-classification/nets/activity_recognition/snapshots/conv4-0.01_after-0.1_lstm-1024-512/_iter_100000.caffemodel";
//    std::string protoFile = "/home/mpss2015/video-classification/nets/activity_recognition/caffenet/deploy.prototxt";
    cv::Size size(224, 224);
    int channels = 3;
    bool isDebug = false;

    CaffeClassifier classifier(true, preModel, protoFile, size, channels, isDebug);

//    cv::Mat inputImage = cv::imread("/home/knub/Repositories/video-classification/nets/activity_recognition/caffenet/c_050.jpg", CV_LOAD_IMAGE_COLOR);
    cv::Mat inputImage = cv::imread("/home/knub/Repositories/video-classification/nets/activity_recognition/caffenet/bl_015.jpg", CV_LOAD_IMAGE_COLOR);

    cv::Mat heatMap;
    classifier.predictHeatMap(inputImage, 38, "prediction", "data", heatMap);

    cv::Mat normalized;
    cv::normalize(heatMap, normalized, 0.0, 255.0, cv::NORM_MINMAX, CV_8UC1);
    std::cout << heatMap << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << normalized << std::endl;

    std::vector<int> compressionParams;
    compressionParams.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compressionParams.push_back(9);

    cv::Mat colorMap = normalized;
//    applyColorMap(normalized, colorMap, cv::COLORMAP_AUTUMN);

    cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Heatmap", cv::WINDOW_AUTOSIZE);

    cv::imshow("Image", inputImage);
    cv::imshow("Heatmap", colorMap);
    cv::waitKey(0);


    cv::imwrite("/home/knub/Repositories/video-classification/nets/activity_recognition/caffenet/heatmap.png", colorMap, compressionParams);
    return 0;
}
