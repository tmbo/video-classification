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
    train();
//    predict();
    predict("/home/joseph/Masterprojekt/imageNet/predict/n00452293");
}

/**
 * TRAINING
 */

void train() {
    FileWriter fileWriter;

    // Load valid images
    std::vector<ic::Image> images = FileReader::loadGoldStandardImages(fileWriter);

    // Extract features
    std::vector<Feature> features = extractFeatures(images);

    // Train SVM
    trainSVM(features);

    fileWriter.close();
}

void trainSVM(std::vector<Feature> features) {
    SvmData data;
    convertFeatures(data, features);

    float* labels = &data.labels[0];
    float* values = &data.values[0][0];

    // Set up training data
    cv::Mat labelsMat(data.labelSize, 1, CV_32FC1, labels);
    cv::Mat trainingDataMat(data.labelSize, data.valueSize, CV_32FC1, values);

    SVMLearner svm;
    svm.train(trainingDataMat, labelsMat);
    svm.save("model.xml");
}

/**
 * PREDICTING
 */

void predict(std::string dir) {
    FileWriter fileWriter;

    // Load valid images
    std::vector<ic::Image> images = FileReader::loadImages(fileWriter, dir);

    // Extract features
    std::vector<Feature> features = extractFeatures(images);

    // Predict classes
    predictSVM(features, fileWriter);

    fileWriter.close();
}

void predictSVM(std::vector<Feature> features, ic::FileWriter& fileWriter) {
    SVMLearner svm;
    svm.load("model.xml");

    for (int i = 0; i < features.size(); i++) {
        std::vector<float> vec = convertMatToVector(features[i].values);
        cv::Mat dataMat(vec.size(), 1, CV_32FC1, &vec[0]);

        if (svm.predict(dataMat) > 0.0) {
            fileWriter.writeLine(features[i].file);
        }
    }
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

        Feature feature = {images[i].clazz, hist, images[i].file};
        features.push_back(feature);
    }

    return features;
}

void convertFeatures(SvmData &data, std::vector<Feature> features) {
    data.labelSize = features.size();

    for (int i = 0; i < data.labelSize; i++) {
        Feature feature = features[i];
        std::vector<float> v = convertMatToVector(feature.values);
        data.valueSize = v.size();
        data.values.push_back(v);
        data.labels.push_back(feature.clazz);
    }
}

std::vector<float> convertMatToVector(cv::Mat values) {
    std::vector<float> vec;
    vec.assign((float*) values.datastart, (float*) values.dataend);
    return vec;
}

void showMat(cv::Mat &img, int id) {
    cv::namedWindow(std::to_string(id), cv::WINDOW_NORMAL);
    cv::imshow(std::to_string(id), img);
    cv::waitKey(1);
}
