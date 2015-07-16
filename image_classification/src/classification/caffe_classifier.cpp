#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <caffe/caffe.hpp>
#include <caffe/proto/caffe.pb.h>
#include <caffe/util/io.hpp>
#include <caffe/data_layers.hpp>
#include "caffe_classifier.hpp"

using namespace std;
using namespace caffe;
using namespace cv;

namespace ic {

    CaffeClassifier::CaffeClassifier(bool _cpu,
                                     string _modelFile,
                                     string _protoFile,
                                     Size _size,
                                     int _channels,
                                     bool _isDebug = false) {
        caffeNet  = 0;
        isDebug   = _isDebug;
        imageSize = _size;
        channels  = _channels;
        initializeCaffeNet(_cpu, _modelFile, _protoFile);
    }

    CaffeClassifier::~CaffeClassifier() {
        // release everything
        if(caffeNet != 0)
            delete caffeNet;
    }

    /**
     * Initializes the caffe net with a trained model and its corresponding proto file.
     */
    void CaffeClassifier::initializeCaffeNet(bool cpu, string modelFile, string protoFile) {
        if (!cpu) {
            uint device_id = 0;
            if(isDebug)
                cout << "Info: Caffe with GPU mode, device id:" << device_id << endl;
            Caffe::SetDevice(device_id);
            Caffe::set_mode(Caffe::GPU);
        } else {
            if(isDebug)
                cout << "Info: Caffe with CPU mode."<< endl;
            Caffe::set_mode(Caffe::CPU);
        }

        caffeNet = new Net<float>(protoFile, caffe::TEST);
        caffeNet->CopyTrainedLayersFrom(modelFile);
    }

    void CaffeClassifier::predictHeatMap(cv::Mat& inputImage, int label, string predictionLayer, string dataLayer, cv::Mat& heatMap) {
        const int IMAGE_SIZE = 227;
        const int BATCH_SIZE = 64;
        heatMap = cv::Mat(IMAGE_SIZE, IMAGE_SIZE, CV_32FC1, Scalar(26.932154));

        const int STEP_SIZE = 9;
        const int START_OFFSET = STEP_SIZE / 2;
        const int FILLER_SIZE = 50;

        cv::Scalar mean =  cv::mean(inputImage);

        std::vector<Point> middlePoints;
        for (int i = START_OFFSET; i < IMAGE_SIZE; i += STEP_SIZE) {
            for (int j = START_OFFSET; j < IMAGE_SIZE; j += STEP_SIZE) {
                middlePoints.push_back(Point(i, j));
            }
        }

        for (int i = 0; i < middlePoints.size(); i += BATCH_SIZE) {
            std::cout << (i * 100) / middlePoints.size() << "% " << std::flush;
            vector<Datum> vecDatum;

            for (int j = 0; j < BATCH_SIZE; ++j) {
                // do not go over the last middle point
                int index = min(static_cast<int>(middlePoints.size() - 1), i + j);
                Point p = middlePoints[index];
                cv::Mat image = inputImage.clone();

//                cv::Rect rect(Point(max(0, p.x - FILLER_SIZE), max(0, p.y - FILLER_SIZE)), Point(min(IMAGE_SIZE - 1, p.x + FILLER_SIZE), min(IMAGE_SIZE - 1, p.y + FILLER_SIZE)));
//                cv::Mat subMat = image(rect);
//                cv::Scalar mean =  cv::mean(subMat);

                circle(image,
                       p,
                       FILLER_SIZE,
                       mean,
                       CV_FILLED);
//                rectangle(image,
//                          Point(max(0, p.x - FILLER_SIZE), max(0, p.y - FILLER_SIZE)),
//                          Point(min(IMAGE_SIZE - 1, p.x + FILLER_SIZE), min(IMAGE_SIZE - 1, p.y + FILLER_SIZE)),
//                          Scalar(0, 0, 0),
//                          CV_FILLED);
                std::ostringstream o;
                o << "/home/knub/Repositories/video-classification/nets/activity_recognition/caffenet/";
                o << index;
                o << "_heat.png";
                cv::imwrite(o.str(), image);

                // check channels
                if (channels != image.channels()) {
                    cerr << "Error: the channel number of input image is invalid for CNN classifier!" << endl;
                    exit(1);
                }

                // mat to datum
                Datum datum;
                CVMatToDatum(image, &datum);
                vecDatum.push_back(datum);
                image.release();
            }

            // get the data layer
            const caffe::shared_ptr<MemoryDataLayer<float>> memDataLayer = boost::static_pointer_cast<MemoryDataLayer<float>>(caffeNet->layer_by_name(dataLayer));

            // push new image data
            memDataLayer->AddDatumVector(vecDatum);

            // do forward pass
            vector<Blob<float>*> inputVec;
            caffeNet->Forward(inputVec);

            // get results
            const caffe::shared_ptr<Blob<float> > featureBlob = caffeNet->blob_by_name(predictionLayer);
            int dimFeatures = featureBlob->count() /  BATCH_SIZE; // 101
            assert(dimFeatures == 101);

            // get output from each input image
            for (int j = 0; j < BATCH_SIZE; ++j) {
                int index = min(static_cast<int>(middlePoints.size() - 1), i + j);
                Point p = middlePoints[index];

//                std::cout << "Channels: " << featureBlob->channels() << ", Count: " << featureBlob->count() << ", Width: " << featureBlob->width() << ", Height: " << featureBlob->height() << std::endl;

//                featureBlob = 64 x 101 matrix

                float* fs = featureBlob->mutable_cpu_data() + featureBlob->offset(j);
                vector<float> featureVector(fs, fs + dimFeatures);
//                std::vector<float>::iterator result = std::max_element(featureVector.begin(), featureVector.end());
//                int predicted = result - featureVector.begin();
//                std::cout << "Predicted: " << predicted << ", Actual: " << label << std::endl;
//                assert(predicted == label);

                float confidence = featureVector[label];


                rectangle(heatMap,
                    Point(p.x - START_OFFSET, p.y - START_OFFSET),
                    Point(p.x + START_OFFSET, p.y + START_OFFSET),
                    Scalar(confidence),
                    CV_FILLED);
            }
        }
    }

    /**
     * Get the prediction response for the given image.
     * @param originImage image, which should be predicted
     * @param resultLayer the name of the result layer
     * @param dataLayer   the name of the data layer
     * @param predictions the predictions
     */
    void CaffeClassifier::predict(std::vector<cv::Mat>& originImages, std::vector<int> labels, string resultLayer,
                                  string dataLayer, vector<float> & predictions) {
        vector<Datum> vecDatum;

        for (int i = 0; i < originImages.size(); i++) {
            cv::Mat originImage = originImages[i];

            // resize image
            Mat image;
            if (originImage.cols != imageSize.width || originImage.rows != imageSize.height) {
                resize(originImage, image, imageSize);
//                std::cout << "Image does not have correct size. Exiting." << std::endl;
//                exit(99);
            } else
                image = originImage;

            // check channels
            if (channels != image.channels()) {
                cerr << "Error: the channel number of input image is invalid for CNN classifier!" << endl;
                exit(1);
            }

            // mat to datum
            Datum datum;
            CVMatToDatum(image, &datum);
            datum.set_label(labels[i]);
            vecDatum.push_back(datum);
            image.release();
        }

        // get the data layer
        const caffe::shared_ptr<MemoryDataLayer<float>> memDataLayer = boost::static_pointer_cast<MemoryDataLayer<float>> (caffeNet->layer_by_name(dataLayer));

        // push new image data
        memDataLayer->AddDatumVector(vecDatum);
        //memDataLayer->ExactNumBottomBlobs();

        // do forward pass
        vector<Blob<float>*> inputVec;
        caffeNet->Forward(inputVec);

        // get results
        const caffe::shared_ptr<Blob<float> > featureBlob = caffeNet->blob_by_name(resultLayer);
        int batchSize = featureBlob->num();
        int dimFeatures = featureBlob->count() / batchSize;
//        std::cout << "Batch size is " << batchSize << "/ dim features is " << dimFeatures << std::endl;

        // get output from each channel
        for (int n = 0; n < batchSize; ++n) {
            float* fs = featureBlob->mutable_cpu_data() + featureBlob->offset(n);
            if (sizeof(fs) > 0) {
                vector<float> feature_vector(fs, fs + dimFeatures);
                predictions.insert(predictions.end(), feature_vector.begin(), feature_vector.end());
            }
        }

        // release data
        // for (Datum d : vecDatum) {
        //     d.release_data();
        // }
    }
}

