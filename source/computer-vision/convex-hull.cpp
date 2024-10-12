#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

static void help(char** argv) {
    std::cout << "\nThis sample program demonstrates the use of the convexHull() function\n";
}

int main(int argc, char** argv)
{
    cv::CommandLineParser parser(argc, argv, "{help h||}");
    if (parser.has("help"))
    {
        help(argv);
        return 0;
    }
    cv::Mat img(700, 700, CV_8UC3);
    cv::RNG& rng = cv::theRNG();

    for(;;)
    {
        int i, count = (unsigned)rng%100 + 1;
        std::cout << "count: " << count << "\n";
        std::vector<cv::Point> points;

        for(i = 0; i < count; i++)
        {
            cv::Point pt;
            pt.x = rng.uniform(img.cols/4, img.cols*3/4);
            pt.y = rng.uniform(img.rows/4, img.rows*3/4);
            points.push_back(pt);
        }

        std::vector<cv::Point> hull;
        cv::convexHull(points, hull, true);

        img = cv::Scalar::all(0);
        for(i = 0; i < count; i++) {
            cv::circle(img, points[i], 3, cv::Scalar(0, 0, 255), cv::FILLED, cv::LINE_AA);
        }

        cv::polylines(img, hull, true, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
        cv::imshow("hull", img);

        char key = (char)cv::waitKey();
        if( key == 27 || key == 'q' || key == 'Q' ) // 'ESC'
            break;
    }

    return 0;
}
