#pragma once

#include "../forwarddeclarations.hpp"

namespace ic {

class CaffeClassifier {
    public:
        CaffeClassifier(bool cpu,
                        std::string modelFile,
                        std::string protoFile,
                        cv::Size,
                        int channels,
                        bool isDebug);
        virtual ~CaffeClassifier();

        void initializeCaffeNet(bool cpu, std::string modelFile, std::string protoFile);
        void predict(std::vector<cv::Mat>& image, std::vector<int> labels, std::string resultLayer, std::string dataLayer, std::vector<float>& predictions);
        void predictHeatMap(cv::Mat& inputImage, int label, std::string predictionLayer, std::string dataLayer, cv::Mat& heatMap);

    protected:
        bool isDebug;
        bool cpu;
        std::string modelFileName;
        std::string protoFileName;
        caffe::Net<float>* caffeNet;
        cv::Size imageSize;
        int channels;
    };

}
