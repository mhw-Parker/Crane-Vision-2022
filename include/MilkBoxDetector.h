//
// Created by TYY on 2022/7/2.
//

#ifndef CRANE2022_MILKBOXDETECTOR_H
#define CRANE2022_MILKBOXDETECTOR_H

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <string>

#include "SelfDefine.h"

using namespace std;
using namespace cv;

const float INPUT_WIDTH = 640.0;
const float INPUT_HEIGHT = 640.0;
const float SCORE_THRESH = 0.2;
const float NMS_THRESH = 0.4;
const float CONF_THRESH = 0.7;

const vector<Scalar> colors = {Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255), 
                            Scalar(255,255,0), Scalar(0,255,255)};

/**
 * @brief the parameters about the contours
 * */
static struct param {
    float max_ratio, min_ratio;
    float max_area, min_area;
    int max_x, max_y, min_x, min_y;
}param;

/**
 * @brief create a new class to store redefine the height and width of the color blobs
 * */
class roiBox {
public:
    roiBox(RotatedRect rect, float S) : rect(rect), area(S){
        if(fabs(rect.angle) < 10) {
            h = rect.size.height;
            w = rect.size.width;
        } else {
            h = rect.size.width;
            w = rect.size.height;
        }
        h2w = h / w;
        rect.points(pts_4);
    }
    RotatedRect rect;
    Point2f pts_4[4];
    float area, h2w;
    float h, w;
};

class MilkBoxDetector {
public:
    MilkBoxDetector();
    ~MilkBoxDetector() = default;
    void DetectMilkBox(Mat &src);
    void init_yolov5(string &yolo_path);
    /** --   |  =  -|  ||  |-  none  **/
    /**  1   2  3   4   6  5    0    **/
    int pose;
    Mat tempImg;

private:
    void yolov5Detector(Mat &src);
    void modelDetectPose();
    Mat format_yolov5(Mat &src);
    void clearAll();
    
    dnn::Net model;
    string model_path;
    vector<string> outLayerNames;
    vector<Rect> big_face, small_face;

private:
    void Preprocess(Mat &src);
    void GetPossibleBox();
    void TemplateMatch(Mat &src);
    void JudgePose();

    Mat binary, binary_inv;
    Mat source_image, gray;

    vector<RotatedRect> color_box;
    vector<RotatedRect> forward_text_box;
    vector<roiBox> roi_text;
    vector<roiBox> roi_big_blob;

    Point2f srcPoint[4], dstPoint[4];
    int model_cnt = 1, max_model_num = 5;

private:
    float max_dx;
    float max_angle_error;
    float temHeight = 100;

    struct param textBox;
    struct param colorBox;
};




#endif //CRANE2022_MILKBOXDETECTOR_H
