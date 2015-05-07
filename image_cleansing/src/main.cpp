#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include "image_data/file_reader.hpp"
#include "image_data/image.hpp"
#include "svm/svm.hpp"
#include "main.hpp"

using namespace cv;
using namespace std;
using namespace ic;

int main(int argc, char** argv) {
    // Load valid images
    std::vector<ic::Image> images = FileReader::loadImages();

    // Extract features and train SVM
    std::vector<Feature> features = extractFeautes(images);
    trainSVM(features);

    return 0;
}

std::vector<Feature> extractFeautes(std::vector<ic::Image> images) {
    std::vector<Feature> features;
    return features;
}

void trainSVM(std::vector<Feature> features) {
    // Set up training data
    float labels[4] = { 1.0, -1.0, -1.0, -1.0 };
    cv::Mat labelsMat(4, 1, CV_32FC1, labels);

    float trainingData[4][2] = { { 501, 10 }, { 255, 10 }, { 501, 255 }, { 10, 501 } };
    cv::Mat trainingDataMat(4, 2, CV_32FC1, trainingData);

    SVMLearner svm;
    svm.train(trainingDataMat, labelsMat);
//	svm.plotDecisionRegions();
}
