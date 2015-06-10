#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <caffe/caffe.hpp>
#include <caffe/proto/caffe.pb.h>
#include <caffe/util/io.hpp>
#include <glog/logging.h>
#include "io/file_reader.hpp"
#include "io/file_writer.hpp"
#include "classification/caffe_classifier.hpp"
#include "classification/evaluation.hpp"
#include "util.hpp"
#include <boost/format.hpp>
#include <boost/bind.hpp>

using namespace ic;

int main(int argc, char** argv) {
    std::cout << "Hello World" << std::endl;
}
