#pragma once

#include "forwarddeclarations.hpp"

typedef struct {
    float clazz;
    cv::Mat values;
    std::string file;
} Feature;

typedef struct {
    std::vector<float> labels;
    std::vector<std::vector<float>> values;
    int labelSize;
    int valueSize;
} SvmData;


void train();
void predict();

void trainSVM(std::vector<Feature> features);
void predictSVM(std::vector<Feature> features, ic::FileWriter& fileWriter);

std::vector<Feature> extractFeatures(std::vector<ic::Image> images);
std::vector<Feature> buildHistogram(std::vector<ic::Image> images);

SvmData convertFeatures(std::vector<Feature> features);

void showMat(cv::Mat &img, int id = 0);
