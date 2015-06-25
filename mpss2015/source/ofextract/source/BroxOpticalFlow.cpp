#include <ofextract/BroxOpticalFlow.h>

#include <iostream>
#include <sstream>

#include <Poco/RecursiveDirectoryIterator.h>
#include <Poco/File.h>
#include <Poco/Path.h>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/gpu/gpu.hpp"

namespace ofextract
{
    BroxOpticalFlow::BroxOpticalFlow(std::string sourceFolder, std::string opticalFlowOutputFolder):
    m_sourceFolder(sourceFolder)
    , m_opticalOutputFolder(opticalFlowOutputFolder)
    {
    }

    BroxOpticalFlow::~BroxOpticalFlow()
    {
    }

    void BroxOpticalFlow::runAll()
    {
        Poco::DirectoryIterator rdi(m_sourceFolder);
        Poco::DirectoryIterator end_rdi;

        std::string parentFolder = m_sourceFolder;
        bool dirProcessed = false;
        for (; rdi != end_rdi; ++rdi) {
            if ((*rdi).isDirectory())
            {
                runOnFolder(rdi.path().toString());
            }
        }
    }

    void BroxOpticalFlow::runOnFolder(std::string currentFolder)
    {
        std::stringstream videoCapturePath;
        videoCapturePath << currentFolder << "/%3d.jpg";
        cv::VideoCapture videoCapture(videoCapturePath.str());

        std::string currentOpticalOutputFolder = replaceString(currentFolder, m_sourceFolder, m_opticalOutputFolder);
        
        // create neccessary directories
        Poco::File opticalOutputPath(currentOpticalOutputFolder);
        opticalOutputPath.createDirectories();
        std::cout << "Directory Created: " << opticalOutputPath.path() << std::endl;

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
        cv::gpu::BroxOpticalFlow broxOpticalFlow = cv::gpu::BroxOpticalFlow(0.197f, 50.0f, 0.8f, 10, 77, 10);

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
            broxOpticalFlow(PreviousFrameGPU, CurrentFrameGPU, FlowXGPU, FlowYGPU);

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

            if (outputId < 10) {
                outputX << currentOpticalOutputFolder << "/X00" << outputId << flowFileEnding;
                outputY << currentOpticalOutputFolder << "/Y00" << outputId << flowFileEnding;
            } else {
                if (outputId < 100) {
                    outputX << currentOpticalOutputFolder << "/X0" << outputId << flowFileEnding;
                    outputY << currentOpticalOutputFolder << "/Y0" << outputId << flowFileEnding;
                } else {
                    outputX << currentOpticalOutputFolder << "/X" << outputId << flowFileEnding;
                    outputY << currentOpticalOutputFolder << "/Y" << outputId << flowFileEnding;
                }
            }

            successX = cv::imwrite(outputX.str(), NormImageOutputFlowX);
            successY = cv::imwrite(outputY.str(), NormImageOutputFlowY);

            if (!successX || !successY)
            {
                std::cerr << "failed saving image " << outputX.str() << " or " << outputY.str() << std::endl;
                break;
            }

            // // write negative output
            // std::stringstream negOutputX;
            // std::stringstream negOutputY;

            // negOutputX << currentOpticalOutputFolder << "/-X" << outputId << flowFileEnding;
            // negOutputY << currentOpticalOutputFolder << "/-Y" << outputId << flowFileEnding;

            // successX = cv::imwrite(negOutputX.str(), NormImageOutputFlowX);
            // successY = cv::imwrite(negOutputY.str(), NormImageOutputFlowY);

            // if (!successX || !successY)
            // {
            //     std::cerr << "failed saving image " << negOutputX.str() << " or " << negOutputY.str() << std::endl;
            //     break;
            // }

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
