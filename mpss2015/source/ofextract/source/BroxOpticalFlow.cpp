#include <ofextract/BroxOpticalFlow.h>

#include <iostream>

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

        // Create optical flow object
        cv::gpu::BroxOpticalFlow OpticalFlowGPU = cv::gpu::BroxOpticalFlow(0.197f, 50.0f, 0.8f, 10, 77, 10);

        int i = 0;

        while (videoCapture.grab()){
            std::cout << "extracting image " << i << std::endl;
            i++;

            CurrentFrameGray.copyTo(PreviousFrameGray);
            videoCapture.retrieve(CurrentFrame);
            cv::cvtColor(CurrentFrame, CurrentFrameGray, CV_RGB2GRAY);

            if (PreviousFrameGray.empty()){
                std::cout << "previous frame empty" << std::endl;
                continue;
            }

            if (CurrentFrameGray.empty()){
                std::cout << "current frame empty" << std::endl;
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

            // debug output
            // cv::imwrite("output/flowX.jpg", FlowX);
            // cv::imwrite("output/flowY.jpg", FlowY);

            cv::imshow("flowX", FlowX);
            cv::imshow("flowY", FlowY);
            cv::waitKey(0);

            // cv::FileStorage storage("output/result.yml", cv::FileStorage::WRITE);
            // storage << "FlowX" << FlowX;
            // storage << "FlowY" << FlowY;
            // storage.release();

            // Use FlowX and FlowY in further processing
            //...
        }
    }

} // namespace ofextract
