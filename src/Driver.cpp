//
// Created by TYY on 2022/7/2.
//
#include "Driver.h"

bool Driver::InitCam()
{
    return true;
}
bool Driver::StartGrab()
{
    capture.open(camID_1,cv::CAP_ANY);
    if (!capture.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return 0;
    } else {
        Mat src;
        capture.read(src);
        frameWidth = src.cols;
        frameHeight = src.rows;
        printf("camera image size : %d x %d\n",frameWidth,frameHeight);
        return true;
    }
}
bool Driver::SetCam()
{
    return true;
}
bool Driver::Grab(Mat& src)
{
    capture.read(src);
    if(src.empty()) printf("grab failed !");
    return !src.empty();
}
bool Driver::Grab(Mat &src, std::string photo_path) {
    capture.open(photo_path);
    if (!capture.isOpened()) {
        std::cerr << "ERROR! Unable to open photo, maybe wrong path\n";
        exit(0);
    } else {
        capture.read(src);
        return true;
    }
}

