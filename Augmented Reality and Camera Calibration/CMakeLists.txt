cmake_minimum_required(VERSION 3.0.0)
project( AugmentedReality )

set(OpenCV_DIR C:/opencv/build)
set(INCLUDE_DIR include)

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS} ${INCLUDE_DIR})

add_executable( AugmentedReality main.cpp)
target_link_libraries( AugmentedReality ${OpenCV_LIBS} )
