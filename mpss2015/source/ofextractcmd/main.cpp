#include <iostream>
#include <fstream>
#include <string>

#include <mpss2015-version.h>
#include <ofextract/BroxOpticalFlow.h>

int main(int /*argc*/, char* /*argv*/[])
{
    std::cout << "Version: " << MPSS2015_VERSION << std::endl;
    ofextract::BroxOpticalFlow* broxOpticalFlow = new ofextract::BroxOpticalFlow();
    broxOpticalFlow->runAll();

    // // Read file
    // std::cout << "Reading from 'data/example.txt': " << std::endl;
    // std::cout << std::endl;
    // std::ifstream f("data/example.txt");
    // if (f.is_open()) {
    //     std::string line;
    //     while (getline(f, line)) {
    //         std::cout << line << '\n';
    //     }
    //     f.close();
    // } else {
    //     std::cout << "Unable to open file." << std::endl;
    // }

    return 0;
}
