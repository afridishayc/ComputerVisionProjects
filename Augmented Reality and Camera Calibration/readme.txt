

# readme

*****************************************************************
# For Camera Calibration and Augmented Reality

File : main.cpp

Please put camera_params.xml inside debug folder.

Following CMake setup is used

==============================================
cmake_minimum_required(VERSION 3.0.0)
project( AugmentedReality )

set(OpenCV_DIR C:/opencv/build)
set(INCLUDE_DIR include)

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS} ${INCLUDE_DIR})

add_executable( AugmentedReality main.cpp)
target_link_libraries( AugmentedReality ${OpenCV_LIBS} )
======================================================

Keys - Functions

d = Default
a = AR, place an object in the scene
c = calibrate camera
q = quit
s = save an instance of image frame from video camera to perform calibration
p = save camera paramters to the file


*****************************************************************


File : mainh.cpp 

# For Harris Corner Detection Algorithm

Following CMake setup is used

====================================
cmake_minimum_required(VERSION 3.0.0)
project( harris )

set(OpenCV_DIR C:/opencv/build)
set(INCLUDE_DIR include)

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS} ${INCLUDE_DIR})

add_executable( harris mainh.cpp)
target_link_libraries( harris ${OpenCV_LIBS} )
============================================


Keys - Function

d = default
h = harris corners
q = quit

*****************************************************************
