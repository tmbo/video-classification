#pragma once

#include <string>

#include <ofextract/ofextract_api.h>

namespace ofextract
{
    class OFEXTRACT_API BroxOpticalFlow
    {
    public:
        BroxOpticalFlow(
            std::string sourceFolder = "/opt/data_sets/UCF-101/frames/MoppingFloor/v_MoppingFloor_g01_c01/",
            std::string resizedOutputFolder = "/opt/data_sets/UCF-101/resizedFrames/MoppingFloor/v_MoppingFloor_g01_c01/",
            std::string opticalFlowOutputFolder = "/opt/data_sets/UCF-101/broxoptflow/MoppingFloor/v_MoppingFloor_g01_c01/"
        );
        virtual ~BroxOpticalFlow();

        void runAll();
        void runOnFolder(std::string currentFolder);
    private:
        std::string replaceString(std::string &s, std::string toReplace, std::string replaceWith);

        double m_alpha = 7.0;
        double m_beta = 127.0;

        std::string m_sourceFolder;
        std::string m_resizedOutputFolder;
        std::string m_opticalOutputFolder;
    };

} // namespace ofextract
