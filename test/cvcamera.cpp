#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <stdexcept>
#include <cassert>

struct test_error : public std::runtime_error
{ using std::runtime_error::runtime_error; };


void videoCaptureTest()
{
    cv::VideoCapture cap(0); // open the default camera
    if (!cap.isOpened())  // check if we succeeded
        throw test_error("fail to open camera");

    cv::Mat edges, frame;
    cv::namedWindow("edges",1);
    for (;;)
    {
        cap >> frame; // get a new frame from camera
        cv::cvtColor(frame, edges, CV_BGR2GRAY);
        cv::GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
        cv::Canny(edges, edges, 0, 30, 3);
        cv::imshow("edges", edges);
        if (cv::waitKey(30) >= 0) break;
    }
}


int main(int, char*[])
{
    try
    {
        videoCaptureTest();
        return EXIT_SUCCESS;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
