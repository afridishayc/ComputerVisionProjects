#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <math.h>
#include <opencv2/imgproc.hpp>
#include <fstream>


void writeCSV(char* filename, std::string csvString){
    // writes csv string to the given file
    std::ofstream myFile(filename, std::ios_base::app);
    myFile << csvString <<"\n";
    myFile.close();
}

int thresoldImage(cv::Mat &src, cv::Mat &dst){
    // takes in the rgb image, converts it to grayscale and then creates a binary image using thresold
    cv::Mat grayImg(src.rows, src.cols, CV_8UC1, cv::Scalar(0));
    cv::cvtColor(src, grayImg, cv::COLOR_BGR2GRAY);
    for(int i=0; i<grayImg.rows; i++){
        for(int j=0; j<grayImg.cols; j++){
            // std::cout<<(short)grayImg.at<uchar>(i, j)<<std::endl;
            if ((short)grayImg.at<uchar>(i, j) < 100){
                dst.at<uchar>(i, j) = 255;
            }
        }
    }
    return 0;
}

int thresoldVal(cv::Mat &src){
    // a function to print the histogram of pixel values to help in manually selecting thresold value 
    int histogram[255] = {0};
    for(int i=0; i<src.rows; i++){
        for(int j=0; j<src.cols; j++){
            histogram[(short)src.at<uchar>(i, j)] += 1;         
        }
    }
    for(int k=0; k<255; k++){
        std::cout<< k + 1 << " : " << histogram[k] <<std::endl;
    }
    return 0;
}


int erode(cv::Mat &src, cv::Mat &dst){
    // performs 4 connect erosion
    for(int i=1; i<src.rows-1; i++){
        for(int j=1; j<src.cols-1; j++){
            if( (short)src.at<uchar>(i-1, j) > 250 || (short)src.at<uchar>(i, j-1) > 250 || (short)src.at<uchar>(i, j+1) 
            > 250 ||  (short)src.at<uchar>(i+1, j) > 250){
                dst.at<uchar>(i, j) = 255;
            }else{
                dst.at<uchar>(i, j) = 0;
            }
        }
    }

    return 0;
}


int dilate(cv::Mat &src, cv::Mat &dst){
    // performs 4 connect dilation
    for(int i=1; i<src.rows-1; i++){
        for(int j=1; j<src.cols-1; j++){
            if( (short)src.at<uchar>(i-1, j) < 5 || (short)src.at<uchar>(i, j-1) < 5 || (short)src.at<uchar>(i, j+1) 
            < 5 ||  (short)src.at<uchar>(i+1, j) < 5){
                dst.at<uchar>(i, j) = 0;
            }else{
                dst.at<uchar>(i, j) = 255;
            }
        }
    }

    return 0;
}

int findRoot(int ds[], int val){
    // union find function
    int parent = ds[val];
    while (parent != -1){
        val = parent;
        parent = ds[val];
    }
    return val;
}

std::vector<int> connectedComponentLabel(cv::Mat &src, cv::Mat &dst, cv::Mat &vis){
    // labels regions of a binary image, also returns the region IDs as a vector
    int regionID = 1;
    int unionFind[500];
    std::map<int, int> regionCounter;
    std::vector<int> regions;
    std::vector<cv::Scalar> regionColors = { cv::Scalar(228, 117, 83), cv::Scalar(83, 117, 228), cv::Scalar(30, 112, 255) };
    for(int k=0; k < 500 ; k++){    
        unionFind[k] = -1;
    }

    for(int i=1; i<src.rows-1; i++){
        for(int j=1; j<src.cols-1; j++){
            if( (short)src.at<uchar>(i, j) == 255 ){
                // fetch label of neighbor pixels
                int topLabel = (short)dst.at<uchar>(i-1, j);
                int leftLabel = (short)dst.at<uchar>(i, j-1);
                if(topLabel == 0 && leftLabel == 0){
                    dst.at<uchar>(i, j) = regionID;
                    regionID++;
                    regionCounter.insert(std::make_pair(regionID, 0));
                }else if(topLabel == 0){
                    dst.at<uchar>(i, j) = leftLabel;
                }else if(leftLabel == 0){
                    dst.at<uchar>(i, j) = topLabel;
                }else{
                    int minVal = std::min(topLabel, leftLabel);
                    int maxVal = std::max(topLabel, leftLabel);
                    dst.at<uchar>(i, j) = minVal;
                    if(minVal != maxVal){
                        unionFind[maxVal] = minVal;
                    }
                }
            }
        }
    }  

    for(int i=1; i<src.rows-1; i++){
        for(int j=1; j<src.cols-1; j++){
            if( dst.at<uchar>(i, j) != 0 && unionFind[dst.at<uchar>(i, j)] != -1 ){
                dst.at<uchar>(i, j) =  findRoot(unionFind, (short)dst.at<uchar>(i, j));
            }
        }
    }

    for(int i=1; i<src.rows-1; i++){
        for(int j=1; j<src.cols-1; j++){
            regionCounter[(short) dst.at<uchar>(i, j)] += 1;
        }
    }

    int colorIdx = 0;
    for(int m = 0; m < regionCounter.size(); m++){
        if( m != 0 && regionCounter[m] > 1000){
            std::cout<<m<<std::endl;
            // regionColors
            regions.push_back(m);
            for(int i=1; i<src.rows-1; i++){
                for(int j=1; j<src.cols-1; j++){
                    if( (short)dst.at<uchar>(i, j) == m){
                        vis.at<cv::Vec3b>(i, j)[0] = regionColors[colorIdx][0];
                        vis.at<cv::Vec3b>(i, j)[1] = regionColors[colorIdx][1];
                        vis.at<cv::Vec3b>(i, j)[2] = regionColors[colorIdx][2];
                    }
                }
            }
            colorIdx++;
        }
    }

    return regions;
}
 
float M00(cv::Mat &regionMap, int targetRegionID){
    // returns m00 - unused function
    float result = 0;
    for(int i=0; i < regionMap.rows; i++){
        for( int j=0; j < regionMap.cols; j++){
            if( (short)regionMap.at<uchar>(i, j) == targetRegionID){
                result++;
            }
        }
    }
    return result;
}


std::pair<float, float> centroid(cv::Mat &regionMap, int targetRegionID, float m00){
    // returns centroid - unused function
    float m10 = 0;
    float m01 = 0; 
    for(int y=0; y < regionMap.rows; y++){
        for( int x=0; x < regionMap.cols; x++){
            if( (short)regionMap.at<uchar>(y, x) == targetRegionID){
                m10 += x;
                m01 += y;
            }
        }
    }
    m10 = m10/m00;
    m01 = m01/m00;
    return std::make_pair(m10, m01);
}

float MU11(cv::Mat &regionMap, int targetRegionID, float moo){
    // returns m11 - unused function
    float result = 0;
    std::pair<float, float> centr = centroid(regionMap, targetRegionID, moo);
    for(int y=0; y < regionMap.rows; y++){
        for( int x=0; x < regionMap.cols; x++){
            if( (short)regionMap.at<uchar>(y, x) == targetRegionID){
                result += (x - centr.first) * (y - centr.second);
            }
        }
    }
    return result;
}

std::pair<float, float> centralMoments(cv::Mat &regionMap, int targetRegionID, float moo){
    // returns central moments - unused function
    float mu02 = 0;
    float mu20 = 0;
    std::pair<float, float> centr = centroid(regionMap, targetRegionID, moo);
    for(int i=0; i < regionMap.rows; i++){
        for( int j=0; j < regionMap.cols; j++){
            if( (short)regionMap.at<uchar>(i, j) == targetRegionID ){
                mu02 += (j - centr.second) * (j - centr.second);
                mu20 += (i - centr.first) * (i - centr.first);
            }
        }
    }
    mu02 = mu02/moo;
    mu20 = mu20/moo;
    return std::make_pair(mu02, mu20);
}


float centralAxis(cv::Mat &regionMap, int targetRegionID, float mu11, float mu02, float mu20){
    return 0.5 * std::atan( 2 * mu11 / (mu20 - mu02) );
}



std::vector<float> calcMoments(cv::Mat &regionMap, int targetRegionID){
    // returns features vector for a region map and calculates features of a region based on the target region ID
    // calculates central moments and hue moments 
    std::vector<float> momentVec;
    float m00 = 0;
    float m10 = 0;
    float m01 = 0;
    float x_bar = 0;
    float y_bar = 0;
    float m11 = 0;
    float m20 = 0;
    float m02 = 0;
    float m12 = 0;
    float m21 = 0;
    float m03 = 0;
    float m30 = 0;
    float mu11 = 0;
    float mu02 = 0;
    float mu20 = 0;
    float mu21 = 0;
    float mu12 = 0;
    float mu03 = 0;
    float mu30 = 0;
    
    float nu11 = 0;
    float nu20 = 0;
    float nu02 = 0;
    float nu21 = 0;
    float nu12 = 0;
    float nu03 = 0;
    float nu30 = 0;
    float norm1 = 0;
    float norm2 = 0;
    float alpha = 0;
    float mu22a = 0;    

    float h1 = 0;
    float h2 = 0;
    float h3 = 0;
    float h4 = 0;
    float h5 = 0;

    for(int y=0; y < regionMap.rows; y++){
        for( int x=0; x < regionMap.cols; x++){
            if( (short)regionMap.at<uchar>(y, x) == targetRegionID){
                m00++;
                m10 += x;
                m01 += y;
                m11 += x * y;
                m20 += x * x;
                m02 += y * y;
                m21 += x * x * y;
                m12 += y * y * x;
                m03 += y * y * y;
                m30 += x * x * x;
            }
        }
    }

    x_bar = m10 / m00;
    y_bar = m01 / m00;

    for(int y=0; y < regionMap.rows; y++){
        mu02 += ( y - y_bar) * ( y - y_bar);
    }

    for(int x=0; x < regionMap.cols; x++){
        mu20 += ( x - x_bar) * ( x - x_bar);
    }

    mu20 = m20 - m00 * x_bar * x_bar;
    mu11 = m11 - m00 * x_bar * y_bar;
    mu02 = m02 - m00 * y_bar * y_bar;
    
    
    mu30 = m30 - (3 * m20 * x_bar) + (2 * m00 * x_bar * x_bar * x_bar);
    
    mu21 = m21 - (m20 * y_bar) - (2 * m11 * x_bar) + (2 * m00 * x_bar * x_bar * y_bar);
    mu21 =  m21 - (2 * x_bar * m11) - (y_bar * m20) + (2 * x_bar * x_bar * m01);
    mu12 = m12 - (m02 * x_bar) - (2 * m11 * y_bar) + (2 * m00 * x_bar * y_bar * y_bar);
    mu03 = m03 - (3 * m02 * y_bar) + (2 * m00 * y_bar * y_bar * y_bar);

    norm1 = 1/m00;
    norm2 = std::sqrt(std::abs(norm1));
    float normx = norm1*norm1;
    float normy = normx*norm2;

    nu20 = mu20 * normx; 
    nu11 = mu11 * normx; 
    nu02 = mu02 * normx;

    nu30 = mu30 * normy; 
    nu21 = mu21 * normy; 
    nu12 = mu12 * normy; 
    nu03 = mu03 * normy;


    h1 = nu20 + nu02;
    h2 = (nu20 - nu02) * (nu20 - nu02) + (4 * nu11 * nu11);
    h3 = (nu30 - 3*nu12) * (nu30 - 3*nu12) + (3*nu21 - nu03) * (3*nu21 - nu03);
    h4 = (nu30 + nu12) * (nu30 + nu12) + (nu21 + nu03) * (nu21 + nu03);

    momentVec.push_back(h1);
    momentVec.push_back(h2);
    momentVec.push_back(h3);
    momentVec.push_back(h4);
    return momentVec;
}

std::vector<float> calcStd(){
    // calculate mean and sd of features in objects csv
    std::vector<float> dev(4, 0);
    std::vector<float> mean(4, 0);
    std::ifstream featureFile("objects.csv");
    if(!featureFile){
        std::cout<<"file not found"<<std::endl;
    }
    std::string line;
    int points = 0;
    while(std::getline(featureFile, line, '\n')){
        std::string x;
        std::stringstream a(line);
        int counter = 0;
        while (std::getline(a, x, ',')) {
            // fv.push_back(stof(x));
            if(counter != 0){
                mean[counter-1] += stof(x);
            }
            counter++;
        }  
        points++;     
    }
    // featureFile.close();
    for(int z=0; z<mean.size(); z++){
        mean[z] = mean[z] / points;
    }
    
    featureFile.clear();
    featureFile.seekg(0);
    std::string linex;
    while(std::getline(featureFile, linex, '\n')){
        std::string x;
        std::stringstream a(linex);
        int counter = 0;
        // std::cout<<"again" <<std::endl;
        while (std::getline(a, x, ',')) {
            if(counter != 0){
                dev[counter-1] += (stof(x) - mean[counter-1]) * (stof(x) - mean[counter-1]);
                // std::cout<<dev[counter-1]<<std::endl;
            }
            counter++;
        }     
    }
    featureFile.close();

    return dev;
}

float normEuclideanDistance(std::vector<float> &a, std::vector<float> &b, std::vector<float> &sd){
    // calcualtes normalized euclidean distance between two vectors
    float result = 0;
    for(int i=0; i<a.size(); i++){
        result += ((a[i] - b[i]) * (a[i] - b[i])) / (sd[i] * sd[i]) ;
    }
    return result;
}

bool sortPair(const std::pair<std::string, float> &v1, const std::pair<std::string, float> &v2){
    // sorts the pair based on its value
    return (v1.second < v2.second);
}

std::string nearestLabel(std::vector<float> targetFeatures){
    // returns the label of the nearest (by normalized Euclidean distance) vector to the target features
    std::vector<std::pair<std::string, float>> topMatches; 
    std::vector<float> sd = calcStd();
    std::ifstream featureFile("objects.csv");
    if(!featureFile){
        std::cout<<"file not found"<<std::endl;
    }
    std::string line;
    while(std::getline(featureFile, line, '\n')){
        std::vector<float> fv;
        std::string x;
        std::stringstream a(line);
        int counter = 0;
        std::string currentLabel = "";
        while (std::getline(a, x, ',')) {
            // fv.push_back(stof(x));
            if(counter == 0){
                currentLabel = x;
            }else{
                fv.push_back(stof(x));
            }
            counter++;
        }
        float e = normEuclideanDistance(targetFeatures, fv, sd);
        topMatches.push_back(std::make_pair(currentLabel, e));
    }
    sort(topMatches.begin(), topMatches.end(), sortPair);
    return topMatches[0].first;
}

std::string knn(std::vector<float> targetFeatures, int neighbors){
    // returns the label of the best k nearest neighbors (3 in this case)
    std::vector<std::pair<std::string, float>> topMatches;
    std::map<std::string, float> myDict;
    std::map<std::string, int> myDictCount;
    std::vector<std::pair<std::string, float>> labelDists; 
    std::vector<float> sd = calcStd();
    std::ifstream featureFile("objects.csv");
    if(!featureFile){
        std::cout<<"file not found"<<std::endl;
    }
    std::string line;
    while(std::getline(featureFile, line, '\n')){
        std::vector<float> fv;
        std::string x;
        std::stringstream a(line);
        int counter = 0;
        std::string currentLabel = "";
        while (std::getline(a, x, ',')) {
            // fv.push_back(stof(x));
            if(counter == 0){
                currentLabel = x;
            }else{
                fv.push_back(stof(x));
            }
            counter++;
        }
        float e = normEuclideanDistance(targetFeatures, fv, sd);
        topMatches.push_back(std::make_pair(currentLabel, e));
    }
    sort(topMatches.begin(), topMatches.end(), sortPair);
    for(int z=0; z < topMatches.size(); z++){
        if(myDict.count(topMatches[z].first) == 0){
            myDict.insert( std::make_pair( topMatches[z].first, topMatches[z].second ));
            myDictCount.insert( std::make_pair( topMatches[z].first, 1 ));
        }else{
            if(myDictCount[topMatches[z].first] < neighbors){
                myDict[topMatches[z].first] += topMatches[z].second;
                myDictCount[topMatches[z].first] += 1;
            }
        }
    }
    for(auto p:myDict){
        labelDists.push_back(std::make_pair(p.first, p.second));
    }
    sort(labelDists.begin(), labelDists.end(), sortPair);
    return labelDists[0].first;
}