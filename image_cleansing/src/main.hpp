#pragma once

#include "forwarddeclarations.hpp"

typedef struct {
    float clazz;
    cv::Mat values;
} Feature;

typedef struct {
    std::vector<float> labels;
    std::vector<std::vector<float>> values;
    int labelSize;
    int trainingDataSize;
} SvmData;

void train();
void predict();
SvmData convertFeatures(std::vector<Feature> features);
std::vector<ic::Image> getImageFilePaths();
std::vector<Feature> buildHistogram(std::vector<ic::Image> images);
std::vector<Feature> extractFeatures(std::vector<ic::Image> images);
void trainSVM(std::vector<Feature> features);
void showMat(cv::Mat &img, int id = 0);
