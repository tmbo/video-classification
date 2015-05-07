#pragma once

#include "forwarddeclarations.hpp"

typedef struct {
    float clazz;
    cv::Mat values;
} Feature;

std::vector<ic::Image> getImageFilePaths();
std::vector<Feature> buildHistogram(std::vector<ic::Image> images);
std::vector<Feature> extractFeatures(std::vector<ic::Image> images);
void trainSVM(std::vector<Feature> features);
void showMat(cv::Mat &img, int id = 0);
