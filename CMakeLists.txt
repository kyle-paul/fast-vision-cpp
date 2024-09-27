cmake_minimum_required(VERSION 2.8)
project( application )

set(OpenCV_DIR /home/pc/libs/opencv-4.10.0/build)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( application tutorials/pca.cpp )

target_link_libraries( application ${OpenCV_LIBS} )