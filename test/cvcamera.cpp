#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <stdexcept>
#include <cassert>

struct test_error : public std::runtime_error
{ using std::runtime_error::runtime_error; };


void capture()
{
    cv::VideoCapture cap(0); // open the default camera
    if (!cap.isOpened())  // check if we succeeded
        throw test_error("fail to open camera");

    cv::Mat frame;
    cv::namedWindow("capture",1);
    while (cv::waitKey(30) < 0)
    {
        cap >> frame; // get a new frame from camera
        cv::imshow("capture", frame);
    }

}

void captureEdges()
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


void captureGrayFrame(cv::VideoCapture& capture, cv::Mat& frame)
{
    capture >> frame;
    cv::cvtColor(frame, frame, CV_RGB2GRAY);
}

void captureMotion()
{
    cv::VideoCapture capture(0);
    if (!capture.isOpened())
        throw test_error("failed to open camera");

    cv::Mat prev, current, next, d1, d2, motion;
    cv::namedWindow("diff", 1);
    captureGrayFrame(capture, prev);
    captureGrayFrame(capture, current);
    while (cv::waitKey(30) < 0)
    {
        // current capture
        captureGrayFrame(capture, next);

        // compute diff image
        cv::absdiff(prev, current, d1);
        cv::absdiff(current, next, d2);
        cv::bitwise_and(d1, d2, motion);
        cv::threshold(motion, motion, 35, 255, CV_THRESH_BINARY);

        // compute ndiff
        int ndiff = 0;
        for (int row = 0; row < motion.rows; ++row)
        {
            uchar* p = motion.ptr(row);
            ndiff += std::count_if(p, p + motion.cols, [](uchar a){ return a > 0; });
        }

        // display diff
        std::ostringstream text;
        text << "Diff number: ";
        text << ndiff;
        cv::putText(motion, text.str().c_str(), cv::Point(30,30),
            CV_FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(200,200,250), 1, CV_AA);
        cv::imshow("diff", motion);

        // swap frames
        std::swap(prev, next);
        std::swap(current, prev);
    }
}


int main(int, char*[])
{
    try
    {
        capture();
        captureEdges();
        captureMotion();
        return EXIT_SUCCESS;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
