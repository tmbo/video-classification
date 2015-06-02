#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <caffe/caffe.hpp>
#include <caffe/proto/caffe.pb.h>
#include <caffe/util/io.hpp>
#include <glog/logging.h>
#include "io/file_reader.hpp"
#include "io/file_writer.hpp"
#include "classification/caffe_classifier.hpp"
#include "main.hpp"
#include <boost/format.hpp>

using namespace ic;


int main(int argc, char** argv) {
    // Check the number of parameters
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] << " <model> <prototxt> <txt-file> <sequence-size> <result-layer>" << std::endl;
        std::cerr << "<model>         path to the caffe model file" << std::endl;
        std::cerr << "<prototxt>      path to the deploy.prototxt" << std::endl;
        std::cerr << "<txt-file>      txt-file containing the frames and labels" << std::endl;
        std::cerr << "<sequence-size> size of the sequence" << std::endl;
        std::cerr << "<result-layer>  name of the result layer" << std::endl;

        for (int i = 0; i < argc; i++) {
            std::cout << argv[i] << std::endl;
        }

        return 1;
    }

    // Disable logging, just print warnings
    FLAGS_minloglevel = 1;

    // programm parameter
    std::string txtFile = argv[3];
    int sequenceSize = atoi(argv[4]);
    std::string outputFile = "../resources/results.txt";

    // Caffee parameters
    std::string preModel = argv[1];
    std::string protoFile = argv[2];
    bool cpuSetting = false;
    cv::Size size(227, 227);
    int channels = 3;
    bool isDebug = false;
    std::string resultLayer = argv[5];
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
    for (int i = 0; i < sequences.size(); i += 4) {

        std::cout << "Predicting sequence " << i + 1 << " - " << i + 4 << std::endl;

        std::vector<cv::Mat> frames = std::vector<cv::Mat>();
        std::vector<int> labels = std::vector<int>();

        for (int n = i; n < i + 4; n++) {
            Sequence sequence = sequences[n];

            // reading frames and labels of sequence
            for (int j = 0; j < sequence.frames.size(); j++) {
                std::string frameFile = sequence.frames[j];
                cv::Mat frame = cv::imread(frameFile);

                // check if image contains data
                if(!frame.data) {
                    std::cerr << "Warning: input image (" << frameFile << ") for caffe classifier is empty." << std::endl;
                    exit(1);
                }

                frames.push_back(frame);
                labels.push_back(atoi(sequence.clazz));
            }
        }

        // get prediction for frames
        std::vector<float> predictions;
        classifier.predict(frames, labels, resultLayer, dataLayer, predictions);

        // write predictions
        for (int k = 0; k < predictions.size(); k++) {
            boost::format line("%1% %2%");
            line % predictions[k];
            line % sequences[i + (k / 16)].clazz;
            writer.writeLine(line.str());
        }

        frames.clear();
        labels.clear();
    }   

    writer.close();
    
    return 0;
}