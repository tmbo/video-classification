#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include "main.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    std::vector<std::string> imagePaths = getImageFilePaths();

    return 0;
}


std::vector<std::string> getImageFilePaths() {
//    std::string server_dir = "opt/data_sets/imagedata_2146_classes/";
    std::string local_dir = "../resources/images/";
    std::vector<std::string> imagePaths;

    boost::filesystem::recursive_directory_iterator rdi(local_dir);
    boost::filesystem::recursive_directory_iterator end_rdi;
    for (; rdi != end_rdi; rdi++) {
        imagePaths.push_back((*rdi).path().string());
//        cout << (*rdi).path().string() << endl;
    }

    return imagePaths;
}
