//
// Created by TYY on 2022/7/2.
//

#ifndef CRANE2022_DRIVER_H
#define CRANE2022_DRIVER_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;

class Driver
{
private:
    VideoCapture capture;
    int camID_0 = 0, camID_1 = 1;
    int frameWidth, frameHeight;
public:
    bool InitCam() ;
    bool StartGrab() ;
    bool SetCam() ;
    bool Grab(Mat& src) ;
    bool StopGrab() ;
};
#endif //CRANE2022_DRIVER_H
