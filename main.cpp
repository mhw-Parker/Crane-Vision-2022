//
// Created by TYY on 2022/4/14.
//
#include <thread>

#include "Driver.h"
#include "MilkBoxDetector.h"
#include "FIFO.h"

using namespace std;
using namespace cv;

string savePath = "../photos/";

bool debug = true;
int start_num = 75; //the start number of the first photo
Driver driver; // camera or photo
MilkBoxDetector detector;
FIFO<Mat> mat_queue;
Mat dst;

string FileLocation(string Location, int num, string EndLocationType)
{
    string imageFileName;
    std::stringstream StrStm1;
    StrStm1 << num + 1;
    StrStm1 >> imageFileName;
    imageFileName += EndLocationType;
    return Location + imageFileName;
}

void Produce(){
    if(!debug) {
        if(driver.StartGrab())
            printf("camera set successfully ! \n");
        else
            return;
    }
    while (1) {
        Mat frame;
        if(debug) {
            string src_path = FileLocation(savePath,++start_num,".jpg");
            driver.Grab(frame,src_path);
        } else {
            driver.Grab(frame);
#if SAVE_PHOTO == 1
            if(waitKey(10) == 32) {
                string path = FileLocation(savePath,start_num++,".jpg");
                imwrite(path,frame);
                printf("take a shot !\n");
            }
#endif
        }
        if(frame.empty()) {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        mat_queue.push(frame);
        if(debug) {
            this_thread::sleep_for(1000ms);
            //waitKey();
        }
    }
}
void Consume(){
    while (1) {
        dst = mat_queue.wait_and_pop().clone();
        if(dst.empty())
            break;
        detector.DetectMilkBox(dst);
        imshow("result",dst);
        waitKey(1);
#if SAVE_TEMPLATE == 1
        if(!detector.tempImg.empty()) {
            string path = FileLocation("../TemplateModel/",++start_num,".jpg");
            imwrite(path,detector.tempImg);
        }
#endif
    }
}
int main(int argc, char** argv)
{
   std::thread grabThread(Produce);
   std::thread detectThread(Consume);
   printf("thread id : %d\t%d\n",grabThread.get_id(),detectThread.get_id());
   if(grabThread.joinable() && grabThread.joinable()) {
       printf("Thread start !\n");
       grabThread.join();
       detectThread.join();
   }
}

