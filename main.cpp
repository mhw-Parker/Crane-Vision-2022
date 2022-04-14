//
// Created by TYY on 2022/4/14.
//
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    string file_name = "C:\\Users\\14107\\Pictures\\Pictures\\表情包\\1.png";
    Mat src = cv::imread(file_name);
    imshow("src",src);
    waitKey(0);
}

