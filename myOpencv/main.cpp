#include <opencv2/opencv.hpp>
#include "loadConfig.h"
#include "visualLocalize.h"

using namespace std;
using namespace cv;


int main() {
    //Mat srcImage = imread("/Users/zhangqr/Desktop/pics/photo3.jpg");
    Mat srcImage;
    
    VideoCapture cap(0);
    cap.set(3,1920);
    cap.set(4,1080);
    if(!cap.isOpened()){
        cout << "摄像头未成功打开" << endl;
        return 0;
    }
    
    // 加载配置文件
    loadConfig();
    
    for(; ;){
        clock_t startTime, endTime;
        startTime = clock();
        
        cap >> srcImage;
        if(srcImage.empty()){
            break;
        }
        
        // 视觉定位数据获取
        LocalizeData visualData = getVisualLocalizeData(srcImage);
        
        endTime = clock();
        cout << "单帧处理耗时：" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
        cout << visualData.getLandmarkId() << "---" << visualData.getVisualX() << "," << visualData.getVisualY() << endl;
        // uwb定位数据获取
           
        // 数据融合
        
    }
    
    return 0;
}
