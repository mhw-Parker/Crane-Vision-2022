//
// Created by TYY on 2022/7/2.
//
#include "MilkBoxDetector.h"

cv::Mat src_mat, dst_mat, gray_;
int bin_thresh = 100, bin_maxval = 255;

void ThresholdMat(int, void *) {
    Mat dst;
    threshold(gray_, dst, bin_thresh, bin_maxval, THRESH_BINARY);
    imshow("threshold", dst);
    waitKey(1);
}

MilkBoxDetector::MilkBoxDetector() {
    max_ratio = 2;
    min_ratio = 0.5;
    min_box_area = 2000;

    dstPoint[1] = Point2f(0,0);
    dstPoint[2] = Point2f(1.37*temHeight,0);
    dstPoint[3] = Point2f(1.37*temHeight,temHeight);
    dstPoint[0] = Point2f (0,temHeight);
}

void MilkBoxDetector::DetectMilkBox(Mat &src) {
    source_image = src.clone();
    Preprocess(src);
    GetPossibleBox();
    TemplateMatch(src);
    JudgePose();
    for(auto &i : text_box) {
        Point2f *pts_4 = new Point2f[4];
        i.points(pts_4);
        for(int j=0;j<4;j++){
            line(src,pts_4[j],pts_4[(j+1)%4],Scalar(255,145,0),2);
        }
    }
    for(auto &i : color_box) {
        Point2f *pts_4 = new Point2f[4];
        i.points(pts_4);
        for(int j=0;j<4;j++){
            line(src,pts_4[j],pts_4[(j+1)%4],Scalar(0,255,0),2);
        }
    }
    putText(src, "pose: ", Point(0, 30), cv::FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0),
            2,
            8, 0);
    putText(src, to_string(pose), Point(100, 30), cv::FONT_HERSHEY_PLAIN, 2,
            Scalar(0, 255, 0), 2, 8, 0);
}

void MilkBoxDetector::Preprocess(Mat &src) {
    Mat dst;
    source_image = src.clone();
    cvtColor(source_image, gray, COLOR_BGR2GRAY);
    threshold(gray,binary,50,255,THRESH_BINARY);
    threshold(gray,binary_inv,50,255,THRESH_BINARY_INV);
    Mat element = getStructuringElement(MORPH_RECT,Size(3,3));
    morphologyEx(binary_inv,binary_inv,MORPH_OPEN,element);
    imshow("gray",gray);
    imshow("binary",binary);
    imshow("binary inv",binary_inv);
}

void MilkBoxDetector::GetPossibleBox(){
    color_box.clear(); // empty the condition
    text_box.clear();
    vector<vector<Point>> inv_contours;
    findContours(binary_inv,inv_contours,RETR_EXTERNAL,CHAIN_APPROX_NONE);
    Mat background = Mat::zeros(Size(640,480),CV_8UC1);
    RotatedRect box;
    for(auto &i : inv_contours) {
        if(i.size() < 50) continue;
        box = minAreaRect(i);

        if(abs(box.center.x - 640/2)>100) continue; // only detect the milk box which locates in the middle
        //if(box.center.y < 150) continue;

        float ratio = box.size.width / box.size.height;
        if(ratio > max_ratio || ratio < min_ratio) continue; // a condition about the roi w/h ratio

        float box_area = box.size.width * box.size.height;
        if(box_area < min_box_area) continue; // a condition about roi area;

        if(box_area < 10000) {
            //printf("ratio : %f\t", ratio);
            if(box.center.y < 50) continue;
            text_box.push_back(box);
        }
        else {
            if(box.center.y < 150) continue;
            color_box.push_back(box); // store the possible which possibly include the text;
        }

        Point2f *pts_4 = new Point2f[4];
        box.points(pts_4);
        for(int j=0;j<4;j++){
            line(background,pts_4[j],pts_4[(j+1)%4],Scalar(255));
        }
    }
    imshow("contour", background);
}

void MilkBoxDetector::JudgePose() {
    Point2f s = Point2f(text_box.size(),color_box.size());
    int total_box = s.x + s.y;
    if(total_box > 3) {
        printf("--- ERROR DETECT !\n");
        return;
    } else if(total_box == 0) {
        pose = 0;
        return;
    }
    if(s.x == 1 && s.y == 0)
        pose = 1;
    else if(s.x == 0 && s.y == 1) {
        if(color_box.back().center.y > 280)
            pose = 2;
        else {
            float area = color_box.back().size.width * color_box.back().size.height;
            float ratio = color_box.back().size.width / color_box.back().size.height;
            float angle = color_box.back().angle;
            if(area < 60000)
                pose = 4;
            else
                pose = 5;
            //printf("%f\t%f\t%f\n",area,ratio,angle);
        }
    }
    else if(s.x == 2)
        pose = 3;
    else if(s.x == 1 && s.y == 1)
        pose = 6;

}

void MilkBoxDetector::TemplateMatch(Mat &src) {
    for(auto &i : text_box) {
        i.points(srcPoint);
        if(fabs(i.angle) < 45) {
            dstPoint[1] = Point2f(0,0);
            dstPoint[2] = Point2f(1.37*temHeight,0);
            dstPoint[3] = Point2f(1.37*temHeight,temHeight);
            dstPoint[0] = Point2f (0,temHeight);
        } else {
            dstPoint[2] = Point2f(0,0);
            dstPoint[3] = Point2f(1.37*temHeight,0);
            dstPoint[0] = Point2f(1.37*temHeight,temHeight);
            dstPoint[1] = Point2f (0,temHeight);
        }
        Mat warpPerspective_mat, warpPerspective_dst;
        warpPerspective_mat = getPerspectiveTransform(srcPoint, dstPoint); //对 svm 矩形区域进行透视变换
        warpPerspective(gray, warpPerspective_dst, warpPerspective_mat, Size(1.37*temHeight,temHeight), INTER_LINEAR, BORDER_CONSTANT, Scalar(0));
        imshow("warpPerspective",warpPerspective_dst);
        //printf("angle : %f\t",i.angle);
        //Mat roi = gray(i.boundingRect());
        //imshow("roi",roi);
        waitKey(0);
    }
}
void MilkBoxDetector::SetWarpPoint() {

}





