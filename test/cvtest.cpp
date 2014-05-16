#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cassert>


bool captureTest()
{
    CvCapture* camera = cvCaptureFromCAM(CV_CAP_ANY);
    assert(camera);
    return true;
}

bool videoCaptureTest()
{
    cv::VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    cv::Mat edges;
    cv::namedWindow("edges",1);
    for(;;)
    {
        cv::Mat frame;
        cap >> frame; // get a new frame from camera
        cv::cvtColor(frame, edges, CV_BGR2GRAY);
        cv::GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
        cv::Canny(edges, edges, 0, 30, 3);
        cv::imshow("edges", edges);
        if (cv::waitKey(30) >= 0) break;
    }
    return 0;
}


int main()
{
    captureTest();
    videoCaptureTest();
    return 0;
}
