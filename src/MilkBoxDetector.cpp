//
// Created by TYY on 2022/7/2.
//
#include "MilkBoxDetector.h"


MilkBoxDetector::MilkBoxDetector() {
    max_ratio = 2;          // the rect max ratio
    min_ratio = 0.5;        // the rect min ratio
    min_box_area = 2000;    // the min area of the box
    max_dx = 50;            // the center of the blob max delta x pixel
    max_text_box_area = 10000;  //
    min_color_box_area = 16000;

    max_angle_error = 10; // the max error angle of the deep color blob
    max_model_num = 3;      // the list of the model match
}

void MilkBoxDetector::DetectMilkBox(Mat &src) {
    source_image = src.clone();
    Preprocess(src);
    GetPossibleBox();
    if(text_box.size()) {
        double s = getTickCount();
        TemplateMatch(src);
        //cout << "match time : " << (getTickCount() - s) * 1000 / getTickFrequency() << endl;
    }
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
    //threshold(gray,binary,50,255,THRESH_BINARY);
    threshold(gray,binary_inv,50,255,THRESH_BINARY_INV);
    Mat element = getStructuringElement(MORPH_RECT,Size(3,3));
    morphologyEx(binary_inv,binary_inv,MORPH_OPEN,element);
    imshow("gray",gray);
    //imshow("binary",binary);
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

        if(abs(box.center.x - 640/2) > max_dx) continue; // only detect the milk box which locates in the middle
        //if(box.center.y < 150) continue;

        float ratio = (box.size.width > box.size.height) ? box.size.width / box.size.height : box.size.height / box.size.width;
        if(ratio > max_ratio || ratio < min_ratio) continue; // a condition about the roi w/h ratio

        float box_area = box.size.width * box.size.height;
        if(box_area < min_box_area) continue; // a condition about roi area;

        float angle = fabs(box.angle);
        if(angle - 90.0 > max_angle_error && angle > max_angle_error) continue;

        if(box_area < max_text_box_area) { // the text box
            //printf("ratio : %f\t", ratio);
            if(box.center.y < 50) continue;
            text_box.push_back(box);
        }
        else {
            if(box.center.y < 150) continue;
            if(box_area < min_color_box_area) continue;
            //  cout << "area : " << box_area << endl;
            color_box.push_back(box); // store the possible which possibly include the text;
        }

        Point2f *pts_4 = new Point2f[4];
        box.points(pts_4);
        for(int j=0;j<4;j++){
            line(background,pts_4[j],pts_4[(j+1)%4],Scalar(255));
        }
    }
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
                pose = 6;
            //printf("%f\t%f\t%f\n",area,ratio,angle);
        }
    }
    else if(s.x == 2)
        pose = 3;
    else if(s.x == 1 && s.y == 1)
        pose = 5;

}

void MilkBoxDetector::TemplateMatch(Mat &src) {
    for(auto &i : text_box) {
        i.points(srcPoint);
        Point p;
        if(fabs(i.angle) < 45) {
            dstPoint[1] = Point2f(0,0);
            dstPoint[2] = Point2f(1.37*temHeight,0);
            dstPoint[3] = Point2f(1.37*temHeight,temHeight);
            dstPoint[0] = Point2f (0,temHeight);
            p = srcPoint[1];
        } else {
            dstPoint[2] = Point2f(0,0);
            dstPoint[3] = Point2f(1.37*temHeight,0);
            dstPoint[0] = Point2f(1.37*temHeight,temHeight);
            dstPoint[1] = Point2f (0,temHeight);
            p = srcPoint[2];
        }
        circle(src,p,2, Scalar(0, 0, 255), 2);
        Mat warpPerspective_mat, warpPerspective_dst, text_roi;
        warpPerspective_mat = getPerspectiveTransform(srcPoint, dstPoint);
        warpPerspective(gray, warpPerspective_dst, warpPerspective_mat, Size(1.37*temHeight,temHeight), INTER_LINEAR, BORDER_CONSTANT, Scalar(0));
        warpPerspective(source_image,text_roi,warpPerspective_mat, Size(1.37*temHeight,temHeight), INTER_LINEAR, BORDER_CONSTANT, Scalar(0));
        resize(warpPerspective_dst,warpPerspective_dst,Size(1.37*temHeight,temHeight));
        //imshow("warpPerspective",warpPerspective_dst);
        tempImg = warpPerspective_dst.colRange(28,108); // the text size is 80 x 36 pixel
        tempImg = tempImg.rowRange(18,54);

        for(model_cnt = 1; model_cnt < max_model_num; model_cnt++){
            string temp_path = string("../TemplateModel/" + to_string(model_cnt)).append(".jpg");
            Mat model = imread(temp_path, CV_8UC1);
            //imshow("template",model);
            if(model.empty()) {
                std::cerr << "Wrong template path !\n";
                break;
            } else {
                int result_width = warpPerspective_dst.cols - model.cols + 1;
                int result_height = warpPerspective_dst.rows - model.rows + 1;
                //printf("w-h : %f\t%f\n",result_width,result_height);
                Mat result_img = Mat(Size(result_width,result_height),CV_32FC1);
                matchTemplate(warpPerspective_dst,model,result_img,TM_SQDIFF_NORMED);
                normalize(result_img,result_img,0,1,NORM_MINMAX,-1);
                double minVal = -1, maxVal;
                Point minLoc, maxLoc, matchLoc;
                minMaxLoc(result_img,&minVal,&maxVal,&minLoc,&maxLoc,Mat());
                matchLoc = minLoc;
                // draw out the most possible text box
                Point center = Point(minLoc.x + model.cols / 2, minLoc.y + model.rows / 2);
                rectangle(text_roi, matchLoc, Point(matchLoc.x + model.cols, matchLoc.y + model.rows), Scalar(255, 2, 250), 2, 8, 0);
                circle(text_roi, center, 2, Scalar(0, 255, 0), 2);
                imshow("match result", text_roi);
                if(minVal < 1e-07) break;
            }
        }
        if(model_cnt > max_model_num) { // which means template match does not success, delete this rotateRect
            swap(i,text_box[text_box.size()-1]);
            text_box.pop_back();
        }
    }
}






