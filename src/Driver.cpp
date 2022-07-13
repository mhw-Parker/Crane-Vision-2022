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
    for(int cam_id = 0; cam_id < 3; cam_id++) {
        capture.open(cam_id,cv::CAP_ANY);
        if (!capture.isOpened()) {
            std::cerr << "ERROR! Unable to open camera\n";
            continue;
        } else {
            Mat src;
            capture.read(src);
            frameWidth = src.cols;
            frameHeight = src.rows;
            printf("camera image size : %d x %d\n",frameWidth,frameHeight);
            return true;
        }
    }
    return false;
}
bool Driver::SetCam()
{
    capture.set(CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
    capture.set(CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
    printf("camera image resize to : %d x %d\n",FRAME_WIDTH,FRAME_HEIGHT);
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

