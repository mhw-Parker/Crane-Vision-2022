//
// Created by TYY on 2022/7/2.
//

#ifndef CRANE2022_MILKBOXDETECTOR_H
#define CRANE2022_MILKBOXDETECTOR_H

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

class MilkBoxDetector {
public:
    MilkBoxDetector();
    ~MilkBoxDetector() = default;
    void DetectMilkBox(Mat &src);

private:
    void Prepocess(Mat &src);
    Mat gray;
};

#endif //CRANE2022_MILKBOXDETECTOR_H
