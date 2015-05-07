#include <iostream>
#include <vector>
#include <iostream>
#include <regex>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include "file_reader.hpp"
#include "../output/file_writer.hpp"

using namespace ic;
using namespace std;

std::vector<Image> FileReader::loadImages(FileWriter& fileWriter) {
    std::vector<Image> images;

    // read truth data

    load("resources/images/truth/", -1.0, images, fileWriter);

    // read noise data
    load("resources/images/noise/", 1.0, images, fileWriter);

    return images;
}

void FileReader::load(std::string dir, float clazz, std::vector<Image>& images, FileWriter& fileWriter) {
    boost::filesystem::recursive_directory_iterator rdi(dir);
    boost::filesystem::recursive_directory_iterator end_rdi;

    for (; rdi != end_rdi; rdi++) {
        std::string file = (*rdi).path().string();

        if (checkImageSize(file, fileWriter)) {
            Image image(clazz, file);
            images.push_back(image);
        }
    }
}

bool FileReader::checkImageSize(std::string file, FileWriter& fileWriter) {
    cv::Mat image = cv::imread(file, CV_LOAD_IMAGE_COLOR);
    cv::Size s = image.size();

    if (s.height == 0 && s.width == 0) {
//            cout << "Image size 0: " << file << endl;
        fileWriter.writeLine(file);
        return false;
    }
    return true;
}
