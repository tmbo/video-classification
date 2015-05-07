#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include "image_data/file_reader.hpp"
#include "image_data/image.hpp"
#include "output/file_writer.hpp"
#include "svm/svm.hpp"
#include "histogram/histogram.hpp"
#include "main.hpp"

using namespace cv;
using namespace std;
using namespace ic;

int main(int argc, char** argv) {
    // Train
    train();

    // Predict
//    predict();
}

/**
 * TRAINING
 */

void train() {
    FileWriter fileWriter;

    // Load valid images
    std::vector<ic::Image> images = FileReader::loadImages(fileWriter);

    // Extract features and train SVM
    std::vector<Feature> features = extractFeatures(images);
    trainSVM(features);

    fileWriter.close();
}

void trainSVM(std::vector<Feature> features) {
    SvmTrainingData data = convertFeatures(features);

    // Set up training data
    cv::Mat labelsMat(data.labelSize, 1, CV_32FC1, &data.labels[0]);
    cv::Mat trainingDataMat(data.labelSize, data.trainingDataSize, CV_32FC1, &data.values[0]);

//    showMat(labelsMat, 1);
//    showMat(trainingDataMat, 2);
//    cv::waitKey(0);

    SVMLearner svm;
    svm.train(trainingDataMat, labelsMat);
    svm.save();

//	svm.plotDecisionRegions();
}

/**
 * PREDICTING
 */

void predict() {
    FileWriter fileWriter;

    // Load valid images
    std::vector<ic::Image> images = FileReader::loadImages(fileWriter);

    // Extract features
    std::vector<Feature> features = extractFeatures(images);

    // Predict classes

    fileWriter.close();
}

/**
 * HELPER FUNCTIONS
 */

std::vector<Feature> extractFeatures(std::vector<ic::Image> images) {
    std::vector<Feature> histogramFeatures = buildHistogram(images);

    return histogramFeatures;
}

std::vector<Feature> buildHistogram(std::vector<ic::Image> images) {
    std::vector<Feature> features;
    Histogram histBuilder(8);

    for (int i = 0; i < images.size(); i++) {
        Mat image = imread(images[i].file, CV_LOAD_IMAGE_COLOR);
        Mat hist = histBuilder.buildHistogram(image);

        Feature feature = {images[i].clazz, hist};
        features.push_back(feature);
    }

    return features;
}

SvmTrainingData convertFeatures(std::vector<Feature> features) {
    std::vector<float> labels;
    std::vector<std::vector<float>> trainingData;

    int trainingDataSize = 0;
    for (int i = 0; i < features.size(); i++) {
        Feature feature = features[i];

        std::vector<float> array;
        array.assign((float*) feature.values.datastart, (float*) feature.values.dataend);
        trainingDataSize = array.size();

        trainingData.push_back(array);
        labels.push_back(feature.clazz);
    }

    SvmTrainingData data = {labels, trainingData, features.size(), trainingDataSize};
    return data;
}

void showMat(cv::Mat &img, int id) {
    cv::namedWindow(std::to_string(id), cv::WINDOW_NORMAL);
    cv::imshow(std::to_string(id), img);
    cv::waitKey(1);
}
