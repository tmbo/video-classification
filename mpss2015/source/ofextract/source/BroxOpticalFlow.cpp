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
        folder << m_sourceFolder << "%d.jpg";
        cv::VideoCapture videoCapture(folder.str());

        cv::Mat CurrentFrame;

        cv::Mat PreviousFrameGray;
        cv::Mat CurrentFrameGray;

        cv::Mat PreviousFrameGrayFloat; // Has an image in format CV_32FC1
        cv::Mat CurrentFrameGrayFloat;  // Has an image in format CV_32FC1

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

        double alpha = 5.0;
        double beta = 127.0;

        double min = 0.0;
        double max = 0.0;
        double totalMin = 0.0;
        double totalMax = 0.0;

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

            cv::minMaxLoc(FlowX, &min, &max);
            if (min < totalMin)
            {
                totalMin = min;
                std::cout << "new total min: " << totalMin << std ::endl; 
            }
            if (max > totalMax)
            {
                totalMax = max;
                std::cout << "new total max: " << totalMax << std ::endl; 
            }
            cv::minMaxLoc(FlowY, &min, &max);
            if (min < totalMin)
            {
                totalMin = min;
                std::cout << "new total min: " << totalMin << std ::endl; 
            }
            if (max > totalMax)
            {
                totalMax = max;
                std::cout << "new total max: " << totalMax << std ::endl; 
            }

            FlowX.convertTo(NormImageOutputFlowX, CV_8UC1, alpha, beta);
            FlowY.convertTo(NormImageOutputFlowY, CV_8UC1, alpha, beta);

            std::stringstream outputX;
            std::stringstream outputY;
            outputX << "/opt/data_sets/UCF-101/broxoptflow/Archery/v_Archery_g01_c01/X" << i << ".jpg";
            outputY << "/opt/data_sets/UCF-101/broxoptflow/Archery/v_Archery_g01_c01/Y" << i << ".jpg";

            // write output
            cv::imwrite(outputX.str(), NormImageOutputFlowX);
            cv::imwrite(outputY.str(), NormImageOutputFlowY);

            // cv::imshow("flowX", FlowX);
            // cv::imshow("flowY", FlowY);
            // cv::waitKey(0);

            // Use FlowX and FlowY in further processing
            //...
        }
    }

} // namespace ofextract
