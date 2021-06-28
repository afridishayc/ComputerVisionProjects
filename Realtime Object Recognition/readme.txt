

# Readme


Following cmake configuration is used to build the project


========================================

cmake_minimum_required(VERSION 3.0.0)
project( 2drecog )

set(OpenCV_DIR C:/opencv/build)
set(INCLUDE_DIR include)

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS} ${INCLUDE_DIR})

add_executable( 2drecog main.cpp recog.cpp)
target_link_libraries( 2drecog ${OpenCV_LIBS} )

=========================================

Please put the objects.csv inside the build/Debug folder to enable system to read pregenerated features


URL of the video demonstration: https://drive.google.com/file/d/14PVykTP18kS5alWa6R_VaBnphGa5nH1E/view

If there is an issue watching the video, please download the video from the above link. 


