# Computer Vision in C++ with OpenCV and accelerated performance backends.
Welcome to the Computer Vision with OpenCV in C++ repository! This project is designed to provide a comprehensive introduction to computer vision techniques using the powerful OpenCV library in C++. The folder `convert` is python code that convert a pytorch model (e.g dinov2) into `onnx` format for acceleated performance. In order to run the conver code in python, you need to use Docker or your own environment. For those want to use docker:

```bash
docker pull kylepaul/deeplearning:deployment`
```

More information of this docker image is at [here](`https://hub.docker.com/repository/docker/kylepaul/deeplearning/tags`). Then you would want to follow my `compose.yml` file for intitializing docker container and runnning with `docker compose up -d`

## Getting Started
To get started with this repository, you will need to have a basic understanding of C++ and an environment set up with OpenCV. You can find installation instructions and prerequisites in the Installation Guide below.

Feel free to explore the code, run the examples, and modify them to suit your needs. Contributions are welcome, so if you have improvements or additional projects to share, please consider submitting a pull request!

## Installation Guide

### OpenCV dependencies
```md
# Install minimal prerequisites (Ubuntu 18.04 as reference)
sudo apt update && sudo apt install -y cmake g++ wget unzip
 
# Download and unpack sources
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.10.0.zip
unzip opencv.zip
 
# Create build directory
cd opencv-4.10.0 && mkdir build 
```

To enable `cv::imshow` function to run without errors, you need to install `gtk` package and specify `WITH_GTK=ON` in the build command. Add the `--jobs=$(nproc --all)` to utilize all cpu cores for speeding up building source.
```md
# Install gtk package
sudo apt-get install libgtk2.0-dev libgtk-3-dev

# Configure
cmake  -B build -DCMAKE_BUILD_TYPE=Release -D WITH_GTK=ON
 
# Build
cmake --build --jobs=$(nproc --all)
```

### Build with CMake file
```cmake
cmake_minimum_required(VERSION 2.8)
project( application )

set(OpenCV_DIR /path/to/your/opencv-4.x/build)
find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( application tutorials/<the_file_you_want>.cpp )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )
```

All the *.cpp are stored in the tutorials directory. You must replace the executable files to the one that you would like to experiment. Then you build the project and run the executable file.
```
cmake -B build && cmake --build build && ./build/application
```

### Torch dependencies
You can follow the instruction of installing the prebuilt libtorch package at [pytoch document](https://pytorch.org/cppdocs/installing.html). Then you have to append the path to your `libtorch` package in `CMAKE_PREFIX_PATH` if you do not set it in the default system path.

```cmake
list(APPEND CMAKE_PREFIX_PATH /path/to/your/libtorch)
find_package(Torch REQUIRED)

target_link_libraries(${PROJECT_NAME} ${TORCH_LIBRARIES})
```

### OnnxRuntime dependencies
#### CUDA and TensorRT backends
Go to the onnxruntime [github releases](https://github.com/microsoft/onnxruntime/releases/tag/v1.19.2) and choose the prebuilt package that is compatible with your sysmte. For example, this project is built on ubuntu 24.04. Thus the package is `onnxruntime-linux-x64-gpu-1.19.2.tgz`. Then you need to set the path in cmake and include in the target libraries.
```cmake
set(ONNXRUNTIME_DIR /path/to/onnxruntime-linux-x64-gpu-1.19.2)
set(ONNXRUNTIME_LIBS ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so)

find_package(CUDAToolkit)

target_link_libraries(application 
    ${ONNXRUNTIME_LIBS} 
    ${ONNXRUNTIME_CUDA}
    ${ONNXRUNTIME_TENSORRT}
)
```

Because, there is no prebuilt package for `openvino` backend from onnxruntime releases. We need to build the onnxruntime from source `./build.sh` with option `--use_openvino <hardware_option>` `--build_shared_lib --build` . The full documentation can be found [here](https://onnxruntime.ai/docs/build/eps.html#openvino)
#### Openvino dependencies
```cmake
set(OpenVINO_DIR /path/to/o
    penvino_2024.4.0/runtime/cmake)
set(ONNXRUNTIME_OPENVINO ${ONNXRUNTIME_DIR}/lib/libonnxruntime_providers_openvino.so)

find_package(OpenVINO REQUIRED)
target_link_libraries(${PROJECT_NAME} ${ONNXRUNTIME_OPENVINO})
```

You can reference to the `CMakeLists.txt` in the repository.