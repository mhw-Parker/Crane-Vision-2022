//
// Created by TYY on 2022/7/2.
//

#ifndef CRANE2022_MILKBOXDETECTOR_H
#define CRANE2022_MILKBOXDETECTOR_H

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <string>

#include "SelfDefine.h"

using namespace std;
using namespace cv;

class MilkBoxDetector {
public:
    MilkBoxDetector();
    ~MilkBoxDetector() = default;
    void DetectMilkBox(Mat &src);

    /** --   |  =  -|  ||  |-  none  **/
    /**  1   2  3   4   5  6    0    **/
    int pose;
    Mat tempImg;

private:
    void Preprocess(Mat &src);
    void GetPossibleBox();
    void TemplateMatch(Mat &src);
    void JudgePose();

    Mat binary, binary_inv;
    Mat source_image, gray;

    vector<RotatedRect> color_box;
    vector<RotatedRect> text_box;

    Point2f srcPoint[4], dstPoint[4];
    int model_cnt = 1, max_model_num;

private:
    void SetWarpPoint();

    float max_ratio, min_ratio;
    float min_box_area;
    float max_dx;
    float max_angle_error;
    float temHeight = 100;

};
class roiBox {
public:
    roiBox(RotatedRect rect, float r, float S) : rect(rect), ratio(r), area(S){}
    RotatedRect rect;
    float ratio, area;
};

#endif //CRANE2022_MILKBOXDETECTOR_H
