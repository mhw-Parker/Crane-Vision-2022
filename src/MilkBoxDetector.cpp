//
// Created by TYY on 2022/7/2.
//
#include "MilkBoxDetector.h"


MilkBoxDetector::MilkBoxDetector() {
    /** text box parameter **/
    textBox.max_ratio = 1;  // the rect max ratio
    textBox.min_ratio = 0.5;    // the rect min ratio
    textBox.max_area = 15000;
    textBox.min_area = 2000;    // the min area of the box

    /** color box area **/
    colorBox.min_area = 15000;
    colorBox.max_ratio = 3.5;
    colorBox.min_ratio = 0.5;
    colorBox.min_y = 50;

    max_dx = 100;            // the center of the blob max delta x pixel
    max_angle_error = 12; // the max error angle of the deep color blob
    max_model_num = 3;      // the list of the model match
}
/**
 * @brief the top task of the detection
 * */
void MilkBoxDetector::DetectMilkBox(Mat &src) {
    source_image = src.clone();
    Preprocess(src);
    GetPossibleBox();
    if(forward_text_box.size()) {
        double s = getTickCount();
        TemplateMatch(src);
        //cout << "match time : " << (getTickCount() - s) * 1000 / getTickFrequency() << endl;
    }
    JudgePose();
    for(auto &i : forward_text_box) {
        Point2f *pts_4 = new Point2f[4];
        i.points(pts_4);
        for(int j=0;j<4;j++){
            line(src,pts_4[j],pts_4[(j+1)%4],Scalar(255,145,0),2);
        }
    }
    for(auto &i : roi_big_blob) {
        for(int j=0;j<4;j++){
            line(src,i.pts_4[j],i.pts_4[(j+1)%4],Scalar(0,255,0),2);
        }
    }
    putText(src, "pose: ", Point(0, 30), cv::FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0),
            2,
            8, 0);
    putText(src, to_string(pose), Point(100, 30), cv::FONT_HERSHEY_PLAIN, 2,
            Scalar(0, 255, 0), 2, 8, 0);
}

void MilkBoxDetector::Preprocess(Mat &src) {
    Mat dst , cannyEdge;
    source_image = src.clone();
    cvtColor(source_image, gray, COLOR_BGR2GRAY);
    threshold(gray,binary,50,255,THRESH_BINARY);
    threshold(gray,binary_inv,60,255,THRESH_BINARY_INV);
    Mat element = getStructuringElement(MORPH_RECT,Size(3,3));
    morphologyEx(binary_inv,binary_inv,MORPH_OPEN,element);
    //dilate(binary,binary,element);
    //Canny(gray,cannyEdge,100,180);
    //imshow("canny",cannyEdge);
    //imshow("gray",gray);
    //imshow("binary",binary);
    imshow("binary inv",binary_inv);
}
/**
 * @brief use the deep color on the box's surface, filter the possible roi
 * */
void MilkBoxDetector::GetPossibleBox(){
    color_box.clear(); // empty the condition
    forward_text_box.clear();
    roi_big_blob.clear();
    vector<vector<Point>> inv_contours, contours;
    findContours(binary_inv,inv_contours,RETR_EXTERNAL,CHAIN_APPROX_NONE);
    Mat background = Mat::zeros(Size(FRAME_WIDTH,FRAME_WIDTH),CV_32FC1);
    RotatedRect box;
    for(auto &i : inv_contours) {
        if(i.size() < 50) continue;
        box = minAreaRect(i);

        // only detect the milk box which locates in the middle
        if(abs(box.center.x - FRAME_WIDTH / 2) > max_dx) continue;

        float angle = fabs(box.angle);
        if(fabs(angle - 90.0) > max_angle_error && angle > max_angle_error) continue;
        // a condition about the roi h/w ratio
        float ratio = (fabs(box.angle) < 10) ? box.size.height/box.size.width : box.size.width/box.size.height;
        float box_area = box.size.width * box.size.height;

        Point2f *pts_4 = new Point2f[4];
        box.points(pts_4);
        for(int j=0;j<4;j++){
            line(background,pts_4[j],pts_4[(j+1)%4],Scalar(255));
        }

        if(box_area > textBox.min_area && box_area < textBox.max_area) {
            if(ratio > textBox.max_ratio || ratio < textBox.min_ratio) continue;
            forward_text_box.push_back(box);
        }else if(box_area > colorBox.min_area) {
            if(ratio > colorBox.max_ratio || ratio < colorBox.min_ratio) continue;
            if(box.center.y < colorBox.min_y) continue;
            roiBox temp_color_box(box,box_area);
            roi_big_blob.push_back(temp_color_box);
        }else
            continue;

    }
    //imshow("back",background);
}
/**
 * @brief judge the pose of the milk boxes which totally include 7 conditions
 *  --   |   =   -|  |-  ||  none
 *  1    2   3   4   5   6   0
 * */
void MilkBoxDetector::JudgePose() {
    int text_num = forward_text_box.size(), color_num = roi_big_blob.size();
    int total_box = text_num + color_num;
    /// the number of the milk boxes in the camera vision can't be more than 2
    if(total_box > 3) {
        printf("--- ERROR DETECT !\n");
        return;
    } else if(total_box == 0) {
        pose = 0;
        return;
    }
    /// while the following steps only detected some color blobs but didn't include test box
    if(!text_num && color_num ) {
        if(color_num >= 2) {
            pose = 6;
        }
        else {
            int y_ = roi_big_blob.back().rect.center.y;
            float area = roi_big_blob.back().area;
            if(roi_big_blob.back().h2w > 1.4) {
                pose = 6;
            }
            else {
                if(y_ > FRAME_HEIGHT/2 + 50)
                    pose = 2;
                else
                    pose = 4;
            }
            printf("y: %d\tarea: %f\tratio: \n",y_,area,roi_big_blob.back().h2w);
        }
    }
    else if( text_num && !color_num) {
        pose = (text_num > 1) ? 3 : 1;
    }
    else {
        pose = 5;
    }

}
/**
 * @brief the forward surface of the milk box contain the text "特仑苏", so I suppose to use a
 * template to match the words in order to filter the roi which may be wrongly detecting
 * */
void MilkBoxDetector::TemplateMatch(Mat &src) {
    for(auto &i : forward_text_box) {
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

        for(model_cnt = 1; model_cnt <= max_model_num; model_cnt++){
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
                //cout << "min difference : " << minVal << endl;
                if(fabs(minVal) < 5e-08 && minVal) break; // if the square deviation is small enough,
            }
        }
        if(model_cnt > max_model_num) { // which means template match does not success, delete this rotateRect
            swap(i, forward_text_box[forward_text_box.size() - 1]);
            forward_text_box.pop_back();
        }
    }
}






