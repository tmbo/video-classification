#include <iostream>
#include <fstream>
#include <string>

#include <mpss2015-version.h>
#include <ofextract/BroxOpticalFlow.h>

int main(int argc, char* argv[])
{
    std::cout << "Version: " << MPSS2015_VERSION << std::endl;

    if (argc == 3)
    {
        ofextract::BroxOpticalFlow* broxOpticalFlow = new ofextract::BroxOpticalFlow(
            argv[1],
            argv[2]
            );

        broxOpticalFlow->runAll();
    } else {
        std::cout << "usage: <source_root_dir> <output_root_dir>" << std::endl;
        // ofextract::BroxOpticalFlow* broxOpticalFlow = new ofextract::BroxOpticalFlow(
        //     "/opt/data_sets/UCF-101/frames/",
        //     "/opt/data_sets/UCF-101/broxoptflow/"
        //     );

        // broxOpticalFlow->runAll();
    }
    return 0;
}
