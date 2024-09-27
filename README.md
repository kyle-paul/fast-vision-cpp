# Computer Vision with OpenCV in C++
Welcome to the Computer Vision with OpenCV in C++ repository! This project is designed to provide a comprehensive introduction to computer vision techniques using the powerful OpenCV library in C++.

## Getting Started
To get started with this repository, you will need to have a basic understanding of C++ and an environment set up with OpenCV. You can find installation instructions and prerequisites in the Installation Guide below.

Feel free to explore the code, run the examples, and modify them to suit your needs. Contributions are welcome, so if you have improvements or additional projects to share, please consider submitting a pull request!

## Installation Guide

### OpenCV dependencies
```
# Install minimal prerequisites (Ubuntu 18.04 as reference)
sudo apt update && sudo apt install -y cmake g++ wget unzip
 
# Download and unpack sources
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.x.zip
unzip opencv.zip
 
# Create build directory
cd opencv-4.x && mkdir -p build && cd build
 
# Configure
cmake  ../opencv-4.x
 
# Build
cmake --build .
```

### Build with CMake file
```
cmake_minimum_required(VERSION 2.8)
project( application )

set(OpenCV_DIR /path/to/your/opencv-4.x/build)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( application tutorials/<the_file_you_want>.cpp )

target_link_libraries( application ${OpenCV_LIBS} )
```

All the *.cpp are stored in the tutorials directory. You must replace the executable files to the one that you would like to experiment.