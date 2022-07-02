//
// Created by TYY on 2022/4/14.
//
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <string>

#include "Driver.h"
#include "MilkBoxDetector.h"

#ifndef SAVE_PHOTO
#define SAVE_PHOTO 1
#endif

using namespace std;
using namespace cv;

string FileLocation(string Location, int num, string EndLocationType)
{
    string imageFileName;
    std::stringstream StrStm1;
    StrStm1 << num + 1;
    StrStm1 >> imageFileName;
    imageFileName += EndLocationType;
    return Location + imageFileName;
}

int main(int argc, char** argv)
{
    Driver driver;
    MilkBoxDetector detector;
    string savePath = "../photos/";
    int start_num = 0;
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
#if SAVE_PHOTO == 1
        if(waitKey(15) == 32) {
            string path = FileLocation(savePath,start_num++,".jpg");
            imwrite(path,frame);
        }
#endif
        detector.DetectMilkBox(frame);
        imshow("capture",frame);
        waitKey(1);
    }
}

