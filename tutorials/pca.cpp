#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;
struct params
{
    cv::Mat data;
    int ch;
    int rows;
    cv::PCA pca;
    std::string winName;
};


static void readImageList(const std::vector<std::string>& paths, std::vector<cv::Mat>& images) {
	for (const auto &path : paths) {
		images.push_back(cv::imread(path, cv::IMREAD_GRAYSCALE));
	}
}

static  cv::Mat formatImagesForPCA(const std::vector<cv::Mat> &data) {
    cv::Mat dst(static_cast<int>(data.size()), data[0].rows*data[0].cols, CV_32F);
    for(unsigned int i = 0; i < data.size(); i++) {
        cv::Mat image_row = data[i].clone().reshape(1, 1);
        cv::Mat row_i = dst.row(i);
        image_row.convertTo(row_i, CV_32F);
    }
    return dst;
}

static cv::Mat toGrayscale(cv::InputArray _src) 
{
	cv::Mat src = _src.getMat();
    if(src.channels() != 1) {
        CV_Error(cv::Error::StsBadArg, "Only Matrices with one channel are supported");
    }
    // create and return normalized image
    cv::Mat dst;
    cv::normalize(_src, dst, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    return dst;
}


static void onTrackbar(int pos, void* ptr)
{
    std::cout << "Retained Variance = " << pos << "%   ";
    std::cout << "re-calculating PCA..." << std::flush;
 
    double var = pos / 100.0;
    struct params *p = (struct params *)ptr;
 
    p->pca = cv::PCA(p->data, cv::Mat(), cv::PCA::DATA_AS_ROW, var);
 
   	cv::Mat point = p->pca.project(p->data.row(0));
    cv::Mat reconstruction = p->pca.backProject(point);
    reconstruction = reconstruction.reshape(p->ch, p->rows);
    reconstruction = toGrayscale(reconstruction);
 
    imshow(p->winName, reconstruction);
    std::cout << "done!   # of principal components: " << p->pca.eigenvectors.rows << "\n";
}

 
int main(int argc, char** argv) {

	// argument parser
	cv::CommandLineParser parser(argc, argv, 
		"{@input||image list}"
        "{help h||show help message}");

	if (parser.has("help")) {
		parser.printMessage();
		exit(0);
	}

	// dataset directory
	std::string dataset = parser.get<std::string>("@input");
	if (dataset.empty()) {
		parser.printMessage();
		exit(1);
	}

	// load all the paths
	std::vector<std::string> paths;
	for (const auto& entry : fs::recursive_directory_iterator(dataset)) {
		if (entry.path().extension() == ".pgm") {
			paths.push_back(entry.path().string());
		}
	}
	std::cout << "There are " << paths.size() << " images in your dataset\n";

	// vector to hold the images
    std::vector<cv::Mat> images;

	// read in the data. This can fail if not valid
	try {
		readImageList(paths, images);
	} catch(const cv::Exception& e) {
		std::cerr << "Error opening file \"" << dataset << "\". Reason: " << e.msg << "\n";
		exit(1);		
	}

	// quit if there are not enough images for this demo.
	if(images.size() <= 1) {
        std::string error_message = "This demo needs at least 2 images to work.";
        CV_Error(cv::Error::StsError, error_message);
    }
 
   	// reshape and stack images into a rowMatrix
    cv::Mat data = formatImagesForPCA(images);
    std::cout << "Data: " << data.size() << "\n";

    // perform PCA
    cv::PCA pca(data, cv::Mat(), cv::PCA::DATA_AS_ROW, 0.95);

    // Print the eigenvalues and eigenvectors
    std::cout << "Eigenvalues shape: " << pca.eigenvalues.size() << "\n";
    std::cout << "Eigenvectors shape: " << pca.eigenvectors.size() << "\n";


    // demostrate the effect of retainedVariance on the first image
    cv::Mat point = pca.project(data.row(0)); // project into the eigenspace, thus the image becomes a "point"
    cv::Mat reconstruction = pca.backProject(point); // re-create the image from the "point"
    reconstruction = reconstruction.reshape(images[0].channels(), images[0].rows); // reshape from a row vector into image shape
    reconstruction = toGrayscale(reconstruction); // re-scale for displaying purposes
    std::cout << "Reconstruction: " << reconstruction.size() << "\n";

     // init highgui window
    std::string winName = "Reconstruction | press 'q' to quit";
    namedWindow(winName, cv::WINDOW_NORMAL);
 
    // params struct to pass to the trackbar handler
    params p;
    p.data = data;
    p.ch = images[0].channels();
    p.rows = images[0].rows;
    p.pca = pca;
    p.winName = winName;
 
    // create the tracbar
    int pos = 95;
    cv::createTrackbar("Retained Variance (%)", winName, &pos, 100, onTrackbar, (void*)&p);
 
    // display until user presses q
    imshow(winName, reconstruction);
 
    char key = 0;
    while(key != 'q') key = (char)cv::waitKey();
    return 0;
}

/*
example usage with att_faces dataset:
 ./build/application --input /home/pc/dev/opencv/dataset/att_faces

get dataset from:
http://www.cl.cam.ac.uk/research/dtg/attarchive/facedatabase.html
*/