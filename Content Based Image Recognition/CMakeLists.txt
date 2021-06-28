cmake_minimum_required(VERSION 3.0.0)
project( CBIR )

set(OpenCV_DIR C:/opencv/build)
set(INCLUDE_DIR include)

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS} ${INCLUDE_DIR})

add_executable( CBIR main.cpp)
target_link_libraries( CBIR ${OpenCV_LIBS} )

