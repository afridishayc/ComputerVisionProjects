#include <opencv2/opencv.hpp>

// header file - descriptions of functions are provided in the recog.cpp

int thresoldVal(cv::Mat &src);

int thresoldImage(cv::Mat &src, cv::Mat &dst); 

int erode(cv::Mat &src, cv::Mat &dst);

int dilate(cv::Mat &src, cv::Mat &dst);

std::vector<int> connectedComponentLabel(cv::Mat &src, cv::Mat &dst, cv::Mat &vis);

int momentTest(cv::Mat &regionMap, int targetRegionID);

std::vector<float> calcMoments(cv::Mat &regionMap, int targetRegionID);

void writeCSV(char* filename, std::string csvString);

std::string nearestLabel(std::vector<float> targetFeatures);

std::string knn(std::vector<float> targetFeatures, int neighbors);