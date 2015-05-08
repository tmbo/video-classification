#pragma once

#include "../forwarddeclarations.hpp"

namespace ic
{

    class SVMLearner
    {
    public:
            SVMLearner();

            void train(cv::Mat& trainingData, cv::Mat& labels);
            float predict(cv::Mat& sample);
            void plotDecisionRegions();
            void save(std::string filename = "model.xml");
            void load(std::string filename = "model.xml");

    private:
            CvSVMParams m_params;
            CvSVM m_svm;
            CvMat m_cvMat;
            cv::Mat m_classWeights;
            std::vector<float> m_classWeightsV;
    };

}
