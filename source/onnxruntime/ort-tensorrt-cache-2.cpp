#include <onnxruntime_cxx_api.h>
#include <cuda_runtime.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <array>

void printImage(cv::Mat image) {
    for (int c=0; c<image.channels(); c++) {
       for (int i=0; i<image.rows; i++) {
            for (int j=0; j<image.cols; j++) {
                float pixel_value = image.at<cv::Vec3f>(i, j)[c];
                std::cout << pixel_value << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n\n";
    }
}

void displayImage(cv::Mat image) {
    cv::imshow("Preprocessed image", image);
    char key = (char)cv::waitKey();
    while(key != 'q') key = (char)cv::waitKey();
}

std::vector<float> process_image(const std::string& image_path, int& input_width, int& input_height, int &shortest_edge) 
{
    // Mean and standard deviation for normalization
    cv::Scalar mean(0.485, 0.456, 0.406);
    cv::Scalar std(0.229, 0.224, 0.225);
    float rescale_factor = 1.0f / 255.0f;

    // Load the image using OpenCV
    cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Error opening and loading image\n";
        return {};
    }

    // Convert the image to RGB (OpenCV loads in BGR by default)
    cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

    // Resize the image
    int new_width, new_height;
    if (image.cols < image.rows) {
        new_width = shortest_edge;
        new_height = static_cast<int>(input_width * static_cast<float>(image.rows) / image.cols);
    }
    else {
        new_height = input_height;
        new_width = static_cast<int>(input_height * static_cast<float>(image.cols) / image.rows);
    }

    cv::resize(image, image, cv::Size(new_width, new_height), 0, 0, cv::INTER_AREA);

    // Center crop the image to 518x518
    int start_x = (new_width - input_width) / 2;
    int start_y = (new_height - input_height) / 2;
    cv::Rect crop_region(start_x, start_y, input_width, input_height);
    image = image(crop_region);

    // Convert the image to float and rescale
    image.convertTo(image, CV_32F, rescale_factor);

    // Normalize the image
    cv::subtract(image, mean, image);
    cv::divide(image, std, image);

    // Convert image to a flattened vector in NCHW format (channel-first)
    std::vector<float> input_tensor_values;
    input_tensor_values.assign((float*)image.datastart, (float*)image.dataend);

    // Return the preprocessed image vector
    return input_tensor_values;
}


int main () 
{
    // constant
    const std::string model_path = "/home/pc/dev/opencv/models/dinov2/dinov2.onnx";
    const std::string image_path = "/home/pc/dev/dataset/samples/truck.jpg";
    int input_width = 518;
    int input_height = 518;
    int shortest_edge = 518;

    // Initialize the ONNX Runtime environment
    Ort::Env env = Ort::Env(ORT_LOGGING_LEVEL_WARNING, "ort-tensort");  
    Ort::SessionOptions session_options;

    OrtTensorRTProviderOptions options{};
    options.device_id = 0; // code the device id needed
    options.trt_max_workspace_size = 21474836480;
    options.trt_max_partition_iterations = 1000; 
    options.trt_min_subgraph_size = 1;
    options.trt_engine_cache_enable = 1;
    options.trt_engine_cache_path = "/home/pc/dev/opencv/models/dinov2";
    session_options.AppendExecutionProvider_TensorRT(options);
    Ort::Session session(env, model_path.c_str(), session_options);


    // Define batch size and input/output sizes
    int64_t batch_size = 1;
    size_t input_size = batch_size * 3 * 518 * 518;
    size_t output_size = batch_size * 768;

    // Define the input and output shapes
    std::array<int64_t, 4> input_shape = {batch_size, 3, 518, 518};
    std::vector<float> input_tensor_values = process_image(image_path, input_width, input_height, shortest_edge);
    if (input_tensor_values.empty()) {
        std::cerr << "Error loading input tensor values\n";
        return -1;
    }

    std::array<int64_t, 2> output_shape = {batch_size, 768};
    std::vector<float> output_tensor_values(output_size, 0.0f); // Initialize output tensor with zeros

    // Allocate tensor on CPU
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        memory_info, input_tensor_values.data(), input_tensor_values.size(),
        input_shape.data(), input_shape.size());

    Ort::Value output_tensor = Ort::Value::CreateTensor<float>(
        memory_info, output_tensor_values.data(), output_tensor_values.size(),
        output_shape.data(), output_shape.size());

    // Define input and output names
    const char* input_names[] = {"input"};
    const char* output_names[] = {"output"};

    // Run the model
    session.Run(Ort::RunOptions{nullptr}, input_names, &input_tensor, 1, output_names, &output_tensor, 1);

    // Print a success message
    std::cout << "Model inference completed successfully.\n";
    for (auto& x:output_tensor_values) 
        std::cout << x << ", ";

    return 0;
}