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

string savePath = "../photos/";

bool debug = false;

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
    int start_num = 0; //the start number of the first photo
    Mat frame;
    if(!debug) {
        if(driver.StartGrab())
            printf("camera set successfully ! \n");
        else
            return 0;
    }
    while(1){
        if(debug) {
            string src_path = FileLocation(savePath,++start_num,".jpg");
            driver.Grab(frame,src_path);
        } else
            driver.Grab(frame);
        if (frame.empty()) {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }
#if SAVE_PHOTO == 1
        if(waitKey(15) == 32) {
            string path = FileLocation(savePath,start_num++,".jpg");
            imwrite(path,frame);
            printf("take a shot !\n");
        }
#endif
        double s = getTickCount();
        detector.DetectMilkBox(frame);
        //cout << "latency : " << (getTickCount() - s) * 1000 / getTickFrequency() << endl;
#if SAVE_TEMPLATE == 1
        if(!detector.tempImg.empty()) {
            string path = FileLocation("../TemplateModel/",++start_num,".jpg");
            imwrite(path,detector.tempImg);
        }
#endif
        imshow("result", frame);
        if(debug)
            waitKey(0);
        else
            waitKey(1);
    }
}

