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
    std::vector<ic::Image> images = FileReader::loadImages();
    std::vector<ic::Image> invalidImages;

    // Remove images with size 0 x 0
    checkImageSize(images, invalidImages);

    // Extract features and train SVM
    std::vector<Feature> features = extractFeautes(images);
    trainSVM(features);

    return 0;
}

void checkImageSize(std::vector<ic::Image>& images, std::vector<ic::Image>& invalidImages) {
    for(std::vector<ic::Image>::size_type i = 0; i != images.size(); i++) {
        std::string file = images[i].file;

        Mat image = imread(file, CV_LOAD_IMAGE_COLOR);
        cv::Size s = image.size();

        if (s.height == 0 && s.width == 0) {
//            cout << "Image size 0: " << images[i].file << endl;
            invalidImages.push_back(images[i]);
        }
    }

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
