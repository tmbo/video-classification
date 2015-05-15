#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <caffe/caffe.hpp>
#include <caffe/proto/caffe.pb.h>
#include <caffe/util/io.hpp>
#include "io/file_reader.hpp"
#include "io/file_writer.hpp"
#include "classification/caffe_classifier.hpp"
#include "main.hpp"

using namespace ic;

int main(int argc, char** argv) {
    // Image directory
    std::string dir = "../resources/images/";
    // Result file
    std::string outputFile = "results.txt";
    // Caffee parameters
    bool cpuSetting = true;
    std::string preModel = "../resources/model/bvlc_reference_caffenet.caffemodel";
    std::string protoFile = "../resources/model/deploy.prototxt";
    cv::Size size(277, 277);
    int channels = 3;
    bool isDebug = true;
    std::string result_layer = "prob";
    std::string data_layer = "data";

    // Get all image paths
    std::vector<std::string> imagePaths;
    FileReader::load(dir, imagePaths);

    // Initialzie classifier
    CaffeClassifier classifier(cpuSetting, preModel, protoFile, size, channels, isDebug);

    // Predict all images
    FileWriter writer(outputFile);

    for (int i = 0; i < imagePaths.size(); i++) {
        std::string imagePath = imagePaths[i];

        std::vector<float> predictions;
        cv::Mat img = cv::imread(imagePath);
        classifier.predict(img, result_layer, data_layer, predictions);

        writer.writeLine(imagePath);
        for (int j = 0; j < predictions.size(); j++) {
            writer.writeLine(predictions[j]);
        }
    }

    writer.close();
}
