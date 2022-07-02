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
        return -1;
    } else {
        Mat src;
        capture.read(src);
        frameWidth = src.cols;
        frameHeight = src.rows;
        printf("image width: %d\t image height: %d\n",frameWidth,frameHeight);
    }
        return true;
}
bool Driver::SetCam()
{
    //capture.set(CAP_PROP_FRAME_WIDTH, 1280);			//设置相机采样宽度
    //capture.set(CAP_PROP_FRAME_HEIGHT, 720);		//设置相机采样高度
    //capture.set(CAP_PROP_AUTO_EXPOSURE,0.25);
    //capture.set(CAP_PROP_EXPOSURE,-13);
    return true;
}
bool Driver::Grab(Mat& src)
{
    capture.read(src);
    if(src.empty()) printf("grab failed !");
    return !src.empty();
}

bool Driver::StopGrab()
{
    return true;
}
