cmake_minimum_required(VERSION 2.8)
project( application )

# Set OpenCV and ONNX Runtime directories
set(OpenCV_DIR /home/pc/libs/opencv-4.10.0/build)
set(ONNXRUNTIME_DIR /home/pc/libs/onnxruntime-linux-x64-gpu-1.19.2)
set(ONNXRUNTIME_LIBS ${ONNXRUNTIME_DIR}/lib/libonnxruntime.so)
set(ONNXRUNTIME_LIBS_CUDA ${ONNXRUNTIME_DIR}/lib/libonnxruntime_providers_cuda.so)
set(ONNXRUNTIME_LIBS_TENSORRT ${ONNXRUNTIME_DIR}/lib/libonnxruntime_providers_tensorrt.so)

# Find OpenCV
find_package( OpenCV REQUIRED )

# Manually specify ONNX Runtime include and library paths
include_directories(${ONNXRUNTIME_DIR}/include)
link_directories(${ONNXRUNTIME_DIR}/lib)

# Add your executable
add_executable( application tutorials/onnxruntime/ort-tensorrt.cpp )

# Link OpenCV and ONNX Runtime libraries
target_link_libraries( application 
	${OpenCV_LIBS} 
	${ONNXRUNTIME_LIBS} 
	${ONNXRUNTIME_LIBS_CUDA}
	${ONNXRUNTIME_LIBS_TENSORRT}
)
