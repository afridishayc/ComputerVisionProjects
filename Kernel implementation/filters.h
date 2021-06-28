#include<opencv2/opencv.hpp>

cv::Mat negativeImg(cv::Mat frame);

int blur5x5(cv::Mat &src, cv::Mat &dst);

int sobolX3x3(cv::Mat &src, cv::Mat &dst);

int sobolY3x3(cv::Mat &src, cv::Mat &dst);

int magnitude( cv::Mat &sx, cv::Mat &sy, cv::Mat &dst );

int blurQuantize( cv::Mat &src, cv::Mat &dst, int levels );

int cartoon( cv::Mat &src, cv::Mat&dst, int levels, int magThreshold );

int cornerFilter(cv::Mat &src, cv::Mat &dst);

int maxRGB(cv::Mat &src, cv::Mat&dst);

int contractBrightNessControl(cv::Mat &src, int alpha, int beta);