#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <vector>


int main(){
    
    // open video camera
    cv::VideoCapture camera;
    camera.open(0);
    if(!camera.isOpened()){
        std::cout << "Unable to open camera" << std::endl;
        return -1;
    }
    cv::namedWindow("Video", 1);
    cv::Mat full_frame;
    cv::Mat frame;
    cv::Mat save_frame;
    std::string mode = "d";
    int lcount = 0;
    int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE;
    bool found;
    
    // initialize point set and corner set for camera calibration and AR
    std::vector<cv::Point2f> corner_set;
    std::vector<cv::Vec3f> point_set;
    std::vector<std::vector<cv::Point2f>> corner_list;
    std::vector<std::vector<cv::Vec3f>> point_list;

    // initialize calibration parameters for camera calibration and AR
    cv::Mat cameraMatrix =  cv::Mat::eye(3, 3, CV_64FC1);
    cv::Mat rotationMatrix;
    cv::Mat translationMatrix;
    cv::Mat distortionMatrix =  cv::Mat::zeros(8, 1, CV_64F);
    cv::Mat stdDeviationsIntrinsics;
    cv::Mat stdDeviationsExtrinsics;
    cv::Mat perViewErrors;

    // initialize rotation matrix for the image points
    cv::Mat tfr =  cv::Mat::zeros(3, 3, CV_32FC1);
    tfr.at<float>(0, 1) = -1;
    tfr.at<float>(1, 0) = -1;
    tfr.at<float>(2, 2) = -1;

    // coordinates of points to draw three axes
    std::vector<cv::Vec3f> target_point_set;
    target_point_set.push_back(cv::Vec3f(0, 0, 0));
    target_point_set.push_back(cv::Vec3f(2, 0, 0));
    target_point_set.push_back(cv::Vec3f(0, 2, 0));
    target_point_set.push_back(cv::Vec3f(0, 0, 2));

    // coordinates of rotated points to draw three axes
    std::vector<cv::Vec3f> transformed_point_set;
    for(int i=0; i<target_point_set.size(); i++){
        cv::Mat mul = tfr * cv::Mat(target_point_set[i]);
        cv::Vec3f a((float*)mul.data);
        transformed_point_set.push_back( a );
    }

    // coordinates of points to draw tower
    std::vector<cv::Vec3f> object_point_set;
    object_point_set.push_back(cv::Vec3f(2, -2, 0));
    object_point_set.push_back(cv::Vec3f(2, -3, 0));
    object_point_set.push_back(cv::Vec3f(3, -3, 0));
    object_point_set.push_back(cv::Vec3f(3, -2, 0));
    

    object_point_set.push_back(cv::Vec3f(2, -2, 3));
    object_point_set.push_back(cv::Vec3f(2, -3, 3));
    object_point_set.push_back(cv::Vec3f(3, -3, 3));
    object_point_set.push_back(cv::Vec3f(3, -2, 3));

    object_point_set.push_back(cv::Vec3f(2.5, -2.5, 4));

    // coordinates of rotated points to draw tower
    std::vector<cv::Vec3f> transformed_object_point_set;
    for(int i=0; i<object_point_set.size(); i++){
        cv::Mat mul = tfr * cv::Mat(object_point_set[i]);
        cv::Vec3f a((float*)mul.data);
        transformed_object_point_set.push_back( a );
    }

    // flag to check if calib parameters are read
    bool readParams = false;

    for(;;) {
        camera >> full_frame;
        cv::resize(full_frame, frame, cv::Size(), 1, 1);
        if(mode == "d"){
            // default mode, no operations
            cv::imshow("Video", frame);
        }else if(mode == "a"){
            // draws tower or AR part

            // read camera parameters from the file
            if(!readParams){
                cv::FileStorage fs("camera_params.xml", cv::FileStorage::READ);
                fs["camera_matrix"] >> cameraMatrix;
                fs["distortion_matrix"] >> distortionMatrix;
                fs.release();
                readParams = true;
            }

            // find chessboard corners
            cv::Mat grayImg;
            cv::cvtColor(frame, grayImg, cv::COLOR_BGR2GRAY);
            found = cv::findChessboardCorners( grayImg, cv::Size2i(9, 6), corner_set, chessBoardFlags);
            if(found){
                // if corners are found, get the location of corners    
                cv::cornerSubPix( grayImg, corner_set, cv::Size(11 , 11),
                        cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.0001 ));
                point_set = {};
                float prev_y = NULL;
                int pointx = 0;
                int pointy = 0;

                // generate chessboard corners in 3D board frame
                for(int i=0; i<corner_set.size(); i++){
                    if(prev_y){
                        if( prev_y < corner_set[i].y ){
                            // next column
                            pointx++;
                            pointy = 0;
                        }
                    }
                    point_set.push_back( cv::Vec3f(pointx, pointy, 0) );
                    pointy--;
                    prev_y = corner_set[i].y;
                }

                std::vector<cv::Point2f> projectedPoints;
                std::vector<cv::Point2f> objprojectedPoints;
                
                // obtain rotation and translation matrix of the board
                cv::solvePnP(point_set, corner_set, cameraMatrix, distortionMatrix, rotationMatrix, translationMatrix);
                
                // project 3D axes and the tower points into the image plane
                cv::projectPoints(transformed_point_set, rotationMatrix, translationMatrix, cameraMatrix, distortionMatrix, projectedPoints);
                cv::projectPoints(transformed_object_point_set, rotationMatrix, translationMatrix, cameraMatrix, distortionMatrix, objprojectedPoints);
               
               // draw projected points of axes on the image
                cv::arrowedLine(frame, projectedPoints[0], projectedPoints[1], cv::Scalar(0, 0, 255), 2);
                cv::arrowedLine(frame, projectedPoints[0], projectedPoints[2], cv::Scalar(0, 255, 0), 2);
                cv::arrowedLine(frame, projectedPoints[0], projectedPoints[3], cv::Scalar(255, 0, 0), 2);

                // draw projected points of tower on the image
                cv::line(frame, objprojectedPoints[0], objprojectedPoints[1], cv::Scalar(0, 0, 255), 2);
                cv::line(frame, objprojectedPoints[1], objprojectedPoints[2], cv::Scalar(0, 0, 255), 2);
                cv::line(frame, objprojectedPoints[2], objprojectedPoints[3], cv::Scalar(0, 0, 255), 2);
                cv::line(frame, objprojectedPoints[3], objprojectedPoints[0], cv::Scalar(0, 0, 255), 2);

                cv::line(frame, objprojectedPoints[4], objprojectedPoints[5], cv::Scalar(255, 0, 0), 2);
                cv::line(frame, objprojectedPoints[5], objprojectedPoints[6], cv::Scalar(255, 0, 0), 2);
                cv::line(frame, objprojectedPoints[6], objprojectedPoints[7], cv::Scalar(255, 0, 0), 2);
                cv::line(frame, objprojectedPoints[7], objprojectedPoints[4], cv::Scalar(255, 0, 0), 2);

                cv::line(frame, objprojectedPoints[0], objprojectedPoints[4], cv::Scalar(0, 255, 0), 2);
                cv::line(frame, objprojectedPoints[1], objprojectedPoints[5], cv::Scalar(0, 255, 0), 2);
                cv::line(frame, objprojectedPoints[2], objprojectedPoints[6], cv::Scalar(0, 255, 0), 2);
                cv::line(frame, objprojectedPoints[3], objprojectedPoints[7], cv::Scalar(0, 255, 0), 2);

                cv::line(frame, objprojectedPoints[4], objprojectedPoints[8], cv::Scalar(255, 130, 255), 2);
                cv::line(frame, objprojectedPoints[5], objprojectedPoints[8], cv::Scalar(255, 130, 255), 2);
                cv::line(frame, objprojectedPoints[6], objprojectedPoints[8], cv::Scalar(255, 130, 255), 2);
                cv::line(frame, objprojectedPoints[7], objprojectedPoints[8], cv::Scalar(255, 130, 255), 2);

                
                std::cout<< "Rotation Matrix of the Board" << std::endl;
                for(int i=0; i<rotationMatrix.rows; i++){
                    for(int j=0; j<rotationMatrix.cols; j++){
                        std::cout <<  rotationMatrix.at<double>(i, j) << " ";
                    }
                    std::cout<<std::endl;
                }

                std::cout<< "Translation Matrix of the Board" << std::endl;
                for(int i=0; i<translationMatrix.rows; i++){
                    for(int j=0; j<translationMatrix.cols; j++){
                        std::cout <<  translationMatrix.at<double>(i, j) << " ";
                    }
                    std::cout<<std::endl;
                }

            }

            save_frame = frame;
            cv::imshow("Video", frame);

        }else if(mode == "c"){
            // calibration mode
            found = cv::findChessboardCorners( frame, cv::Size2i(9, 6), corner_set, chessBoardFlags);
            if(found){
                // chessboard pattern found
                
                cv::Mat grayImg;
                cv::cvtColor(frame, grayImg, cv::COLOR_BGR2GRAY);
                cv::cornerSubPix( grayImg, corner_set, cv::Size(11 , 11),
                        cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.0001 ));
                drawChessboardCorners( frame, cv::Size2i(9, 6), cv::Mat(corner_set), found );
                cv::imshow("Video", frame);

                std::cout<< "No. of corners found : " << corner_set.size() << std::endl;
                std::cout<< "Coordinates of the first corner : " << corner_set[0].x << "," << corner_set[0].y << std::endl;
                std::cout<< "===================================" << std::endl;

                if(corner_list.size() >= 1){
                    std::cout<< "Calibrating now" << std::endl;
                    
                    cameraMatrix.at<double>(0, 2) = frame.cols / 2;
                    cameraMatrix.at<double>(1, 2) = frame.rows / 2;

                    std::cout<< frame.cols / 2 << " " << frame.rows / 2 << std::endl;
                    float reprojection_error = cv::calibrateCamera(point_list, corner_list, cv::Size2i(9, 6), cameraMatrix, distortionMatrix, rotationMatrix, translationMatrix, stdDeviationsIntrinsics, stdDeviationsExtrinsics, perViewErrors, cv::CALIB_FIX_ASPECT_RATIO);
                    std::cout<< "Reprojection Error: " << reprojection_error << " " <<  point_list.size() << std::endl;
                    
                    // print camera matrix
                    std::cout<< "Camera Matrix" << std::endl;
                    for(int i=0; i<cameraMatrix.rows; i++){
                        for(int j=0; j<cameraMatrix.cols; j++){
                            std::cout <<  cameraMatrix.at<double>(i, j) << " ";
                        }
                        std::cout<<std::endl;
                    }
                    // distortion Matrix
                    std::cout<< "Distortion Matrix" << std::endl;
                    for(int i=0; i<distortionMatrix.rows; i++){
                        for(int j=0; j<distortionMatrix.cols; j++){
                            std::cout <<  distortionMatrix.at<double>(i, j) << " ";
                        }
                        std::cout<<std::endl;
                    }

                    // rotation Matrix
                    std::cout<< "Rotation Matrix" << std::endl;
                    for(int i=0; i<rotationMatrix.rows; i++){
                        for(int j=0; j<rotationMatrix.cols; j++){
                            std::cout <<  rotationMatrix.at<double>(i, j) << " ";
                        }
                        std::cout<<std::endl;
                    }

                    // translation Matrix
                    std::cout<< "Translation Matrix" << std::endl;
                    for(int i=0; i<translationMatrix.rows; i++){
                        for(int j=0; j<translationMatrix.cols; j++){
                            std::cout <<  translationMatrix.at<double>(i, j) << " ";
                        }
                        std::cout<<std::endl;
                    }

                }
            } 
        }
        save_frame = frame;
        char key = cv::waitKey(10);
        if( key == 'q') {
            // quit
            break;
        }else if( key == 's'){ 
            // save points to the point set for camera calibration
            // create point_set
            point_set = {};
            float prev_y = NULL;
            int pointx = 0;
            int pointy = 0;
            for(int i=0; i<corner_set.size(); i++){
                if(prev_y){
                    if( prev_y < corner_set[i].y ){
                        pointx++;
                        pointy = 0;
                    }
                }
                
                std::cout<< corner_set[i] << " " << cv::Vec3f(pointx, pointy, 0) << std::endl;
                point_set.push_back( cv::Vec3f(pointx, pointy, 0) );
                pointy--;
                prev_y = corner_set[i].y;
            }

            corner_list.push_back( corner_set );
            point_list.push_back( point_set );
            std::cout << corner_set.size() << std::endl;
            std::cout << point_set.size() << std::endl;
            // cv::imwrite("img_" + std::to_string(lcount) + ".jpg", save_frame);
            lcount++;
        }else if( key == 'p'){
            // save file parameters to a XML file
            cv::FileStorage fs("camera_params.xml", cv::FileStorage::WRITE);
            fs << "camera_matrix" << cameraMatrix;
            fs << "distortion_matrix" << distortionMatrix;
            fs.release();
            std::cout<< "Parameters Saved!" << std::endl;
        }else if(int(key) != -1){
            mode = key;
        }
    }
    return 0;
}