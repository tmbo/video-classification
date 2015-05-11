#include <ofextract/BroxOpticalFlow.h>

#include <iostream>
#include <sstream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/gpu/gpu.hpp"

namespace ofextract
{
    BroxOpticalFlow::BroxOpticalFlow(std::string sourceFolder, std::string outputFolder):
    m_sourceFolder(sourceFolder)
    , m_outputFolder(outputFolder)
    {
    }

    BroxOpticalFlow::~BroxOpticalFlow()
    {
    }

    void BroxOpticalFlow::runAll()
    {
        std::stringstream folder;
        folder << m_sourceFolder << "/%d.jpg";
        cv::VideoCapture videoCapture(folder.str());

        cv::Mat CurrentFrame;

        cv::Mat PreviousFrameGray;
        cv::Mat CurrentFrameGray;

        cv::Mat PreviousFrameGrayFloat;
        cv::Mat CurrentFrameGrayFloat;

        // Prepare receiving variables
        cv::gpu::GpuMat FlowXGPU;
        cv::gpu::GpuMat FlowYGPU;

        cv::Mat FlowX;
        cv::Mat FlowY;

        cv::Mat NormImageOutputFlowX;
        cv::Mat NormImageOutputFlowY;

        // Create optical flow object
        cv::gpu::BroxOpticalFlow OpticalFlowGPU = cv::gpu::BroxOpticalFlow(0.197f, 50.0f, 0.8f, 10, 77, 10);

        int i = 0;

        int outputId = 0;

        while (videoCapture.grab()){
            std::cout << "extracting image " << i << std::endl;
            i++;

            CurrentFrameGray.copyTo(PreviousFrameGray);
            videoCapture.retrieve(CurrentFrame);
            cv::cvtColor(CurrentFrame, CurrentFrameGray, CV_RGB2GRAY);

            if (PreviousFrameGray.empty() || CurrentFrameGray.empty()){
                continue;
            }

            PreviousFrameGray.convertTo(PreviousFrameGrayFloat, CV_32FC1, 1.0/255.0);
            CurrentFrameGray.convertTo(CurrentFrameGrayFloat, CV_32FC1, 1.0/255.0);

            // Upload images to GPU
            cv::gpu::GpuMat PreviousFrameGPU(PreviousFrameGrayFloat);
            cv::gpu::GpuMat CurrentFrameGPU(CurrentFrameGrayFloat);

            // Perform optical flow
            OpticalFlowGPU(PreviousFrameGPU, CurrentFrameGPU, FlowXGPU, FlowYGPU);

            // Download flow from GPU
            FlowXGPU.download(FlowX);
            FlowYGPU.download(FlowY);

            FlowX.convertTo(NormImageOutputFlowX, CV_8UC1, m_alpha, m_beta);
            FlowY.convertTo(NormImageOutputFlowY, CV_8UC1, m_alpha, m_beta);

            std::stringstream outputX;
            std::stringstream outputY;
            outputX << m_outputFolder << "/X" << outputId << ".jpg";
            outputY << m_outputFolder << "/Y" << outputId << ".jpg";

            outputId++;

            // write output
            cv::imwrite(outputX.str(), NormImageOutputFlowX);
            cv::imwrite(outputY.str(), NormImageOutputFlowY);

            // // debug output
            // cv::imshow("flowX", FlowX);
            // cv::imshow("flowY", FlowY);
            // cv::waitKey(0);

        }
    }

} // namespace ofextract
