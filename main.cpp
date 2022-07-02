//
// Created by TYY on 2022/4/14.
//
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <string>

#include "Driver.h"
#include "MilkBoxDetector.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    Driver driver;
    MilkBoxDetector detector;
    Mat frame;
    if(driver.InitCam() && driver.SetCam() && driver.StartGrab())
        printf("camera set successfully ! \n");
    else
        return 0;
    while(1){
        driver.Grab(frame);
        if (frame.empty()) {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        detector.DetectMilkBox(frame);
        imshow("capture",frame);
        waitKey(1);
    }
}

