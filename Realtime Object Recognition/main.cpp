#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>
#include <dirent.h>
#include "recog.h"
#include <opencv2/imgproc.hpp>

std::vector<float> processFrame(cv::Mat &img, cv::Mat &binImg){
    // processes each from the video steam and return features
    std::vector<float> featureVector;
    cv::Mat finalImg(img.rows, img.cols, CV_8UC1, cv::Scalar(0));
    cv::Mat finalImg2(img.rows, img.cols, CV_8UC1, cv::Scalar(0));
    cv::Mat visualRegions(img.rows, img.cols, CV_8UC3, cv::Scalar(0, 0, 0));

    cv::Mat thresoldImg(img.rows, img.cols, CV_8UC1, cv::Scalar(0));
    thresoldImage(img, thresoldImg);

    cv::Mat eroded(img.rows, img.cols, CV_8UC1, cv::Scalar(0));
    erode(thresoldImg, eroded);
    dilate(eroded, finalImg);

    std::vector<int> regionIDs = connectedComponentLabel(finalImg, finalImg2, visualRegions);
    binImg = visualRegions;
    if(regionIDs.size() > 0){
        featureVector = calcMoments(finalImg2, regionIDs[0]);
    }
    return featureVector;
}


int main(){

    cv::VideoCapture capdev;
    capdev.open(0);
    // capdev = new cv::VideoCapture(1);
    if( !capdev.isOpened() ) {
        printf("Unable to open video device\n");
        // return(-1);
    }

    cv::namedWindow("Video", 1);
    cv::Mat frame;
    char mode = 'd';
    std::vector<float> features;
    int lcount = 0;
    // std::string dirName = "C:\\Users\\afrid\\Desktop\\2drecog\\imgs\\dataset\\pen1.jpg";
    // cv::Mat img = cv::imread(dirName);
    // cv::Mat binImg;
    // features = processFrame(img, binImg);
    // std::string label = nearestLabel(features);
    // std::cout<< label <<std::endl;
    // cv::putText(binImg, label, cv::Point(10, binImg.rows / 10), 0, 1, CV_RGB(255, 255, 255), 1.5);
    // cv::imshow("Video", binImg);

    // while(1){
    //     if(static_cast<char>(cv::waitKey()) == 'q'){
    //         cv::destroyWindow("Video");
    //         // cv::destroyWindow("dstImg");
    //         // cv::destroyWindow("finalImg");
    //         break;
    //     }
    // }

     cv::Mat saveFrame;

    for(;;) {
        capdev >> frame;
        if( frame.empty() ) {
            printf("frame is empty\n");
            break;
        } 
        if(mode == 'd'){
            cv::imshow("Video", frame);
            saveFrame = frame;
        }else if(mode == 'c'){
            // std::string dirName = "C:\\Users\\afrid\\Desktop\\2drecog\\imgs\\dataset\\case1.jpg";
            // cv::Mat binImg = cv::imread(dirName);
            cv::Mat binImg;
            features = processFrame(frame, binImg);
            if(features.size() > 0){
                std::string label = knn(features, 3);
                cv::putText(binImg, label, cv::Point(10, binImg.rows / 10), 0, 1, CV_RGB(255, 255, 255), 1.5);
            }
            cv::imshow("Video", binImg);
            saveFrame = binImg;
        }else if(mode == 't'){
            cv::Mat binImg;
            features = processFrame(frame, binImg);
            cv::imshow("Video", binImg);
            saveFrame = binImg;
        }else if( mode == 'l'){
            cv::Mat binImg;
            features = processFrame(frame, binImg);
            if(features.size() > 0){
                std::string label = nearestLabel(features);
                cv::putText(binImg, label, cv::Point(10, binImg.rows / 10), 0, 1, CV_RGB(255, 255, 255), 1.5);
            }
            cv::imshow("Video", binImg);
            saveFrame = binImg;
        }else if(mode == 'b'){
            cv::Mat binImg(frame.rows, frame.cols, CV_8UC1, cv::Scalar(0));
            cv::Mat thresoldImg(frame.rows, frame.cols, CV_8UC1, cv::Scalar(0));
            thresoldImage(frame, thresoldImg);
            
            cv::Mat eroded(frame.rows, frame.cols, CV_8UC1, cv::Scalar(0));
            erode(thresoldImg, eroded);
            dilate(eroded, binImg);
            cv::imshow("Video", binImg);
            saveFrame = binImg;
        }else if(mode == 'r'){
            cv::Mat binImg;
            features = processFrame(frame, binImg);
            if(features.size() > 0){
                std::string label = knn(features, 3);
            }
            cv::imshow("Video", binImg);
            saveFrame = binImg;
        }
        char key = cv::waitKey(10);
        if( key == 'q') {
            break;
        }else if(key == 'n'){
            std::string label = "";
            std::cout<<"Enter the name of label : ";
            std::cin >> label;
            std::cout << "label is : " << label << std::endl;
            std::string featureString = label;
            std::cout<<mode<<std::endl;
            for(int f=0; f<features.size(); f++){
                featureString.append(",");
                featureString.append(std::to_string(features[f]));
            }
            writeCSV("objects.csv", featureString);
        }else if(key == 's'){
            cv::imwrite("nearest" + std::to_string(lcount) + ".jpg", saveFrame);
            lcount++;
        }else if(int(key) != -1){
            mode = key;
        }
    }
    return 0;
}