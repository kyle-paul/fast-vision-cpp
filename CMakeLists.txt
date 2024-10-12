cmake_minimum_required(VERSION 3.10)
project(application LANGUAGES CXX)

enable_language(CXX CUDA)

set(OpenCV_DIR /home/pc/libs/opencv-4.10.0/build)
set(OpenVINO_DIR /opt/intel/openvino_2024.4.0/runtime/cmake)
set(ONNXRUNTIME_DIR /home/pc/libs/onnxruntime-linux-x64-gpu-1.19.2)
set(ONNXRUNTIME_LIBS ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so)
set(ONNXRUNTIME_OPENVINO ${ONNXRUNTIME_DIR}/lib/libonnxruntime_providers_openvino.so)

# Find OpenCV
find_package(OpenCV REQUIRED)
find_package(OpenVINO REQUIRED)
find_package(CUDAToolkit)

# Manually specify ONNX Runtime include and library paths
include_directories(${ONNXRUNTIME_DIR}/include ${OPENVINO_DIR}/include)
link_directories(${ONNXRUNTIME_DIR}/lib ${OPENVINO_DIR}/lib ${OPENVINO_LIBS})

# Add your executable
add_executable(application tutorials/onnxruntime/ort-openvino.cpp)

# Link OpenCV and ONNX Runtime libraries
target_link_libraries(application 
    ${OpenCV_LIBS} 
    ${ONNXRUNTIME_LIBS} 
    ${ONNXRUNTIME_CUDA}
    ${ONNXRUNTIME_TENSORRT}
    ${ONNXRUNTIME_OPENVINO}
)