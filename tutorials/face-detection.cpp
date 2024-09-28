#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
 
using namespace std;
using namespace cv;

static void help(const char** argv)
{
    cout << "\nThis program demonstrates the use of cv::CascadeClassifier class to detect objects (Face + eyes). You can use Haar or LBP features."
            "This classifier can recognize many kinds of rigid objects, once the appropriate classifier is trained."
            "It's most known use is for faces.\n\n"

        <<  "Argument parser:\n"
            "\t[--cascade=<cascade_path> this is the primary trained classifier such as frontal face]\n"
            "\t[--nested-cascade[=nested_cascade_path this an optional secondary classifier such as eyes]]\n"
            "\t[--scale=<image scale greater or equal to 1, try 1.3 for example>]\n"
            "\t[--try-flip]\n"
            "\t[filename|camera_index]\n\n"

        <<   "Example usage:\n"
             "   ./build/application\n"
            "\t--cascade=/path/to/your/haarcascade_frontalface_alt.xml\n"
            "\t--nested-cascade=/path/to/your/haarcascade_eye_tree_eyeglasses.xml\n"
            "\t--scale=1.3\n\n"
            
        <<  "During execution:\n\tHit any key to quit.\n"
            "\tUsing OpenCV version " << CV_VERSION << "\n" << "\n";
}
 
void detectAndDraw(cv::Mat& img, double scale, bool tryflip,
                   cv::CascadeClassifier& cascade,
                   cv::CascadeClassifier& nestedCascade);
 
string cascadeName;
string nestedCascadeName;
 
int main(int argc, const char** argv)
{
    cv::VideoCapture capture;
    cv::Mat frame, image;
    cv::CascadeClassifier cascade, nestedCascade;
    std::string inputName;
    bool tryflip;
    double scale;
 
    // argument parser
    cv::CommandLineParser parser(argc, argv,
        "{help h||}"
        "{cascade||}"
        "{nested-cascade||}"
        "{scale||}{try-flip||}{@filename||}");

    if (parser.has("help")) {
        help(argv);
        return 0;
    }

    // get parser
    cascadeName = parser.get<string>("cascade");
    nestedCascadeName = parser.get<string>("nested-cascade");
    scale = parser.get<double>("scale");
    if (scale < 1) scale = 1;
    tryflip = parser.has("try-flip");
    inputName = parser.get<string>("@filename");

    // check validation
    if (!parser.check()) {
        parser.printErrors();
        return 0;
    }
    if (!cascade.load(samples::findFile(cascadeName))) {
        std::cerr << "ERROR: Could not load classifier cascade\n";
        help(argv);
        return -1;
    }
    if (!nestedCascade.load(cv::samples::findFileOrKeep(nestedCascadeName))) {
        std::cerr << "WARNING: Could not load classifier cascade for nested objects\n";
    }
    
    // choose to turn on camera
    if(inputName.empty() || (isdigit(inputName[0]) && inputName.size() == 1) ) {
        int camera = inputName.empty() ? 0 : inputName[0] - '0';
        std:cout << "Using camera: " << camera << "\n";
        if(!capture.open(camera)) {
            std::cout << "Capture from camera #" <<  camera << " didn't work\n";
            return 1;
        }
    }

    // choose to input and load file
    else if (!inputName.empty()) {
        image = cv::imread(cv::samples::findFileOrKeep(inputName), cv::IMREAD_COLOR);
        if (image.empty()) {
            if (!capture.open(cv::samples::findFileOrKeep(inputName))) {
                std::cout << "Could not read " << inputName << "\n";
                return 1;
            }
        }
    }
 
    if(capture.isOpened()) {
        std::cout << "Video capturing has been started ...\n";
        for(;;) {
            capture >> frame;
            if(frame.empty()) break;
            cv::Mat frame1 = frame.clone();
            detectAndDraw(frame1, scale, tryflip, cascade, nestedCascade);
            char c = (char)waitKey(10);
            if(c == 27 || c == 'q' || c == 'Q') break;
        }
    }
    else {
        cout << "Detecting face(s) in " << inputName << endl;
        if(!image.empty()) {
            detectAndDraw(image, scale, tryflip, cascade, nestedCascade);
            waitKey(0);
        }
        else if(!inputName.empty()) {
            /* assume it is a text file containing the
            list of the image filenames to be processed - one per line */
            FILE* file = fopen( inputName.c_str(), "rt" );
            if(file) {
                char buf[1000+1];
                while(fgets(buf, 1000, file)) 
                {
                    int len = (int)strlen(buf);
                    while( len > 0 && isspace(buf[len-1]) )
                        len--;
                    buf[len] = '\0';
                    std::cout << "file " << buf << "\n";
                    image = cv::imread(buf, cv::IMREAD_COLOR);
                    if(!image.empty())
                    {
                        detectAndDraw(image, scale, tryflip, cascade, nestedCascade);
                        char c = (char)waitKey(0);
                        if( c == 27 || c == 'q' || c == 'Q' )
                            break;
                    }
                    else
                    {
                        std::cerr << "Error, couldn't read image " << buf << "\n";
                    }
                }
                fclose(file);
            }
        }
    }
    return 0;
}
 
void detectAndDraw(Mat& img, double scale, bool tryflip,
                   CascadeClassifier& cascade,
                   CascadeClassifier& nestedCascade)
{
    double t = 0;
    std::vector<cv::Rect> faces, faces2;
    const static Scalar colors[] =
    {
        Scalar(255,0,0),
        Scalar(255,128,0),
        Scalar(255,255,0),
        Scalar(0,255,0),
        Scalar(0,128,255),
        Scalar(0,255,255),
        Scalar(0,0,255),
        Scalar(255,0,255)
    };

    // preprocessing
    double fx = 1 / scale;
    cv::Mat gray, smallImg;
    cv::cvtColor(img, gray, COLOR_BGR2GRAY);
    cv::resize(gray, smallImg, Size(), fx, fx, INTER_LINEAR_EXACT);
    equalizeHist(smallImg, smallImg);
 
    // Cascaded prediction
    t = (double)getTickCount();
    cascade.detectMultiScale (
        smallImg, faces, 1.1, 2, 0
        |CASCADE_FIND_BIGGEST_OBJECT
        |CASCADE_DO_ROUGH_SEARCH
        |CASCADE_SCALE_IMAGE,
        Size(30, 30)
    );

    if(tryflip)
    {
        cv::flip(smallImg, smallImg, 1);
        cascade.detectMultiScale(
            smallImg, faces2, 1.1, 2, 0
            |CASCADE_FIND_BIGGEST_OBJECT
            |CASCADE_DO_ROUGH_SEARCH
            |CASCADE_SCALE_IMAGE,
            Size(30, 30)
        );

        for( std::vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); ++r ){
            faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
        }
    }

    // display the prediction time
    t = (double)getTickCount() - t;
    std::cout << "detection time = " <<  t*1000/getTickFrequency() << "ms\n";


    // display the bounding box
    for (size_t i = 0; i < faces.size(); i++ )
    {
        cv::Rect r = faces[i];
        cv::Mat smallImgROI;
        std::vector<cv::Rect> nestedObjects;
        cv::Point center;
        cv::Scalar color = colors[i%8];
 
        // draw rectangle boundbing box for cascaded objects
        cv::rectangle (
            img, Point(cvRound(r.x*scale), cvRound(r.y*scale)),
            Point(cvRound((r.x + r.width-1)*scale), cvRound((r.y + r.height-1)*scale)),
            color, 3, 8, 0
        );

        // Nested Cascaded prediction
        if(nestedCascade.empty())continue;
        smallImgROI = smallImg( r );
        nestedCascade.detectMultiScale ( 
            smallImgROI, nestedObjects, 1.1, 2, 0
            |CASCADE_FIND_BIGGEST_OBJECT
            |CASCADE_DO_ROUGH_SEARCH
            |CASCADE_DO_CANNY_PRUNING
            |CASCADE_SCALE_IMAGE,
            Size(30, 30) 
        );

        // draw rectangle boundbing box for nested cascaded objects
        for ( size_t j = 0; j < nestedObjects.size(); j++ ) {
            cv::Rect nr = nestedObjects[j];
            cv::rectangle (
                img, Point(cvRound((r.x + nr.x)*scale), cvRound((r.y + nr.y)*scale)),
                Point(cvRound((r.x + nr.x + nr.width-1)*scale), cvRound((r.y + nr.y + nr.height-1)*scale)),
                color, 3, 8, 0
            );
        }
    }
    cv::imshow( "Cascaded face detection", img );
}


/*
Example usage:
    ./build/application \
        --cascade=/home/pc/libs/opencv-4.10.0/data/haarcascades/haarcascade_frontalface_alt.xml \
        --nested-cascade=/home/pc/libs/opencv-4.10.0/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml \
        --scale=1.3
*/