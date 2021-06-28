#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <numeric>

// pass filename and csv string to write into the csv file
void writeCSV(char* filename, std::string csvString){
    std::ofstream myFile(filename, std::ios_base::app);
    myFile << csvString <<"\n";
    myFile.close();
}

// writes cv::Mat matrix as a csv
void writeCSV(char *filename, cv::Mat m){
    std::ofstream myFile(filename, std::ios_base::app);
    myFile << cv::format(m, cv::Formatter::FMT_CSV) <<"\n";
    myFile.close();
}

// a function that returns the bin number for multihistogram
std::pair<int, int> targetBins(float rpix, float gpix, int bins){
    int binSize =  100 / bins;
    int rbinNum = (rpix * 100/binSize);
    int gbinNum = (gpix * 100/binSize);
    if(rbinNum >= bins){
        rbinNum = bins - 1;
    }
    if(gbinNum >= bins){
        gbinNum = bins - 1;
    }
    return std::make_pair(rbinNum, gbinNum);
}

// a function that returns the bin number for texture features 
int binIdx(int pixVal){
    int binSize = 255/8;
    int binNum = pixVal / binSize;
    if(binNum >= 8){
        binNum = 7;
    }
    return binNum;
}   

// a function that convolves the image using input kernels
int textureFilter(cv::Mat &src, cv::Mat &dst, std::vector<int> filter_1, std::vector<int> filter_2, int norm1, int norm2){    
    cv::Mat gray_frame;
    cv::cvtColor(src, gray_frame, cv::COLOR_BGR2GRAY);
    cv::Mat midx(src.rows, src.cols, CV_8UC1, cv::Scalar(255));
    for(int i=2; i<gray_frame.rows-2; i++){
        for(int j=2; j<gray_frame.cols-2; j++){
            int jindices[5] = {j-2, j-1, j, j+1, j+2};
            midx.at<uchar>(i, j) = ((gray_frame.at<uchar>(i, jindices[0]) * filter_1[0]) + (gray_frame.at<uchar>(i, jindices[1]) * filter_1[1]) + (gray_frame.at<uchar>(i, jindices[2]) * filter_1[2]) + (gray_frame.at<uchar>(i, jindices[3]) * filter_1[3]) + (gray_frame.at<uchar>(i, jindices[4]) * filter_1[4])) / norm1;
        }
    }

    for(int i=2; i<gray_frame.rows-2; i++){
        for(int j=2; j<gray_frame.cols-2; j++){
            int iindices[5] = {i-2, i-1, i, i+1, i+2};
            dst.at<uchar>(i, j) = ((midx.at<uchar>(iindices[0], j) * filter_2[0]) + (midx.at<uchar>(iindices[1], j) * filter_2[1]) + (midx.at<uchar>(iindices[2], j) * filter_2[2]) + (midx.at<uchar>(iindices[3], j) * filter_2[3]) + (midx.at<uchar>(iindices[4], j) * filter_2[4])) / norm2;
        }
    }

    return 0;
};

// a function that calculates the magnitude of reflected filter outputs 
int textureMagnitude(cv::Mat &imgA, cv::Mat &imgB, cv::Mat &mag){
    for(int i=0; i<imgA.rows; i++){
        for(int j=0; j<imgA.cols; j++){
            mag.at<uchar>(i, j) = sqrt((imgA.at<uchar>(i, j) * imgA.at<uchar>(i, j)) + (imgB.at<uchar>(i, j) * imgB.at<uchar>(i, j)));
        }
    }
    return 0;
}

// a function that normalizes the pixels of an image using L5
int textureNormalize(cv::Mat &magImg, cv::Mat &gVal, cv::Mat &dst){
    for(int i=0; i<magImg.rows; i++){
        for(int j=0; j<magImg.cols; j++){
            int denom = gVal.at<uchar>(i, j);
            if(denom == 0){
                denom = 1;
            }
            dst.at<uchar>(i, j) = magImg.at<uchar>(i, j) / denom;
        }
    }
    return 0;
}

// a function that calculates features of all images inside a folder
void featureCalculation(int featureMode, char *buffer){
    char dirname[256];
    // char buffer[256];
    FILE *fp;
    DIR *dirp;
    struct dirent *dp;
    int i;

    strcpy_s(dirname, buffer);
    dirp = opendir(dirname);
    if( dirp == NULL) {
        printf("Cannot open directory %s\n", dirname);
        exit(-1);
    }

    if(featureMode == 1){
        int pixelX = 252;
        int pixelY = 316;
        std::vector<std::pair<int, int>> targetPixels;
        for(int i=0; i<9; i++){
            for(int j=0; j<9; j++){
                std::pair<int, int> pixel;
                pixel.first = pixelX + i;
                pixel.second = pixelY + j;
                targetPixels.push_back(pixel);
            }
        }

        // std::cout<<""<<std::endl; 

        while( (dp = readdir(dirp)) != NULL ) {
            // check if the file is an image
            if( strstr(dp->d_name, ".jpg") || strstr(dp->d_name, ".png") || strstr(dp->d_name, ".ppm") || strstr(dp->d_name, ".tif") ) {
                strcpy(buffer, dirname);
                strcat(buffer, "\\");
                strcat(buffer, dp->d_name);
                cv::Mat imgSrc = cv::imread(buffer);
                std::string featureString = ",";
                for(auto px: targetPixels){
                    featureString.append(std::to_string((short)imgSrc.at<cv::Vec3b>(px.first, px.second)[0]));
                    featureString.append(",");
                    featureString.append(std::to_string((short)imgSrc.at<cv::Vec3b>(px.first, px.second)[1]));
                    featureString.append(",");
                    featureString.append(std::to_string((short)imgSrc.at<cv::Vec3b>(px.first, px.second)[2]));
                    featureString.append(",");
                }
                featureString.pop_back();
                featureString.insert(0, buffer);
                writeCSV("9by9features.csv", featureString);
            }
        }
    }else if(featureMode == 2){
        std::cout<<featureMode<<std::endl;        
        while( (dp = readdir(dirp)) != NULL ) {
            // check if the file is an image
            if( strstr(dp->d_name, ".jpg") || strstr(dp->d_name, ".png") || strstr(dp->d_name, ".ppm") || strstr(dp->d_name, ".tif") ) {
                strcpy(buffer, dirname);
                strcat(buffer, "\\");
                strcat(buffer, dp->d_name);
                cv::Mat imgSrc = cv::imread(buffer);
                // cv::Mat rgHistogram(cv::Size(16, 16), CV_16UC1, cv::Scalar(0));
                std::vector<int> rgHistogram(256, 0);
                std::string featureString = ",";
                
                for(int l=0; l<imgSrc.rows; l++){
                    for(int m=0; m<imgSrc.cols; m++){
                        int r = (int)imgSrc.at<cv::Vec3b>(l, m)[2];
                        int g = (int)imgSrc.at<cv::Vec3b>(l, m)[1];
                        int b = (int)imgSrc.at<cv::Vec3b>(l, m)[0];
                        if(r <= 0){
                            r = 1;
                        }
                        if(g <= 0){
                            g = 1;
                        }
                        if(b <= 0){
                            b = 1;
                        }
                        int rgb = r + b + g;
                        float rx = (float)r / rgb;
                        float gx = (float)g / rgb;
                        std::pair<int, int> binIndices = targetBins(rx, gx, 16);
                        rgHistogram[binIndices.first + (16 * binIndices.second)] += 1; 
                    }
                }

                for(int idx = 0; idx < rgHistogram.size(); idx++) {
                    featureString.append(std::to_string( rgHistogram[idx] ));
                    featureString.append(",");       
                }

                featureString.pop_back();
                featureString.insert(0, buffer);
                writeCSV("rgfeatures.csv", featureString);
            }
        }
    }else if(featureMode == 3){
        int pixelX = 252;
        int pixelY = 316;
        std::vector<std::pair<int, int>> targetPixels;
        for(int i=0; i<9; i++){
            for(int j=0; j<9; j++){
                std::pair<int, int> pixel;
                pixel.first = pixelX + i;
                pixel.second = pixelY + j;
                targetPixels.push_back(pixel);
            }
        }

        // std::cout<<""<<std::endl; 

        while( (dp = readdir(dirp)) != NULL ) {
            // check if the file is an image
            if( strstr(dp->d_name, ".jpg") || strstr(dp->d_name, ".png") || strstr(dp->d_name, ".ppm") || strstr(dp->d_name, ".tif") ) {
                strcpy(buffer, dirname);
                strcat(buffer, "\\");
                strcat(buffer, dp->d_name);
                cv::Mat imgSrc = cv::imread(buffer);
                std::string featureString = ",";

                std::vector<int> srgHistogram(256, 0);
                for(auto px: targetPixels){
                    int r = (int)imgSrc.at<cv::Vec3b>(px.first, px.second)[2];
                    int g = (int)imgSrc.at<cv::Vec3b>(px.first, px.second)[1];
                    int b = (int)imgSrc.at<cv::Vec3b>(px.first, px.second)[0];
                    if(r <= 0){
                        r = 1;
                    }
                    if(g <= 0){
                        g = 1;
                    }
                    if(b <= 0){
                        b = 1;
                    }
                    int rgb = r + b + g;
                    float rx = (float)r / rgb;
                    float gx = (float)g / rgb;
                    std::pair<int, int> binIndices = targetBins(rx, gx, 16);
                    srgHistogram[binIndices.first + (16 * binIndices.second)] += 1;
                }

                std::vector<int> rgHistogram(256, 0);
                for(int l=0; l<imgSrc.rows; l++){
                    for(int m=0; m<imgSrc.cols; m++){
                        int r = (int)imgSrc.at<cv::Vec3b>(l, m)[2];
                        int g = (int)imgSrc.at<cv::Vec3b>(l, m)[1];
                        int b = (int)imgSrc.at<cv::Vec3b>(l, m)[0];
                        if(r <= 0){
                            r = 1;
                        }
                        if(g <= 0){
                            g = 1;
                        }
                        if(b <= 0){
                            b = 1;
                        }
                        int rgb = r + b + g;
                        float rx = (float)r / rgb;
                        float gx = (float)g / rgb;
                        std::pair<int, int> binIndices = targetBins(rx, gx, 16);
                        rgHistogram[binIndices.first + (16 * binIndices.second)] += 1; 
                    }
                }

                for(int idx = 0; idx < srgHistogram.size(); idx++) {
                    featureString.append(std::to_string( srgHistogram[idx] ));
                    featureString.append(",");       
                }

                for(int idx = 0; idx < rgHistogram.size(); idx++) {
                    featureString.append(std::to_string( rgHistogram[idx] ));
                    featureString.append(",");       
                }

                featureString.pop_back();
                featureString.insert(0, buffer);
                writeCSV("multifeatures.csv", featureString);
            }
        }
    }else if(featureMode == 4){
        while( (dp = readdir(dirp)) != NULL ) {
            // check if the file is an image
            if( strstr(dp->d_name, ".jpg") || strstr(dp->d_name, ".png") || strstr(dp->d_name, ".ppm") || strstr(dp->d_name, ".tif") ) {
                strcpy(buffer, dirname);
                strcat(buffer, "\\");
                strcat(buffer, dp->d_name);
                std::string featureString = ",";
                // cv::Mat imgSrc = cv::imread(buffer);
                std::vector< std::vector<int>> gKernels = {
                                                    {1, 4, 6, 4, 1},
                                                    {1, 2, 0, -2, -1},
                                                    {-1, 0, 2, 0, -1},
                                                    {1, -2, 0, 2, -1},
                                                    {1, -4, 6, -4, 1},
                                                };
                std::vector<int> gNorms = {16, 3, 2, 3, 8};
                cv::Mat imgSrc = cv::imread(buffer);

                // L5 calculation
                std::vector<int> textureVector(112, 0);
                cv::Mat l5img(imgSrc.rows, imgSrc.cols, CV_8UC1, cv::Scalar(255));
                cv::Mat l5mag(imgSrc.rows, imgSrc.cols, CV_8UC1, cv::Scalar(255));
                textureFilter(imgSrc, l5img, gKernels[0], gKernels[0], gNorms[0], gNorms[0]);
                textureMagnitude(l5img, l5img, l5mag);

                int filterIdx = 0;
                for(int k1=0; k1<gKernels.size(); k1++){
                    for(int k2=k1; k2<gKernels.size(); k2++){
                        cv::Mat dst(imgSrc.rows, imgSrc.cols, CV_8UC1, cv::Scalar(255));
                        if(k1 == 0 && k2 == 0){
                            continue;
                        }else if(k1 == k2){
                            cv::Mat mag(imgSrc.rows, imgSrc.cols, CV_8UC1, cv::Scalar(255));
                            textureFilter(imgSrc, mag, gKernels[k1], gKernels[k2], gNorms[k1], gNorms[k2]);
                            textureNormalize(mag, l5mag, dst);
                        }else{
                            cv::Mat tempImgA(imgSrc.rows, imgSrc.cols, CV_8UC1, cv::Scalar(255));
                            cv::Mat tempImgB(imgSrc.rows, imgSrc.cols, CV_8UC1, cv::Scalar(255));
                            cv::Mat mag(imgSrc.rows, imgSrc.cols, CV_8UC1, cv::Scalar(255));
                            textureFilter(imgSrc, tempImgA, gKernels[k1], gKernels[k2], gNorms[k1], gNorms[k2]);
                            textureFilter(imgSrc, tempImgB, gKernels[k2], gKernels[k1], gNorms[k2], gNorms[k1]);
                            textureMagnitude(tempImgA, tempImgB, mag);
                            textureNormalize(mag, l5mag, dst);
                        }

                        for(int l=0; l<dst.rows; l++){
                            for(int m=0; m<dst.cols; m++){
                                int binIdxVal = binIdx( (short)dst.at<uchar>(l, m) );
                                // std::cout<<(8 * filterIdx) +  binIdxVal<<std::endl;
                                textureVector[(8 * filterIdx) + binIdxVal]  += 1;
                            }
                        }
                        // texture histogram
                        filterIdx += 1;
                    }
                }

                // textureVector
                std::vector<int> rgHistogram(256, 0);
                for(int l=0; l<imgSrc.rows; l++){
                    for(int m=0; m<imgSrc.cols; m++){
                        int r = (int)imgSrc.at<cv::Vec3b>(l, m)[2];
                        int g = (int)imgSrc.at<cv::Vec3b>(l, m)[1];
                        int b = (int)imgSrc.at<cv::Vec3b>(l, m)[0];
                        if(r <= 0){
                            r = 1;
                        }
                        if(g <= 0){
                            g = 1;
                        }
                        if(b <= 0){
                            b = 1;
                        }
                        int rgb = r + b + g;
                        float rx = (float)r / rgb;
                        float gx = (float)g / rgb;
                        std::pair<int, int> binIndices = targetBins(rx, gx, 16);
                        rgHistogram[binIndices.first + (16 * binIndices.second)] += 1; 
                    }
                }

                for(int idx = 0; idx < textureVector.size(); idx++) {
                    featureString.append(std::to_string( textureVector[idx] ));
                    featureString.append(",");       
                }

                for(int idx = 0; idx < rgHistogram.size(); idx++) {
                    featureString.append(std::to_string( rgHistogram[idx] ));
                    featureString.append(",");       
                }

                featureString.pop_back();
                featureString.insert(0, buffer);
                writeCSV("textureFeatures.csv", featureString);

            }
        }
    }else if(featureMode == 5){
        int pixelX = 252;
        int pixelY = 316;
        std::vector<std::pair<int, int>> targetPixels;
        for(int i=0; i<9; i++){
            for(int j=0; j<9; j++){
                std::pair<int, int> pixel;
                pixel.first = pixelX + i;
                pixel.second = pixelY + j;
                targetPixels.push_back(pixel);
            }
        }

        while( (dp = readdir(dirp)) != NULL ) {
            // check if the file is an image
            if( strstr(dp->d_name, ".jpg") || strstr(dp->d_name, ".png") || strstr(dp->d_name, ".ppm") || strstr(dp->d_name, ".tif") ) {
                strcpy(buffer, dirname);
                strcat(buffer, "\\");
                strcat(buffer, dp->d_name);
                cv::Mat imgSrc = cv::imread(buffer);
                cv::Mat yuvImage;
                cv::cvtColor(imgSrc, yuvImage, cv::COLOR_BGR2GRAY);
                // cv::Mat rgHistogram(cv::Size(16, 16), CV_16UC1, cv::Scalar(0));
                std::string featureString = ",";

                for(auto px: targetPixels){
                    featureString.append(std::to_string((short)imgSrc.at<cv::Vec3b>(px.first, px.second)[0]));
                    featureString.append(",");
                    featureString.append(std::to_string((short)imgSrc.at<cv::Vec3b>(px.first, px.second)[1]));
                    featureString.append(",");
                    featureString.append(std::to_string((short)imgSrc.at<cv::Vec3b>(px.first, px.second)[2]));
                    featureString.append(",");
                }

                std::vector<int> rgHistogram(256, 0);
                for(int l=0; l<imgSrc.rows; l++){
                    for(int m=0; m<imgSrc.cols; m++){
                        int r = (int)imgSrc.at<cv::Vec3b>(l, m)[2];
                        int g = (int)imgSrc.at<cv::Vec3b>(l, m)[1];
                        int b = (int)imgSrc.at<cv::Vec3b>(l, m)[0];
                        if(r <= 0){
                            r = 1;
                        }
                        if(g <= 0){
                            g = 1;
                        }
                        if(b <= 0){
                            b = 1;
                        }
                        int rgb = r + b + g;
                        float rx = (float)r / rgb;
                        float gx = (float)g / rgb;
                        std::pair<int, int> binIndices = targetBins(rx, gx, 16);
                        rgHistogram[binIndices.first + (16 * binIndices.second)] += 1; 
                    }
                }

                for(int idx = 0; idx < rgHistogram.size(); idx++) {
                    featureString.append(std::to_string( rgHistogram[idx] ));
                    featureString.append(",");       
                }

                featureString.pop_back();
                featureString.insert(0, buffer);
                writeCSV("patchHistogram.csv", featureString);
            }
        }
    }
}

// a function that calculates the sum of squared differences between to vectors
float sumOfSquaredDifferences(std::vector<int> a, std::vector<int> b){
    float distance = 0.0;
    for(int i=0; i< a.size(); i++){
        distance += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return distance;
}

// a function that calculates the histogramIntersection Metric
float histogramIntersection(std::vector<int> a, std::vector<int> b){
    float distance = 0.0;
    int sum_x = 0;
    int sum_y = 0;
    for(int i=0; i<a.size(); i++){
        distance += std::min(a[i], b[i]);
        sum_x += a[i];
    }
    return 1 - (distance / sum_x);
}

// a function that retrives image features from the path given
std::vector<int> imageFeatures(std::string imagePath, std::string filename){

    // 9X9 square in the middle

    // check if the file exists
    std::ifstream featureFile(filename);
    if(!featureFile){
        std::cout<<"file not found"<<std::endl;
    }
    // load a line from CSV file
    std::string line;
    std::vector<int> fv;
    bool pathMatched = false; 
    while(std::getline(featureFile, line, '\n')){
        // vector of features
        std::string x;
        std::stringstream a(line);
        int counter = 0;
        while (std::getline(a, x, ',')) {
            if(counter == 0){
                // std::cout<<x<<" "<<imagePath<<std::endl;
                if(imagePath == x){
                    // std::cout<<"Path matched"<<std::endl;
                    pathMatched = true;
                }
            }else{
                if(pathMatched){
                    fv.push_back(stoi(x));
                }
            }
            counter += 1;
        }
        if(pathMatched){
            break;
        }
    }
    featureFile.close();
    return fv;
}

// sort a pair based on the value in second  
bool sortPair(const std::pair<std::string, float> &v1, const std::pair<std::string, float> &v2){
    return (v1.second < v2.second);
}

// a function to slice a vector 
std::vector<int> slice(std::vector<int> &v, int a, int b){
    auto start = v.begin() + a; 
    auto end = v.begin() + b + 1;
    std::vector<int> result(b - a + 1);
    copy(start, end, result.begin());
    return result;
}

// a function to fetch images based on the center patch of 9X9 pixels IR
std::vector<std::pair<std::string, int>> baselineMatching(std::string targetImageName){
    // Matches the image passed through URI with the images in database based on 9*9 features
    std::vector<int> targetImageFeatures = imageFeatures(targetImageName, "9by9features.csv");

    // calculate SSD for all images in the search space
    
    // load feature file 
    std::ifstream featureFile("9by9features.csv");
    if(!featureFile){
        std::cout<<"file not found"<<std::endl;
    }

    std::vector<std::pair<std::string, int>> featureMatchedImages;
    // load a line from CSV file
    std::string line;
    while(std::getline(featureFile, line, '\n')){

        // vector of features
        std::vector<int> fv;
        std::string x;
        std::stringstream a(line);
        int counter = 0;
        std::string imagePath;
        while (std::getline(a, x, ',')) {
            if(counter == 0){
                imagePath = x;
            }else{
                fv.push_back(stoi(x));
            }
            counter += 1;
        }

        featureMatchedImages.push_back( std::make_pair(imagePath, sumOfSquaredDifferences(fv, targetImageFeatures)));
    }
    sort(featureMatchedImages.begin(), featureMatchedImages.end(), sortPair);
    // std::cout<<featureMatchedImages[0].first<<" "<<featureMatchedImages[0].second;
    featureFile.close();
    return featureMatchedImages;
}

// a function to fetch images based on rg histogram IR
std::vector<std::pair<std::string, float>> rgMatching(std::string targetImageName){
    // Matches the image passed through URI with the images in database based on 9*9 features
    std::vector<int> targetImageFeatures = imageFeatures(targetImageName, "rgfeatures.csv");

    // calculate SSD for all images in the search space
    
    // load feature file 
    std::ifstream featureFile("rgfeatures.csv");
    if(!featureFile){
        std::cout<<"file not found"<<std::endl;
    }

    std::vector<std::pair<std::string, float>> featureMatchedImages;
    // load a line from CSV file
    std::string line;
    while(std::getline(featureFile, line, '\n')){

        // vector of features
        std::vector<int> fv;
        std::string x;
        std::stringstream a(line);
        int counter = 0;
        std::string imagePath;
        while (std::getline(a, x, ',')) {
            if(counter == 0){
                imagePath = x;
            }else{
                fv.push_back(stoi(x));
            }
            counter += 1;
        }
        featureMatchedImages.push_back( std::make_pair(imagePath, histogramIntersection(fv, targetImageFeatures)));
    }
    sort(featureMatchedImages.begin(), featureMatchedImages.end(), sortPair);
    featureFile.close();
    return featureMatchedImages;
}

// a function to fetch images based on multihistogram IR
std::vector<std::pair<std::string, float>> multiMatching(std::string targetImageName){
    // Matches the image passed through URI with the images in database based on 9*9 features
    std::vector<int> targetImageFeatures = imageFeatures(targetImageName, "multifeatures.csv");
    std::vector<int> targetImageFeatures1 = slice(targetImageFeatures, 0, 255);
    std::vector<int> targetImageFeatures2 = slice(targetImageFeatures, 256, 511);
    
    // calculate SSD for all images in the search space
    
    // load feature file 
    std::ifstream featureFile("multifeatures.csv");
    if(!featureFile){
        std::cout<<"file not found"<<std::endl;
    }

    std::vector<std::pair<std::string, float>> featureMatchedImages;
    // load a line from CSV file
    std::string line;
    while(std::getline(featureFile, line, '\n')){

        // vector of features
        std::vector<int> fv1;
        std::vector<int> fv2;
        std::string x;
        std::stringstream a(line);
        int counter = 0;
        std::string imagePath;
        while (std::getline(a, x, ',')) {
            if(counter == 0){
                imagePath = x;
            }else if(counter <= 256){
                fv1.push_back(stoi(x));
            }else{
                fv2.push_back(stoi(x));
            }
            counter += 1;
        }
        float score =  histogramIntersection(fv1, targetImageFeatures1)  +  histogramIntersection(fv2, targetImageFeatures2);
        featureMatchedImages.push_back( std::make_pair(imagePath, score));
    }
    sort(featureMatchedImages.begin(), featureMatchedImages.end(), sortPair);
    featureFile.close();
    return featureMatchedImages;
}

// a function to fetch images based on whole image and texture histogram matching
std::vector<std::pair<std::string, float>> textureMatching(std::string targetImageName){
    std::vector<int> targetImageFeatures = imageFeatures(targetImageName, "textureFeatures.csv");
    std::vector<int> targetImageFeatures1 = slice(targetImageFeatures, 0, 111);
    std::vector<int> targetImageFeatures2 = slice(targetImageFeatures, 112, 367);
    
    // calculate SSD for all images in the search space
    
    // load feature file 
    std::ifstream featureFile("textureFeatures.csv");
    if(!featureFile){
        std::cout<<"file not found"<<std::endl;
    }

    std::vector<std::pair<std::string, float>> featureMatchedImages;
    // // load a line from CSV file
    std::string line;
    while(std::getline(featureFile, line, '\n')){

        // vector of features
        std::vector<int> fv1;
        std::vector<int> fv2;
        std::string x;
        std::stringstream a(line);
        int counter = 0;
        std::string imagePath;
        while (std::getline(a, x, ',')) {
            if(counter == 0){
                imagePath = x;
            }else if(counter <= 112){
                fv1.push_back(stoi(x));
            }else{
                fv2.push_back(stoi(x));
            }
            counter += 1;
        }
        float score =  histogramIntersection(fv1, targetImageFeatures1)  +  histogramIntersection(fv2, targetImageFeatures2);
        featureMatchedImages.push_back( std::make_pair(imagePath, score));
    }
    sort(featureMatchedImages.begin(), featureMatchedImages.end(), sortPair);
    featureFile.close();
    return featureMatchedImages;

}


std::vector<std::pair<std::string, float>> patchMatching(std::string targetImageName){
    // Matches the image passed through URI with the images in database based on 9*9 features
    std::vector<int> targetImageFeatures = imageFeatures(targetImageName, "patchHistogram.csv");
    std::vector<int> targetImageFeatures1 = slice(targetImageFeatures, 0, 242);
    std::vector<int> targetImageFeatures2 = slice(targetImageFeatures, 243, 498);
    
    std::cout<<targetImageFeatures1.size() <<" " << targetImageFeatures2.size()<<std::endl;
    // calculate SSD for all images in the search space
    
    // load feature file 
    std::ifstream featureFile("patchHistogram.csv");
    if(!featureFile){
        std::cout<<"file not found"<<std::endl;
    }

    std::vector<std::pair<std::string, float>> featureMatchedImages;
    // load a line from CSV file
    std::string line;
    while(std::getline(featureFile, line, '\n')){

        // vector of features
        std::vector<int> fv1;
        std::vector<int> fv2;
        std::string x;
        std::stringstream a(line);
        int counter = 0;
        std::string imagePath;
        while (std::getline(a, x, ',')) {
            if(counter == 0){
                imagePath = x;
            }else if(counter <= 243){
                fv1.push_back(stoi(x));
            }else{
                fv2.push_back(stoi(x));
            }
            counter += 1;
        }
        float score =  sumOfSquaredDifferences(fv1, targetImageFeatures1) * 1e-6 +  histogramIntersection(fv2, targetImageFeatures2);
        featureMatchedImages.push_back( std::make_pair(imagePath, score));
    }
    sort(featureMatchedImages.begin(), featureMatchedImages.end(), sortPair);
    featureFile.close();
    return featureMatchedImages;
}
int main(){

    int question = 5;
    int taskMode = 1; // 0 for feature generation, 1 to perform search
    char buffer[256] = "C:\\Users\\afrid\\Desktop\\CBIR\\olympus"; // folder of images
    if(taskMode == 0){
        featureCalculation(question, buffer);
    }else if(taskMode == 1){
        if(question == 1){
            std::vector<std::pair<std::string, int>> matchedImages = baselineMatching("C:\\Users\\afrid\\Desktop\\CBIR\\olympus\\pic.1016.jpg");
            for(int i=0; i<10; i++){
                std::cout<<matchedImages[i].first<<" "<<matchedImages[i].second<<std::endl;
            }
        }else if(question == 2){
            std::vector<std::pair<std::string, float>> matchedImages = rgMatching("C:\\Users\\afrid\\Desktop\\CBIR\\olympus\\pic.0164.jpg");
            for(int i=0; i<10; i++){
                std::cout<<matchedImages[i].first<<" "<<matchedImages[i].second<<std::endl;
            }
        }else if(question == 3){
            std::vector<std::pair<std::string, float>> matchedImages = multiMatching("C:\\Users\\afrid\\Desktop\\CBIR\\olympus\\pic.0135.jpg");
            for(int i=0; i<10; i++){
                std::cout<<matchedImages[i].first<<" "<<matchedImages[i].second<<std::endl;
            }
        }else if(question == 4){
            std::vector<std::pair<std::string, float>> matchedImages = textureMatching("C:\\Users\\afrid\\Desktop\\CBIR\\olympus\\pic.0535.jpg");
            for(int i=0; i<10; i++){
                std::cout<<matchedImages[i].first<<" "<<matchedImages[i].second<<std::endl;
            }
        }else if(question == 5){
            std::vector<std::pair<std::string, float>> matchedImages = patchMatching("C:\\Users\\afrid\\Desktop\\CBIR\\olympus\\pic.0040.jpg");
            for(int i=0; i<11; i++){
                std::cout<<matchedImages[i].first<<" "<<matchedImages[i].second<<std::endl;
            }
        }
    }

    // generate baseline features

    // Question  01:
    // std::vector<std::pair<std::string, int>> matchedImages = baselineMatching("C:\\Users\\afrid\\Desktop\\CBIR\\olympus\\pic.1016.jpg");
    // for(int i=0; i<10; i++){
    //     std::cout<<matchedImages[i].first<<" "<<matchedImages[i].second<<std::endl;
    // }

    // featureCalculation(2);

    // Question 02
    // std::vector<std::pair<std::string, float>> matchedImages = rgMatching("C:\\Users\\afrid\\Desktop\\CBIR\\olympus\\pic.0164.jpg");
    // for(int i=0; i<50; i++){
    //     std::cout<<matchedImages[i].first<<" "<<matchedImages[i].second<<std::endl;
    // }


    // Question 03
    // featureCalculation(3);
    // std::vector<std::pair<std::string, float>> matchedImages = multiMatching("C:\\Users\\afrid\\Desktop\\CBIR\\olympus\\pic.0135.jpg");
    // for(int i=0; i<10; i++){
    //     std::cout<<matchedImages[i].first<<" "<<matchedImages[i].second<<std::endl;
    // }

    // Question 04
    // featureCalculation(4);
    // std::vector<std::pair<std::string, float>> matchedImages = textureMatching("C:\\Users\\afrid\\Desktop\\CBIR\\olympus\\pic.0135.jpg");
    // for(int i=0; i<10; i++){
    //     std::cout<<matchedImages[i].first<<" "<<matchedImages[i].second<<std::endl;
    // }

    // featureCalculation(5);
    // std::vector<std::pair<std::string, float>> matchedImages = patchMatching("C:\\Users\\afrid\\Desktop\\CBIR\\olympus\\pic.0135.jpg");
    // for(int i=0; i<11; i++){
    //     std::cout<<matchedImages[i].first<<" "<<matchedImages[i].second<<std::endl;
    // }

}