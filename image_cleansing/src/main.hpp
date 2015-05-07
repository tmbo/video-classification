#pragma once

#include "forwarddeclarations.hpp"

typedef struct {
    bool clazz;
    cv::Mat values;
} Feature;

std::vector<ic::Image> getImageFilePaths();
std::vector<Feature> buildHistogram(std::vector<ic::Image> images);
std::vector<Feature> extractFeautes(std::vector<ic::Image> images);
void trainSVM(std::vector<Feature> features);
