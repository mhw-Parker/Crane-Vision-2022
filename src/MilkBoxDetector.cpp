//
// Created by TYY on 2022/7/2.
//
#include "MilkBoxDetector.h"

MilkBoxDetector::MilkBoxDetector() {}

void MilkBoxDetector::DetectMilkBox(Mat &src) {
    Prepocess(src);
}

void MilkBoxDetector::Prepocess(Mat &src) {
    Mat dst;
    cv::cvtColor(src,gray,COLOR_BGR2GRAY);
}

