#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat srcImage;
Mat grayImage;
const float minSideLength = 190; //200
const float maxSideLength = 220; //212
const int n = 3;
const double e = 5;
const int bitnum = n*n-4;
int bits[bitnum];
int id = 0;

int main()
{
    //读取图片
    srcImage = imread("/Users/zhangqr/Desktop/zero.jpg");
    
    //转化为灰度图
    cvtColor(srcImage,grayImage,COLOR_RGB2GRAY);

    //定位所有圆心并找出定位顶点
    vector<Vec3f> circles;
    HoughCircles(grayImage, circles, HOUGH_GRADIENT, 2, 10, 200, 100, 0, 0);
    if(circles.empty()){
        cout<<"未检测到圆形"<<endl;
        return 0;
    }
    
    //筛选出存在其它两圆心与该圆心距离为半径的点
    Vec3f LeftTop;
    Vec3f RightTop;
    Vec3f LeftBottom;
    float distance;
    for(int i = 0; i < circles.size(); i++){
        int num = 0;
        for(int j = 0 ; j < circles.size() ; j++){
            distance = sqrt(pow(circles[i][0]-circles[j][0], 2) + pow(circles[i][1]-circles[j][1], 2));
            if(distance <= maxSideLength && distance >= minSideLength){
                num++;
                if(num == 1){
                    RightTop = circles[j];
                }
                if(num == 2){
                    LeftBottom = circles[j];
                    break;
                }
            }
        }
        if(num == 2){
            LeftTop = circles[i];
            bool flag = true;
            if(LeftTop[0] < RightTop[0] && LeftTop[0] <= LeftBottom[0]){ //0<=angle<90
                if(!(LeftTop[1] >= RightTop[1] && LeftTop[1] < LeftBottom[1])){
                    flag = false;
                }
            }else if(LeftTop[0] >= RightTop[0] && LeftTop[0] < LeftBottom[0]){//90<=angle<180
                if(!(LeftTop[1] > RightTop[1] && LeftTop[1] >= LeftBottom[1])){
                    flag = false;
                }
            }else if(LeftTop[0] > RightTop[0] && LeftTop[0] <= LeftBottom[0]){//180<=angle<270
                if(!(LeftTop[1] <= RightTop[1] && LeftTop[1] > LeftBottom[1])){
                    flag = false;
                }
            }else{//270<=angle<360
                if(!(LeftTop[1] < RightTop[1] && LeftTop[1] <= LeftBottom[1])){
                    flag = false;
                }
            }
            if(!flag){
                Vec3f temp = RightTop;
                RightTop = LeftBottom;
                LeftBottom = temp;
            }
            break;
        }
    }
    
    //x和y轴分量计算
    double dx = (RightTop[0] - LeftTop[0]) / (n-1);
    double dy = (LeftBottom[1] - LeftTop[1]) / (n-1);
    
    //按位检测圆是否存在
    int seq = 0;
    for(int i = 1; i <= n; i++){
        for(int j = 1; j<=n; j++){
            if((i == 1 && j == 1) || (i == 1 && j == n) || (i == n && j == 1) || (i == n && j == n)){
                continue;
            }
            double bitx = LeftTop[0] + (j-1) * dx;
            double bity = LeftTop[1] + (i-1) * dy;
            bits[seq] = 0;
            for(int k = 0; k < circles.size(); k++){
                if(fabs(circles[k][0] - bitx) <= e && fabs(circles[k][1] - bity) <=e){
                    bits[seq] = 1;
                    circles.erase(circles.begin() + k);
                    break;
                }
            }
            seq++;
        }
    }
    
    //根据位值计算序号
    for(int i = 0; i < bitnum; i++){
        id += bits[i] * pow(2,i);
    }
    
    cout<<"识别到ID:"<<id<<endl;
    
    return 0;
}


