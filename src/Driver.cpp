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
    capture.open(camID_0,cv::CAP_ANY);
    if (!capture.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    } else
        return true;
}
bool Driver::SetCam()
{
    //    capture.set(CAP_PROP_FRAME_WIDTH, 1024);			//设置相机采样宽度
    //    capture.set(CAP_PROP_FRAME_HEIGHT, 820);		//设置相机采样高度
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

}
