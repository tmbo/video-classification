#include "histogram.hpp"

#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace ic;


Histogram::Histogram(int histSize) {
    this->m_histSize = histSize;
}
int Histogram::getHistSize() {
    return this->m_histSize;
}

void Histogram::displayHistogram(const cv::Mat &hist) {
    std::cout << "Histogram size = " << hist.total() << ": [" << hist.size[0] << " x " << hist.size[1] << " x " << hist.size[2] << "]" << std::endl;
//    for (MatConstIterator_<float> it = hist.begin<float>(); it != hist.end<float>(); it++) {
//        cout << *it << "\n";
//    }
//    cout << std::flush;
}

void Histogram::plotHistogram(const cv::Mat &hist, int width, int height) {
//    return;
    int histSize = hist.rows;
    int binWidth = cvRound((double) width / histSize);

    cv::Mat histImage(height, width, CV_8UC1, cv::Scalar(0));
    normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    for (int i = 1; i <= histSize; i++) {
        cv::line(histImage, cv::Point(binWidth * (i - 1), height - cvRound(hist.at<float>(i - 1))) ,
                            cv::Point(binWidth * (i),     height - cvRound(hist.at<float>(i))),
                            cv::Scalar(255), 2, 8, 0);
    }

    cv::namedWindow("Result", 1 );
    cv::imshow("Result", histImage);
    cv::waitKey(0);
}

cv::Mat Histogram::buildHistogram(const cv::Mat& image) {
    cv::Mat imageHSV;
    cv::cvtColor(image, imageHSV, CV_BGR2HSV);

    std::vector<cv::Mat> hsv_planes;
    cv::split(imageHSV, hsv_planes);

    int histSize = 256;
    float range[] = { 0, 256 } ;
    const float* histRange = { range };

    bool uniform = true;
    bool accumulate = false;

    cv::Mat h_hist, s_hist, v_hist;

    /// Compute the histograms:
    cv::calcHist(&hsv_planes[0], 1, 0, cv::Mat(), h_hist, 1, &histSize, &histRange, uniform, accumulate );
    cv::calcHist(&hsv_planes[1], 1, 0, cv::Mat(), s_hist, 1, &histSize, &histRange, uniform, accumulate );
    cv::calcHist(&hsv_planes[2], 1, 0, cv::Mat(), v_hist, 1, &histSize, &histRange, uniform, accumulate );

    cv::transpose(h_hist, h_hist);
    cv::transpose(s_hist, s_hist);
    cv::transpose(v_hist, v_hist);

    int numberOfPixels = image.size().width + image.size().height;

    h_hist = h_hist / numberOfPixels;
    s_hist = s_hist / numberOfPixels;
    v_hist = v_hist / numberOfPixels;

    std::vector<cv::Mat> allHist;

    allHist.push_back(h_hist);
    allHist.push_back(s_hist);
    allHist.push_back(v_hist);

    cv::Mat allHistMat;
    cv::hconcat(allHist, allHistMat);

    // Debug Output start

//    cv::Mat h_hist_norm, s_hist_norm, v_hist_norm;

//    // Draw the histograms for H, S, V
//    int hist_w = 1024; int hist_h = 255;
//    int bin_w = cvRound((double) hist_w/histSize);

//    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0,0,0));

//    // Normalize the result to [ 0, histImage.rows ]
//    cv::normalize(h_hist, h_hist_norm, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
//    cv::normalize(s_hist, s_hist_norm, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
//    cv::normalize(v_hist, v_hist_norm, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

//    // Draw for each channel
//    for( int i = 1; i < histSize; i++ )
//    {
//        cv::line(histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(h_hist_norm.at<float>(i-1)) ) ,
//                       cv::Point( bin_w*(i), hist_h - cvRound(h_hist_norm.at<float>(i)) ),
//                       cv::Scalar( 255, 0, 0), 2, 8, 0  );
//        cv::line(histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(s_hist_norm.at<float>(i-1)) ) ,
//                       cv::Point( bin_w*(i), hist_h - cvRound(s_hist_norm.at<float>(i)) ),
//                       cv::Scalar( 0, 255, 0), 2, 8, 0  );
//        cv::line(histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(v_hist_norm.at<float>(i-1)) ) ,
//                       cv::Point( bin_w*(i), hist_h - cvRound(v_hist_norm.at<float>(i)) ),
//                       cv::Scalar( 0, 0, 255), 2, 8, 0  );
//    }

//    // Display
//    cv::namedWindow("original", CV_WINDOW_NORMAL);
//    cv::imshow("original", image);

//    cv::namedWindow("histogram", CV_WINDOW_NORMAL );
//    cv::imshow("histogram", histImage );

//    cv::waitKey(0);

    // Debug Output end

    return allHistMat;
}
