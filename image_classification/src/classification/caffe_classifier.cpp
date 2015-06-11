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
        heatMap = cv::Mat(227, 227, CV_32FC1, Scalar(0));

        int STEP_SIZE = 13;
        int START_OFFSET = STEP_SIZE / 2;
        int FILLER_SIZE = 100;

        std::vector<Point> middlePoints;
        for (int i = START_OFFSET; i < 227; i += STEP_SIZE) {
            for (int j = START_OFFSET; j < 227; j += STEP_SIZE) {
                middlePoints.push_back(Point(i, j));
            }
        }
//        assert(middlePoints.size() == 76 * 76);

        for (int i = 0; i < middlePoints.size(); i += 64) {
            std::cout << (i * 100) / middlePoints.size() << "% " << std::flush;
            vector<Datum> vecDatum;

            for (int j = 0; j < 64; j++) {
                int index = min(static_cast<int>(middlePoints.size() - 1), i + j);
                Point p = middlePoints[index];
                cv::Mat image = inputImage.clone();
                rectangle(image,
                          Point(max(0, p.x - FILLER_SIZE), max(0, p.y - FILLER_SIZE)),
                          Point(min(226, p.x + FILLER_SIZE), min(226, p.y + FILLER_SIZE)),
                          Scalar(127, 127, 127),
                          CV_FILLED);
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
                datum.set_label(label);
                vecDatum.push_back(datum);
                image.release();
            }

            // get the data layer
            const caffe::shared_ptr<MemoryDataLayer<float>> memDataLayer = boost::static_pointer_cast<MemoryDataLayer<float>> (caffeNet->layer_by_name(dataLayer));

            // push new image data
            memDataLayer->AddDatumVector(vecDatum);

            // do forward pass
            vector<Blob<float>*> inputVec;
            caffeNet->Forward(inputVec);

            // get results
            const caffe::shared_ptr<Blob<float> > featureBlob = caffeNet->blob_by_name(predictionLayer);
            int batchSize = featureBlob->num(); // 64
            int dimFeatures = featureBlob->count() / batchSize; // 101
//        std::cout << "Batch size is " << batchSize << "/ dim features is " << dimFeatures << std::endl;

            // get output from each channel
            for (int j = 0; j < batchSize; ++j) {
                int index = min(static_cast<int>(middlePoints.size() - 1), i + j);
                Point p = middlePoints[index];

                float* fs = featureBlob->mutable_cpu_data() + featureBlob->offset(j);
                vector<float> featureVector(fs, fs + dimFeatures);
                std::vector<float>::iterator result = std::max_element(featureVector.begin(), featureVector.end());
//                assert(result - featureVector.begin() - 1 == label);
                float confidence = featureVector[label + 1];


                rectangle(heatMap,
                    Point(p.x - STEP_SIZE, p.y - STEP_SIZE),
                    Point(p.x + STEP_SIZE, p.y + STEP_SIZE),
                    Scalar(confidence),
                    CV_FILLED);
//                std::cout << confidence << std::endl;
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

