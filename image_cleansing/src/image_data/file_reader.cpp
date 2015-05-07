#include <iostream>
#include <vector>
#include <iostream>
#include <regex>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include "file_reader.hpp"

using namespace ic;

std::vector<Image> FileReader::loadImages() {
    std::vector<Image> images;

    // read truth data
    load("../resources/images/truth/", 0, images);

    // read noise data
    load("../resources/images/noise/", 1, images);

    return images;
}

void FileReader::load(std::string dir, bool clazz, std::vector<Image>& images) {
    boost::filesystem::recursive_directory_iterator rdi(dir);
    boost::filesystem::recursive_directory_iterator end_rdi;

    for (; rdi != end_rdi; rdi++) {
        std::string file = (*rdi).path().string();

        if (checkImageSize(file)) {
            Image image(clazz, file);
            images.push_back(image);
        }
    }
}

bool FileReader::checkImageSize(std::string file) {
    cv::Mat image = cv::imread(file, CV_LOAD_IMAGE_COLOR);
    cv::Size s = image.size();

    if (s.height == 0 && s.width == 0) {
//            cout << "Image size 0: " << file << endl;
        return false;
    }
    return true;
}
