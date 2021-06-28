# Content Based Image Retrieval 

To run the project, following cmake configuration can be used.
========================================================

cmake_minimum_required(VERSION 3.0.0)
project( CBIR )

set(OpenCV_DIR C:/opencv/build)
set(INCLUDE_DIR include)

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS} ${INCLUDE_DIR})

add_executable( CBIR main.cpp)
target_link_libraries( CBIR ${OpenCV_LIBS} )

========================================================

Inside the main function
> question variable can be used to pass the question number.
> taskMode variable can be used to pass the task mode. 
		> Mode 0 : To search the target image
		> Mode 1 : To generate image features for all the images inside the folder