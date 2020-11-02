#include <opencv2/opencv.hpp>
#include "loadConfig.h"
#include "visualLocalize.h"

using namespace std;
using namespace cv;


int main() {
//    clock_t startTime, endTime;
//    loadConfig();
//    loadVisualParams();
//    Mat srcImage = imread("/Users/zhangqr/Desktop/uncheck/zzz56.jpg");
//
//    LocalizeData visualData = getVisualLocalizeData(srcImage);
//
//    endTime = clock();
//    cout << "单帧处理耗时：" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
//    cout << visualData.getVisualTheta() << "---（" << visualData.getVisualX() << "," << visualData.getVisualY()<< "）"  << endl;
    


    Mat srcImage;

    VideoCapture cap(1);
    cap.set(3,1920);
    cap.set(4,1080);
    if(!cap.isOpened()){
        cout << "摄像头未成功打开" << endl;
        return 0;
    }

    // 加载配置文件
    loadConfig();

    //取配置参数
    loadVisualParams();

    for(; ;){
        clock_t startTime, endTime;

        cap >> srcImage;

        if(srcImage.empty()){
            break;
        }


        startTime = clock();
        // 视觉定位数据获取
        LocalizeData visualData = getVisualLocalizeData(srcImage);
        endTime = clock();
        cout << "单帧处理耗时：" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
        cout << visualData.getDTheta() << "," << visualData.getVisualTheta() << "---（" << visualData.getVisualX() << "," << visualData.getVisualY()<< "）"  << endl;

        namedWindow("show",WINDOW_NORMAL);
        imshow("show",srcImage);
        waitKey(20);
         //uwb定位数据获取

         //数据融合

    }
    
    
    return 0;
}
