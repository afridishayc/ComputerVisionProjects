
#include<cstdio>
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sys/stat.h>

#include "filters.h"


inline bool fileexists(const std::string& filename) {
        struct stat buffer;
        return (stat (filename.c_str(), &buffer) == 0); 
}


int main(int argc, char *argv[]) {

        cv::VideoCapture *capdev;

        // open the video device
        capdev = new cv::VideoCapture(0);
        if( !capdev->isOpened() ) {
                printf("Unable to open video device\n");
                return(-1);
        }

        // get some properties of the image
        cv::Size refS( (int) capdev->get(cv::CAP_PROP_FRAME_WIDTH ),
                       (int) capdev->get(cv::CAP_PROP_FRAME_HEIGHT));
        printf("Expected size: %d %d\n", refS.width, refS.height);

        cv::namedWindow("Video", 1); // identifies a window
        
        int brightSlider = 0;
        int contrastSlider = 1;
        
        cv::createTrackbar("brightness", "Video", &brightSlider, 100);
        cv::createTrackbar("contrast", "Video", &contrastSlider, 5);

        cv::Mat frame;
        char mode = 'd';

        int lcount=0;

        for(;;) {
                *capdev >> frame; // get a new frame from the camera, treat as a stream
                if( frame.empty() ) {
                        printf("frame is empty\n");
                        break;
                }      

                contractBrightNessControl(frame, contrastSlider, brightSlider);

                // std::cout<<brightSlider<<std::endl;
                cv::Mat SaveFrame; 

                if(mode == 'd'){
                        cv::imshow("Video", frame);
                        SaveFrame = frame;
                }else if(mode == 'g'){
                        cv::Mat gray_frame; 
                        cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
                        cv::imshow("Video", gray_frame);
                        SaveFrame = gray_frame;
                }else if(mode == 'n'){
                        cv::Mat negative_frame;
                        negative_frame =  negativeImg(frame);
                        cv::imshow("Video", negative_frame);
                        SaveFrame = negative_frame;
                }else if(mode == 'b'){
                        // cv::Mat blurred(3, {frame.rows, frame.cols, 3}, cv::Mat::type("CV_32FC1"), cv::Scalar(0));
                        cv::Mat blurred(frame.rows, frame.cols, CV_8UC3, cv::Scalar(255, 255, 255));
                        blur5x5(frame, blurred);
                        cv::imshow("Video", blurred);
                        SaveFrame = blurred;
                }else if(mode == 'x'){
                        cv::Mat sobelX(frame.rows, frame.cols, CV_16SC3, cv::Scalar(255, 255, 255));
                        sobolX3x3(frame, sobelX);
                        sobelX = (sobelX / 2) + 127;
                        sobelX.convertTo(sobelX, CV_8UC3);
                        cv::imshow("Video", sobelX);
                        SaveFrame = sobelX;
                }else if(mode == 'y'){
                        cv::Mat sobelY(frame.rows, frame.cols, CV_16SC3, cv::Scalar(255, 255, 255));
                        sobolY3x3(frame, sobelY);
                        sobelY = (sobelY / 2) + 127;
                        sobelY.convertTo(sobelY, CV_8UC3);
                        cv::imshow("Video", sobelY);
                        SaveFrame = sobelY;
                }else if(mode == 'm'){
                        cv::Mat sobelX(frame.rows, frame.cols, CV_16SC3, cv::Scalar(255, 255, 255));
                        cv::Mat sobelY(frame.rows, frame.cols, CV_16SC3, cv::Scalar(255, 255, 255));
                        cv::Mat sobelMag(frame.rows, frame.cols, CV_16SC3, cv::Scalar(255, 255, 255));
                        sobolX3x3(frame, sobelX);
                        sobolY3x3(frame, sobelY);
                        magnitude(sobelX, sobelY, sobelMag);
                        cv::imshow("Video", sobelMag);
                        SaveFrame = sobelMag;
                }else if(mode == 'i'){
                        cv::Mat quantImg(frame.rows, frame.cols, CV_8UC3, cv::Scalar(255, 255, 255));
                        // blur5x5(frame, quantImg);
                        blurQuantize(frame, quantImg, 15);
                        cv::imshow("Video", quantImg);
                        SaveFrame = quantImg;
                }else if(mode == 'c'){
                        cv::Mat cartoonImg(frame.rows, frame.cols, CV_16SC3, cv::Scalar(255, 255, 255));
                        cartoon(frame, cartoonImg, 12, 12);
                        cv::imshow("Video", cartoonImg);
                        SaveFrame = cartoonImg;
                }else if(mode == 'r'){
                        cv::Mat maxr(frame.rows, frame.cols, CV_8UC3, cv::Scalar(255, 255, 255));
                        maxRGB(frame, maxr);
                        cv::imshow("Video", maxr);
                        SaveFrame = maxr;
                }else if(mode == 'z'){
                        // cv::Mat cf(frame.rows, frame.cols, CV_16SC3, cv::Scalar(255, 255, 255));
                        // cornerFilter(frame, cf);
                        // cf = (cf / 2) + 127;
                        // cf.convertTo(cf, CV_8UC3);
                        // cv::imshow("Video", cf);
                }

                char key = cv::waitKey(10);

                if( key == 'q') {
                        break;
                }else if(key == 's'){
                        cv::imwrite("frame" + std::to_string(lcount) + ".jpg", SaveFrame);
                }else if(int(key) != -1){
                        mode = key;
                }

                lcount++;
        }

        delete capdev;
        return(0);
}
