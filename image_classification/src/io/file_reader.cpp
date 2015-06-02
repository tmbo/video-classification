#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include "file_reader.hpp"

using namespace ic;

std::vector<Sequence> FileReader::load(std::string txtFile, int sequenceSize, std::vector<Sequence>& sequences) {
    std::cout << "Reading " << txtFile << " ..." << std::endl;

    std::ifstream input(txtFile);
    std::string line;
    std::vector<std::string> frames;
    int count = 1;

    if (input.is_open()) {
        while (std::getline(input, line)) {
            // read line and get frame path and class
            std::vector<std::string> tokens = splitLine(line);
            std::string file = tokens.at(0);
            int clazz = std::stoi(tokens.at(1));
            // add frame to frames vector
            frames.push_back(file);

            // if sequence is complete, create sequence and
            // add it to the sequence vector
            if (count == sequenceSize) {
                // get clazz name
                ///opt/data_sets/UCF-101/resized_frames/FloorGymnastics/v_FloorGymnastics_g07_c05/7.jpg
                boost::filesystem::path path(file);
                std::string clazzName = boost::filesystem::basename(path.parent_path().parent_path());

                Sequence seq;
                seq.frames = frames;
                seq.clazz = clazz;
                seq.clazzName = clazzName;
                sequences.push_back(seq);

                // clear count and frames vector
                count = 0;
                frames = std::vector<std::string>();
            }

            count++;
        }
        input.close();
    }

    return sequences;
}

std::vector<std::string> FileReader::splitLine(std::string s) {
    std::string delimiter = " ";
    std::vector<std::string> tokens;

    // get frame path
    size_t pos = s.find(delimiter);
    std::string token = s.substr(0, pos);
    tokens.push_back(token);
    // get class value
    token = s.substr(pos + 1, s.size());
    tokens.push_back(token);
    return tokens;
}
