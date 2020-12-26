#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


int main1() {
   clock_t startTime, endTime;

   Mat srcImage = imread("/home/linux/zhangqr/visualLocalize/fish.jpeg");


   endTime = clock();
   cout << "单帧处理耗时：" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

    return 0;
}