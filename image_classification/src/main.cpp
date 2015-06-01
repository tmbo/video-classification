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
    /**
     * CONFIG
     */

    // text file with frames and classes
    std::string txtFile = "../resources/files.txt";
    // sequence size
    int sequenceSize = 2;
    // Result file
    std::string outputFile = "../resources/results.txt";
    // Caffee parameters
    bool cpuSetting = true;
    std::string preModel = "../resources/model/bvlc_reference_caffenet.caffemodel";
    std::string protoFile = "../resources/model/deploy.prototxt";
    cv::Size size(227, 227);
    int channels = 3;
    bool isDebug = false;
    std::string resultLayer = "argmax";
    std::string dataLayer = "data";

    /**
     * MAIN
     */

    // Get all sequences with frame paths and class value
    std::vector<Sequence> sequences;
    FileReader::load(txtFile, sequenceSize, sequences);

    // Initialize classifier
    std::cout << "Initialize classifier ..." << std::endl;
    CaffeClassifier classifier(cpuSetting, preModel, protoFile, size, channels, isDebug);

    // Predict all images
    FileWriter writer(outputFile);

    std::cout << "Predicting " << sequences.size() << " sequences ..." << std::endl;

    for (int i = 0; i < sequences.size(); i++) {
        Sequence sequence = sequences[i];
        std::vector<cv::Mat> frames = std::vector<cv::Mat>();
        for (int j = 0; j < sequence.frames.size(); j++) {
            std::string frame = sequence.frames[j];
            cv::Mat img = cv::imread(frame);
            frames.push_back(img);
        }

        std::vector<float> predictions;
        classifier.predict(frames, resultLayer, dataLayer, predictions);

        writer.writeLine("True class: " + sequence.clazz);
        for (int k = 0; k < predictions.size(); k++) {
            writer.writeLine(predictions[k]);
        }
    }

    writer.close();

    return 0;
}
