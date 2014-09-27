#include "MotionDetector.hpp"

using namespace antifurto;

cv::Mat makeBlackImg(int size)
{
    return { cv::Mat::zeros(size, size, CV_8U) };
}

cv::Mat makeWhiteImg(int size)
{
    cv::Mat result = makeBlackImg(size);
    cv::bitwise_not(result, result);
    return result;
}

cv::Mat makeRandomImg(int size)
{
    cv::Mat result = makeBlackImg(size);
    uchar* p = result.ptr();
    for (int i = 0; i < result.cols * result.rows; ++i)
        *p++ = rand();
    return result;
}

void testNoDiff()
{
    MotionDetector detector;
    Picture b{makeBlackImg(1920)};
    for (int i = 0; i < 1000; ++i) {
        detector.examinePicture(b);
        detector.examinePicture(b);
    }
}

void testDiff()
{
    MotionDetector detector;
    Picture r1{makeRandomImg(1920)};
    Picture r2{makeRandomImg(1920)};
    for (int i = 0; i < 1000; ++i) {
        detector.examinePicture(r1);
        detector.examinePicture(r2);
    }
}

int main()
{
    testNoDiff();
    testDiff();
    return 0;
}
