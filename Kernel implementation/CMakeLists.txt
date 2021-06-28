cmake_minimum_required(VERSION 3.0.0)
project( CS5330 )

set(OpenCV_DIR C:/opencv/build)

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable( CS5330 vidDisplay.cpp filters.cpp)
target_link_libraries( CS5330 ${OpenCV_LIBS} )


# vidDisplay.cpp