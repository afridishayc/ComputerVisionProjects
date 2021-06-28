
#include<math.h>
#include<opencv2/opencv.hpp>

cv::Mat negativeImg(cv::Mat frame){
        for(int i=0; i<frame.rows; i++){
                for(int j=0; j<frame.cols; j++){
                        frame.at<cv::Vec3b>(i, j)[0] = 255 - frame.at<cv::Vec3b>(i, j)[0];
                        frame.at<cv::Vec3b>(i, j)[1] = 255 - frame.at<cv::Vec3b>(i, j)[1];
                        frame.at<cv::Vec3b>(i, j)[2] = 255 - frame.at<cv::Vec3b>(i, j)[2];
                }
        }      

        return frame;
};

int blur5x5(cv::Mat &src, cv::Mat &dst){
        cv::Mat imgx(src.rows, src.cols, CV_8UC3, cv::Scalar(255, 255, 255));

        int filter[5] = {1, 2, 4, 2, 1};
        for(int i=0; i<src.rows; i++){
                for(int j=0; j<src.cols; j++){
                        int jindices[5] = {j-2, j-1, j, j+1, j+2};
                        for(int p=0; p<5; p++){
                                if(jindices[p] < 0){
                                        jindices[p] = (src.cols) + jindices[p];
                                }else if(jindices[p] > (src.cols - 1)){
                                        jindices[p] = jindices[p] - src.cols; 
                                }
                        }
                        
                        imgx.at<cv::Vec3b>(i, j)[0] = (src.at<cv::Vec3b>(i, jindices[0])[0] * filter[0] + src.at<cv::Vec3b>(i, jindices[1])[0] * filter[1] + src.at<cv::Vec3b>(i, jindices[2])[0] * filter[2] + src.at<cv::Vec3b>(i, jindices[3])[0] * filter[3] + src.at<cv::Vec3b>(i, jindices[4])[0] * filter[4]) / 10;

                        imgx.at<cv::Vec3b>(i, j)[1] = (src.at<cv::Vec3b>(i, jindices[0])[1] * filter[0] + src.at<cv::Vec3b>(i, jindices[1])[1] * filter[1] + src.at<cv::Vec3b>(i, jindices[2])[1] * filter[2] + src.at<cv::Vec3b>(i, jindices[3])[1] * filter[3] + src.at<cv::Vec3b>(i, jindices[4])[1] * filter[4]) / 10;

                        imgx.at<cv::Vec3b>(i, j)[2] = (src.at<cv::Vec3b>(i, jindices[0])[2] * filter[0] + src.at<cv::Vec3b>(i, jindices[1])[2] * filter[1] + src.at<cv::Vec3b>(i, jindices[2])[2] * filter[2] + src.at<cv::Vec3b>(i, jindices[3])[2] * filter[3] + src.at<cv::Vec3b>(i, jindices[4])[2] * filter[4]) / 10;
                }
        }

        for(int i=0; i<src.rows; i++){
                for(int j=0; j<src.cols; j++){
                        int iindices[5] = {i-2, i-1, i, i+1, i+2};
                        for(int p=0; p<5; p++){
                                if(iindices[p] < 0){
                                        iindices[p] = (src.rows) + iindices[p];
                                }else if(iindices[p] > (src.rows - 1)){
                                        iindices[p] = iindices[p] - src.rows; 
                                }
                        }

                        dst.at<cv::Vec3b>(i, j)[0] = (imgx.at<cv::Vec3b>(iindices[0], j)[0] * filter[0] + imgx.at<cv::Vec3b>(iindices[1], j)[0] * filter[1] + imgx.at<cv::Vec3b>(iindices[2], j)[0] * filter[2] + imgx.at<cv::Vec3b>(iindices[3], j)[0] * filter[3] + imgx.at<cv::Vec3b>(iindices[4], j)[0] * filter[4]) / 10;

                        dst.at<cv::Vec3b>(i, j)[1] = (imgx.at<cv::Vec3b>(iindices[0], j)[1] * filter[0] + imgx.at<cv::Vec3b>(iindices[1], j)[1] * filter[1] + imgx.at<cv::Vec3b>(iindices[2], j)[1] * filter[2] + imgx.at<cv::Vec3b>(iindices[3], j)[1] * filter[3] + imgx.at<cv::Vec3b>(iindices[4], j)[1] * filter[4]) / 10;

                        dst.at<cv::Vec3b>(i, j)[2] = (imgx.at<cv::Vec3b>(iindices[0], j)[2] * filter[0] + imgx.at<cv::Vec3b>(iindices[1], j)[2] * filter[1] + imgx.at<cv::Vec3b>(iindices[2], j)[2] * filter[2] + imgx.at<cv::Vec3b>(iindices[3], j)[2] * filter[3] + imgx.at<cv::Vec3b>(iindices[4], j)[2] * filter[4]) / 10;
                }
                
        }
        return 0;
};

int sobolX3x3(cv::Mat &src, cv::Mat &dst){

        src.convertTo(src, CV_16SC3);

        cv::Mat imgx(src.rows, src.cols, CV_16SC3, cv::Scalar(255, 255, 255));
        int filter_1[3] = {-1, 0, 1};
        int filter_2[3] = {1, 2, 1};

        for(int i=0; i<src.rows; i++){
                for(int j=0; j<src.cols; j++){
                        int iindices[3] = {i-1, i, i+1};
                        for(int p=0; p<3; p++){
                                if(iindices[p] < 0){
                                        iindices[p] = (src.rows) + iindices[p];
                                }else if(iindices[p] > (src.rows - 1)){
                                        iindices[p] = iindices[p] - src.rows; 
                                }
                        }

                        imgx.at<cv::Vec3s>(i, j)[0] = (src.at<cv::Vec3s>(iindices[0], j)[0] * filter_2[0] + src.at<cv::Vec3s>(iindices[1], j)[0] * filter_2[1] + src.at<cv::Vec3s>(iindices[2], j)[0] * filter_2[2]) / 4;

                        imgx.at<cv::Vec3s>(i, j)[1] = (src.at<cv::Vec3s>(iindices[0], j)[1] * filter_2[0] + src.at<cv::Vec3s>(iindices[1], j)[1] * filter_2[1] + src.at<cv::Vec3s>(iindices[2], j)[1] * filter_2[2]) / 4;
                        
                        imgx.at<cv::Vec3s>(i, j)[2] = (src.at<cv::Vec3s>(iindices[0], j)[2] * filter_2[0] + src.at<cv::Vec3s>(iindices[1], j)[2] * filter_2[1] + src.at<cv::Vec3s>(iindices[2], j)[2] * filter_2[2]) / 4;
                }
                
        }

        for(int i=0; i<src.rows; i++){
                for(int j=0; j<src.cols; j++){
                        int jindices[3] = {j-1, j, j+1};
                        for(int p=0; p<3; p++){
                                if(jindices[p] < 0){
                                        jindices[p] = (src.cols) + jindices[p];
                                }else if(jindices[p] > (src.cols - 1)){
                                        jindices[p] = jindices[p] - src.cols; 
                                }
                        }
                        
                        dst.at<cv::Vec3s>(i, j)[0] = (imgx.at<cv::Vec3s>(i, jindices[0])[0] * filter_1[0] + imgx.at<cv::Vec3s>(i, jindices[1])[0] * filter_1[1] + imgx.at<cv::Vec3s>(i, jindices[2])[0] * filter_1[2]) ;

                        dst.at<cv::Vec3s>(i, j)[1] = (imgx.at<cv::Vec3s>(i, jindices[0])[1] * filter_1[0] + imgx.at<cv::Vec3s>(i, jindices[1])[1] * filter_1[1] + imgx.at<cv::Vec3s>(i, jindices[2])[1] * filter_1[2]);

                        dst.at<cv::Vec3s>(i, j)[2] = (imgx.at<cv::Vec3s>(i, jindices[0])[2] * filter_1[0] + imgx.at<cv::Vec3s>(i, jindices[1])[2] * filter_1[1] + imgx.at<cv::Vec3s>(i, jindices[2])[2] * filter_1[2]);
                }       
        }


        return 0;
};

int sobolY3x3(cv::Mat &src, cv::Mat &dst){

        src.convertTo(src, CV_16SC3);

        cv::Mat imgx(src.rows, src.cols, CV_16SC3, cv::Scalar(255, 255, 255));
        int filter_1[3] = {1, 2, 1}; 
        int filter_2[3] = {-1, 0, 1};

        for(int i=0; i<src.rows; i++){
                for(int j=0; j<src.cols; j++){
                        int iindices[3] = {i-1, i, i+1};
                        for(int p=0; p<3; p++){
                                if(iindices[p] < 0){
                                        iindices[p] = (src.rows) + iindices[p];
                                }else if(iindices[p] > (src.rows - 1)){
                                        iindices[p] = iindices[p] - src.rows; 
                                }
                        }

                        imgx.at<cv::Vec3s>(i, j)[0] = (src.at<cv::Vec3s>(iindices[0], j)[0] * filter_2[0] + src.at<cv::Vec3s>(iindices[1], j)[0] * filter_2[1] + src.at<cv::Vec3s>(iindices[2], j)[0] * filter_2[2]);

                        imgx.at<cv::Vec3s>(i, j)[1] = (src.at<cv::Vec3s>(iindices[0], j)[1] * filter_2[0] + src.at<cv::Vec3s>(iindices[1], j)[1] * filter_2[1] + src.at<cv::Vec3s>(iindices[2], j)[1] * filter_2[2]);
                        
                        imgx.at<cv::Vec3s>(i, j)[2] = (src.at<cv::Vec3s>(iindices[0], j)[2] * filter_2[0] + src.at<cv::Vec3s>(iindices[1], j)[2] * filter_2[1] + src.at<cv::Vec3s>(iindices[2], j)[2] * filter_2[2]);
                }
                
        }

        for(int i=0; i<src.rows; i++){
                for(int j=0; j<src.cols; j++){
                        int jindices[3] = {j-1, j, j+1};
                        for(int p=0; p<3; p++){
                                if(jindices[p] < 0){
                                        jindices[p] = (src.cols) + jindices[p];
                                }else if(jindices[p] > (src.cols - 1)){
                                        jindices[p] = jindices[p] - src.cols; 
                                }
                        }
                        
                        dst.at<cv::Vec3s>(i, j)[0] = (imgx.at<cv::Vec3s>(i, jindices[0])[0] * filter_1[0] + imgx.at<cv::Vec3s>(i, jindices[1])[0] * filter_1[1] + imgx.at<cv::Vec3s>(i, jindices[2])[0] * filter_1[2]) / 4;

                        dst.at<cv::Vec3s>(i, j)[1] = (imgx.at<cv::Vec3s>(i, jindices[0])[1] * filter_1[0] + imgx.at<cv::Vec3s>(i, jindices[1])[1] * filter_1[1] + imgx.at<cv::Vec3s>(i, jindices[2])[1] * filter_1[2]) / 4;

                        dst.at<cv::Vec3s>(i, j)[2] = (imgx.at<cv::Vec3s>(i, jindices[0])[2] * filter_1[0] + imgx.at<cv::Vec3s>(i, jindices[1])[2] * filter_1[1] + imgx.at<cv::Vec3s>(i, jindices[2])[2] * filter_1[2]) / 4;
                }       
        }

        return 0;
};

int magnitude( cv::Mat &sx, cv::Mat &sy, cv::Mat &dst ){

        for(int i=0; i<sx.rows; i++){
                for(int j=0; j<sx.cols; j++){
                        dst.at<cv::Vec3s>(i, j)[0] = sqrt( sx.at<cv::Vec3s>(i, j)[0] * sx.at<cv::Vec3s>(i, j)[0] + sy.at<cv::Vec3s>(i, j)[0] * sy.at<cv::Vec3s>(i, j)[0]);

                        dst.at<cv::Vec3s>(i, j)[1] = sqrt( sx.at<cv::Vec3s>(i, j)[1] * sx.at<cv::Vec3s>(i, j)[1] + sy.at<cv::Vec3s>(i, j)[1] * sy.at<cv::Vec3s>(i, j)[1]);
                        
                        dst.at<cv::Vec3s>(i, j)[2] = sqrt( sx.at<cv::Vec3s>(i, j)[2] * sx.at<cv::Vec3s>(i, j)[2] + sy.at<cv::Vec3s>(i, j)[2] * sy.at<cv::Vec3s>(i, j)[2]);   

                        // std::cout<<dst.at<cv::Vec3s>(i, j)<<std::endl;         
                }
        }
        
        // dst = (dst / 2) + 127;
        
        dst.convertTo(dst, CV_8UC3);
        return 0;
};

int blurQuantize( cv::Mat &src, cv::Mat &dst, int levels ){

        int b = 255 / levels;
        for(int i=0; i<src.rows; i++){
                for(int j=0; j<src.cols; j++){
                        dst.at<cv::Vec3b>(i, j)[0] = (src.at<cv::Vec3b>(i, j)[0] / b) * b;
                        dst.at<cv::Vec3b>(i, j)[1] = (src.at<cv::Vec3b>(i, j)[1] / b) * b;
                        dst.at<cv::Vec3b>(i, j)[2] = (src.at<cv::Vec3b>(i, j)[2] / b) * b;
                }
        }

        return 0;
};

int cartoon( cv::Mat &src, cv::Mat&dst, int levels, int magThreshold ){
        cv::Mat sobelX(src.rows, src.cols, CV_16SC3, cv::Scalar(255, 255, 255));
        cv::Mat sobelY(src.rows, src.cols, CV_16SC3, cv::Scalar(255, 255, 255));
        cv::Mat sobelMag(src.rows, src.cols, CV_16SC3, cv::Scalar(255, 255, 255));
        
        cv::Mat quantImg(src.rows, src.cols, CV_8UC3, cv::Scalar(255, 255, 255));
        blurQuantize(src, quantImg, levels);
        
        sobolX3x3(src, sobelX);
        sobolY3x3(src, sobelY);
        magnitude(sobelX, sobelY, sobelMag);

        for(int i=0; i<quantImg.rows; i++){
                for(int j=0; j<quantImg.cols; j++){
                        // std::cout<<sobelMag.at<cv::Vec3b>(i, j);
                        if(sobelMag.at<cv::Vec3b>(i, j) [0] > magThreshold ){
                               quantImg.at<cv::Vec3b>(i, j)[0] = 0;
                        }
                        if(sobelMag.at<cv::Vec3b>(i, j) [1] > magThreshold ){
                               quantImg.at<cv::Vec3b>(i, j)[1] = 0;
                        }
                        if(sobelMag.at<cv::Vec3b>(i, j) [2] > magThreshold ){
                               quantImg.at<cv::Vec3b>(i, j)[2] = 0;
                        }
                }
        }

        dst = quantImg;
        return 0;
};

int maxRGB(cv::Mat &src, cv::Mat&dst){
        
        for(int i=0; i<src.rows; i++){
                for(int j=0; j<src.cols; j++){

                        int max_val = std::max({src.at<cv::Vec3b>(i, j)[0], src.at<cv::Vec3b>(i, j)[1], src.at<cv::Vec3b>(i, j)[2]});
                        dst.at<cv::Vec3b>(i, j)[0] = src.at<cv::Vec3b>(i, j)[0];
                        dst.at<cv::Vec3b>(i, j)[1] = src.at<cv::Vec3b>(i, j)[1];
                        dst.at<cv::Vec3b>(i, j)[2] = src.at<cv::Vec3b>(i, j)[2];
                        if(src.at<cv::Vec3b>(i, j)[0] < max_val){
                                dst.at<cv::Vec3b>(i, j)[0] = 0;
                        }
                        if(src.at<cv::Vec3b>(i, j)[1] < max_val){
                                dst.at<cv::Vec3b>(i, j)[1] = 0;
                        }
                        if(src.at<cv::Vec3b>(i, j)[2] < max_val){
                                dst.at<cv::Vec3b>(i, j)[2] = 0;
                        }
                }
        }

        return 0;
};


int cornerFilter(cv::Mat &src, cv::Mat &dst){
    
    src.convertTo(src, CV_16SC3);

    cv::Mat imgx(src.rows, src.cols, CV_16SC3, cv::Scalar(255, 255, 255));
    int filter_1[3] = {1, -2, 1};
    int filter_2[3] = {1, -2, 1};

        for(int i=0; i<src.rows; i++){
                for(int j=0; j<src.cols; j++){
                        int iindices[3] = {i-1, i, i+1};
                        for(int p=0; p<3; p++){
                                if(iindices[p] < 0){
                                        iindices[p] = (src.rows) + iindices[p];
                                }else if(iindices[p] > (src.rows - 1)){
                                        iindices[p] = iindices[p] - src.rows; 
                                }
                        }

                        imgx.at<cv::Vec3s>(i, j)[0] = (src.at<cv::Vec3s>(iindices[0], j)[0] * filter_2[0] + src.at<cv::Vec3s>(iindices[1], j)[0] * filter_2[1] + src.at<cv::Vec3s>(iindices[2], j)[0] * filter_2[2]) / 4;

                        imgx.at<cv::Vec3s>(i, j)[1] = (src.at<cv::Vec3s>(iindices[0], j)[1] * filter_2[0] + src.at<cv::Vec3s>(iindices[1], j)[1] * filter_2[1] + src.at<cv::Vec3s>(iindices[2], j)[1] * filter_2[2]) / 4;
                        
                        imgx.at<cv::Vec3s>(i, j)[2] = (src.at<cv::Vec3s>(iindices[0], j)[2] * filter_2[0] + src.at<cv::Vec3s>(iindices[1], j)[2] * filter_2[1] + src.at<cv::Vec3s>(iindices[2], j)[2] * filter_2[2]) / 4;
                }
                
        }

        for(int i=0; i<src.rows; i++){
                for(int j=0; j<src.cols; j++){
                        int jindices[3] = {j-1, j, j+1};
                        for(int p=0; p<3; p++){
                                if(jindices[p] < 0){
                                        jindices[p] = (src.cols) + jindices[p];
                                }else if(jindices[p] > (src.cols - 1)){
                                        jindices[p] = jindices[p] - src.cols; 
                                }
                        }
                        
                        dst.at<cv::Vec3s>(i, j)[0] = (imgx.at<cv::Vec3s>(i, jindices[0])[0] * filter_1[0] + imgx.at<cv::Vec3s>(i, jindices[1])[0] * filter_1[1] + imgx.at<cv::Vec3s>(i, jindices[2])[0] * filter_1[2]) ;

                        dst.at<cv::Vec3s>(i, j)[1] = (imgx.at<cv::Vec3s>(i, jindices[0])[1] * filter_1[0] + imgx.at<cv::Vec3s>(i, jindices[1])[1] * filter_1[1] + imgx.at<cv::Vec3s>(i, jindices[2])[1] * filter_1[2]);

                        dst.at<cv::Vec3s>(i, j)[2] = (imgx.at<cv::Vec3s>(i, jindices[0])[2] * filter_1[0] + imgx.at<cv::Vec3s>(i, jindices[1])[2] * filter_1[1] + imgx.at<cv::Vec3s>(i, jindices[2])[2] * filter_1[2]);
                }       
        }

        return 0;
}

int contractBrightNessControl(cv::Mat &src, int alpha, int beta){

    for(int i=0; i<src.rows; i++){
            for(int j=0; j<src.cols; j++){
                    
                    // std::cout<<(int)x<<" "<<y<<" "<<z<<std::endl; 
                    src.at<cv::Vec3b>(i, j)[0] = std::min(alpha * src.at<cv::Vec3b>(i, j)[0] + beta, 255);
                    src.at<cv::Vec3b>(i, j)[1] = std::min(alpha * src.at<cv::Vec3b>(i, j)[1] + beta, 255);
                    src.at<cv::Vec3b>(i, j)[2] = std::min(alpha * src.at<cv::Vec3b>(i, j)[2] + beta, 255);
            }
    }

    return 0;
}