#include<iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[]){

    cv::Mat src;
    char fileName[256] = "C:/Users/afrid/Desktop/CS5330/images/img0.jpg";

    src = cv::imread(fileName);

    std::cout<<src.size()<<" "<<src.channels() <<std::endl;
    cv::namedWindow(fileName, 1);
    cv::imshow(fileName, src);
    
    while(true){
        if(static_cast<char>(cv::waitKey()) == 'q'){
            cv::destroyWindow(fileName);
            break;
        }
    }
    
    return 0;
}