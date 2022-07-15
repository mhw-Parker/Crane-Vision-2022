//
// Created by TYY on 2022/4/14.
//
#include <thread>

#include "Driver.h"
#include "MilkBoxDetector.h"
#include "FIFO.h"

#if LINUX == 1
#include "SerialPort.hpp"
Serial com;
ReceiveData receiveData;
#endif

using namespace std;
using namespace cv;

string savePath = "../output/photos/";
string modelPath = "../model/best.onnx";

bool debug = true;
int start_num = 450; //the start number of the first photo
int cnt = 0;
bool start_flag = false;
bool identify = false;
int target_pose = 0;
int pose_counter[7] = {0};
Driver driver;      // camera or photo
MilkBoxDetector detector;

FIFO<Mat> mat_queue;
Mat dst;

string FileLocation(string Location, int num, string EndLocationType)
{
    string imageFileName;
    std::stringstream StrStm1;
    StrStm1 << num;
    StrStm1 >> imageFileName;
    imageFileName += EndLocationType;
    return Location + imageFileName;
}
/**
 * @brief get current time from system
 */
inline string getSysTime() {
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d_%H_%M_%S", localtime(&timep));
    return tmp;
}
#if SAVE_VIDEO == 1
string video_path = string("../output/video/" + getSysTime()).append(".avi");
cv::VideoWriter save_video(video_path, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 50.0, cv::Size(FRAME_WIDTH, FRAME_HEIGHT));
#endif
/**
 * @brief producer get source and provide for consumer
 * */
void Produce() {
    if(!debug) {
        if(driver.StartGrab() && driver.SetCam())
            printf("camera set successfully ! \n");
        else
            return;
    }
    while (1) {
        Mat frame;
        if(debug) {
            string src_path = FileLocation(savePath,824,".jpg");
            driver.Grab(frame,src_path);
        } else {
            driver.Grab(frame);
            flip(frame,frame,-1);
#if SAVE_VIDEO == 1
            save_video.write(frame);
#endif
#if SAVE_PHOTO == 1
#if LINUX == 1
            if(cnt++%100==0) {
                string path = FileLocation(savePath,start_num++,".jpg");
                imwrite(path,frame);
                cout << "the " << start_num << "photos" << endl;
                printf("take %d shot !\n",start_num);
            }
#else
            if(waitKey(10) == 32) {
                string path = FileLocation(savePath,start_num++,".jpg");
                imwrite(path,frame);
                printf("take a shot !\n");
            }
#endif
#endif
        }
        if(frame.empty()) {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        mat_queue.push(frame);
        if(debug) {
            this_thread::sleep_for(500ms);
            //waitKey();
        }
    }
}
/**
 * @brief consumer consume source from producer
 * @details detector use frame from camera
 * */
void Consume(){
    while (1) {
        dst = mat_queue.wait_and_pop().clone();
        if(dst.empty())
            break;
        detector.DetectMilkBox(dst);
#if LINUX == 1
        if(com.ReadData(receiveData)) {
            start_flag = receiveData.flag;
            //cout << start_flag << endl;
        }
        if(start_flag) { // which means I can send pose
            switch(detector.pose) {
                case 0: pose_counter[0]++;  break;
                case 1: pose_counter[1]++;  break;
                case 2: pose_counter[2]++;  break;
                case 3: pose_counter[3]++;  break;
                case 4: pose_counter[4]++;  break;
                case 5: pose_counter[5]++;  break;
                case 6: pose_counter[6]++;  break;
                default:
                    break;
            }
            for(int i = 0; i < 7; i++) {
                if(pose_counter[i] > 20) {
                    target_pose = i;
                    identify = true;
                    printf("POSE : %d\tIDENTIFY : %d\n",target_pose,identify);
                    for(auto &j : pose_counter)
                        j = 0;
                    break;
                }
            }
        }
        else {
            identify = false;
            for(auto &i : pose_counter)
                i = 0;
        }
#endif
        imshow("result",dst);
        waitKey(1);
#if LINUX == 1
        com.pack(target_pose, identify);
        if(com.WriteData()) {
            //printf("Write data success !\n");
        }
#endif
#if SAVE_TEMPLATE == 1
        if(!detector.tempImg.empty()) {
            string path = FileLocation(TEMPLATE_PATH,++start_num,".jpg");
            imwrite(path,detector.tempImg);
        }
#endif
    }
}
/**
 * @brief main function
 * */
int main(int argc, char** argv)
{
#if MODEL_MODE == 1
    detector.init_yolov5(modelPath);
#endif
#if LINUX == 1
    if(com.InitPort()){
        printf("Port set success !\n");
    }
#endif
   std::thread grabThread(Produce);
   std::thread detectThread(Consume);

   if(grabThread.joinable() && grabThread.joinable()) {
       printf("Thread start !\n");
       grabThread.join();
       detectThread.join();
   }
}

