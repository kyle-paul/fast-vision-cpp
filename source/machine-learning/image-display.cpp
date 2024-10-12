#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;

int main() {
    std::string image_path = samples::findFile("/home/pc/dev/opencv/dataset/att_faces/s1/1.pgm");
    Mat img = imread(image_path, IMREAD_COLOR);
    std::cout << img.size() << "\n";
 
    if(img.empty()) {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }
 
    imshow("Display window", img); 
    int k = waitKey(0);
 
    if(k == 's') {
        imwrite("starry_night.png", img);
    }
    return 0;
}