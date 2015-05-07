#pragma once

#include "forwarddeclarations.hpp"

typedef struct {
    bool clazz;
    cv::Mat values;
} Feature;

std::vector<ic::Image> getImageFilePaths();
void checkImageSize(std::vector<ic::Image>& images, std::vector<ic::Image>& invalidImages);
std::vector<Feature> extractFeautes(std::vector<ic::Image> images);
void trainSVM(std::vector<Feature> features);
