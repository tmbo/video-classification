#include <ofextract/BroxOpticalFlow.h>

#include <iostream>
#include <sstream>

#include <boost/filesystem.hpp>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/gpu/gpu.hpp"

namespace ofextract
{
    BroxOpticalFlow::BroxOpticalFlow(std::string sourceFolder, std::string resizedOutputFolder, std::string opticalFlowOutputFolder):
    m_sourceFolder(sourceFolder)
    , m_resizedOutputFolder(resizedOutputFolder)
    , m_opticalOutputFolder(opticalFlowOutputFolder)
    {
    }

    BroxOpticalFlow::~BroxOpticalFlow()
    {
    }

    void BroxOpticalFlow::runAll()
    {
        boost::filesystem::recursive_directory_iterator rdi(m_sourceFolder);
        boost::filesystem::recursive_directory_iterator end_rdi;

        std::string parentFolder = m_sourceFolder;
        bool dirProcessed = false;
        for (; rdi != end_rdi; rdi++) {
            if (boost::filesystem::is_directory((*rdi).path()))
            {
                parentFolder = (*rdi).path().string();
                dirProcessed = false;
            }
            else
            {
                if (!dirProcessed && boost::filesystem::extension((*rdi).path()).compare(".jpg") == 0)
                {
                    runOnFolder(parentFolder);
                    dirProcessed = true;
                }
            }
        }
    }

    void BroxOpticalFlow::runOnFolder(std::string currentFolder)
    {
        std::stringstream videoCapturePath;
        videoCapturePath << currentFolder << "/%3d.jpg";
        cv::VideoCapture videoCapture(videoCapturePath.str());

        std::string currentOpticalOutputFolder = replaceString(currentFolder, m_sourceFolder, m_opticalOutputFolder);
        std::string currentResizedOutputFolder = replaceString(currentFolder, m_opticalOutputFolder, m_resizedOutputFolder);
        
        // create neccessary directories
        boost::filesystem::path opticalOutputPath(currentOpticalOutputFolder);
        if(boost::filesystem::create_directories(opticalOutputPath))
        {
            std::cout << "Directory Created: " << opticalOutputPath << std::endl;
        }

        boost::filesystem::path resizedOutputPath(currentResizedOutputFolder);
        if(boost::filesystem::create_directories(resizedOutputPath))
        {
            std::cout << "Directory Created: " << resizedOutputPath << std::endl;
        }

        cv::Mat CurrentFrame;
        cv::Mat ResizedFrame;

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

        bool successX, successY;

        while (videoCapture.grab()){
            // std::cout << "extracting image " << i << std::endl;
            i++;

            CurrentFrameGray.copyTo(PreviousFrameGray);
            videoCapture.retrieve(CurrentFrame);
            cv::resize(CurrentFrame, ResizedFrame, cv::Size(227, 227));

            cv::cvtColor(ResizedFrame, CurrentFrameGray, CV_RGB2GRAY);

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

            int flowFormat = CV_8UC1;
            std::string flowFileEnding = ".jpg";

            FlowX.convertTo(NormImageOutputFlowX, flowFormat, m_alpha, m_beta);
            FlowY.convertTo(NormImageOutputFlowY, flowFormat, m_alpha, m_beta);

            // // check for equal frames
            // double minVal; double maxVal; 
            // minMaxLoc(NormImageOutputFlowX, &minVal, &maxVal);
            // std::cout << outputId << ": " << minVal << " | " << maxVal << std::endl;
            // int tolerance = (int) std::floor(m_alpha / 2.0);
            // if (minVal > 127 - tolerance && maxVal < 127 + tolerance) {
            //     continue;
            // }

            double reverseAlpha = -1.0;
            double reverseBeta = 255.0;

            NormImageOutputFlowX.convertTo(NormImageOutputFlowX, flowFormat, reverseAlpha, reverseBeta);
            NormImageOutputFlowY.convertTo(NormImageOutputFlowY, flowFormat, reverseAlpha, reverseBeta);

            // write positive output
            std::stringstream outputX;
            std::stringstream outputY;

            outputX << currentOpticalOutputFolder << "/X" << outputId << flowFileEnding;
            outputY << currentOpticalOutputFolder << "/Y" << outputId << flowFileEnding;

            successX = cv::imwrite(outputX.str(), NormImageOutputFlowX);
            successY = cv::imwrite(outputY.str(), NormImageOutputFlowY);

            if (!successX || !successY)
            {
                std::cerr << "failed saving image " << outputX.str() << " or " << outputY.str() << std::endl;
                break;
            }

            // write negative output
            std::stringstream negOutputX;
            std::stringstream negOutputY;

            negOutputX << currentOpticalOutputFolder << "/-X" << outputId << flowFileEnding;
            negOutputY << currentOpticalOutputFolder << "/-Y" << outputId << flowFileEnding;

            successX = cv::imwrite(negOutputX.str(), NormImageOutputFlowX);
            successY = cv::imwrite(negOutputY.str(), NormImageOutputFlowY);

            if (!successX || !successY)
            {
                std::cerr << "failed saving image " << negOutputX.str() << " or " << negOutputY.str() << std::endl;
                break;
            }

            // // write resized output
            // std::stringstream resizedOutput;
            // resizedOutput << currentResizedOutputFolder << "/" << outputId << ".jpg";
            // success = cv::imwrite(resizedOutput.str(), ResizedFrame);

            // if (!success)
            // {
            //     std::cerr << "failed saving image " << resizedOutput.str() << std::endl;
            //     break;
            // }

            // // debug output
            // cv::imshow("flowX", FlowX);
            // cv::imshow("flowY", FlowY);
            // cv::waitKey(0);

            outputId++;
        }
    }

    std::string BroxOpticalFlow::replaceString(std::string &s, std::string toReplace, std::string replaceWith)
    {
        if (s.find(toReplace) == std::string::npos){
            return s;
        }
        return (s.replace(s.find(toReplace), toReplace.length(), replaceWith));
    }

} // namespace ofextract
