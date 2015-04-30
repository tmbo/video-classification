#include <ofextract/BroxOpticalFlow.h>

#include <iostream>
#include <sstream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/gpu/gpu.hpp"

namespace ofextract
{
    BroxOpticalFlow::BroxOpticalFlow()
    {
    }

    BroxOpticalFlow::~BroxOpticalFlow()
    {
    }

    void BroxOpticalFlow::runTest()
    {
        cv::VideoCapture videoCapture("/opt/data_sets/UCF-101/frames/Archery/v_Archery_g01_c01/%d.jpg");

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

            cv::normalize(FlowX, NormImageOutputFlowX, 0, 255, cv::NORM_MINMAX, CV_8UC1); // TODO constant normalization factor needed
            cv::normalize(FlowY, NormImageOutputFlowY, 0, 255, cv::NORM_MINMAX, CV_8UC1); // TODO constant normalization factor needed

            std::stringstream outputX;
            std::stringstream outputY;
            outputX << "/opt/data_sets/UCF-101/broxoptflow/Archery/v_Archery_g01_c01/X" << i << ".jpg";
            outputY << "/opt/data_sets/UCF-101/broxoptflow/Archery/v_Archery_g01_c01/Y" << i << ".jpg";

            // write output
            cv::imwrite(outputX.str(), NormImageOutputFlowX);
            cv::imwrite(outputY.str(), NormImageOutputFlowY);


            cv::imshow("flowX", FlowX);
            cv::imshow("flowY", FlowY);
            cv::waitKey(0);

            // Use FlowX and FlowY in further processing
            //...
        }
    }

} // namespace ofextract
