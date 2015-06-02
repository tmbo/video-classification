#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <caffe/caffe.hpp>
#include <caffe/proto/caffe.pb.h>
#include <caffe/util/io.hpp>
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
        //release everything
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

    /**
     * Get the prediction response for the given image.
     * @param originImage image, which should be predicted
     * @param resultLayer the name of the result layer
     * @param dataLayer   the name of the data layer
     * @param predictions the predictions
     */
    void CaffeClassifier::predict(std::vector<cv::Mat> originImages, string resultLayer, string dataLayer, vector<float> & predictions) {
        vector<Datum> vecDatum;

        for (cv::Mat originImage : originImages) {
            // check if image contains data
            if(!originImage.data) {
                cerr << "Warning: input image for caffe calssifier is empty." << endl;
                return;
            }

            // resize image
            std::cout << "Resize image..." << std::endl;
            Mat image;
            if(originImage.cols != imageSize.width || originImage.rows != imageSize.height)
                resize(originImage, image, imageSize);
            else
                image = originImage;

            // check channels
            if(channels != image.channels()){
                cerr << "Error: the channel number of input image is invalid for CNN classifier!" << endl;
                exit(1);
            }

            // mat to datum
            std::cout << "Converting Mat to Datum..." << std::endl;
            Datum datum;
            CVMatToDatum(image, &datum);
            vecDatum.push_back(datum);
        }

        // get the data layer
        std::cout << "Building data layer..." << std::endl;
        const caffe::shared_ptr<MemoryDataLayer<float> > memDataLayer =
                boost::static_pointer_cast<MemoryDataLayer<float>> (caffeNet->layer_by_name(dataLayer));

        // push new image data
        memDataLayer->AddDatumVector(vecDatum);

        // do forward pass
        std::cout << "Doing forward pass..." << std::endl;
        vector<Blob<float>*> inputVec;
        caffeNet->Forward(inputVec);

        // get results
        std::cout << "Get results..." << std::endl;
        const caffe::shared_ptr<Blob<float> > featureBlob = caffeNet->blob_by_name(resultLayer);
        int batchSize = featureBlob->num();
        int dimFeatures = featureBlob->count() / batchSize;

        // get output from each channel
        for (int n = 0; n < batchSize; ++n) {
            float* fs = featureBlob->mutable_cpu_data() + featureBlob->offset(n);
            if (sizeof(fs) > 0) {
                vector<float> feature_vector(fs, fs + dimFeatures);
                 predictions.insert(predictions.begin(), feature_vector.begin(), feature_vector.end());
            }
        }

        // release data
        for (Datum d : vecDatum)
            d.release_data();
    }

}

