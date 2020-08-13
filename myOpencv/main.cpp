#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "absolute_circles.h"
#include "coordinate_value.h"
#include "correct_fixed_points.h"
#include "fixed_points.h"
#include "identify.h"
#include "landmark_rois.h"
#include "pos_affine.h"
#include "rotation.h"
#include "target_rects.h"
#include "landmark_rois.h"

using namespace std;
using namespace cv;

const int binaryThreshold = 220;
const int rectMinLength = 8;
const int rectMaxLength = 20;

const int minDisValue = 15;
const int maxDisValue = 160;
const int fixedPointsNum = 3;

const double dp = 1;
const double minDist = 10;
const double param1 = 200;
const double param2 = 15;
const int minRadius = 0;
const int maxRadius = 10;

const double minSideLength = 89;
const double maxSideLength = 93;

const double realSideLength = 24;

int main() {

    VideoCapture cap(0);
    cap.set(3, 1920);//宽度 
    cap.set(4, 1080);//高度
    if (!cap.isOpened())
    {
        cout << "摄像头未成功打开" << endl;
    }
    Mat resultImg;

    //读取摄像头中的帧
    clock_t startTime, endTime;
    for (; ; )
    {
        startTime = clock();//计时开始
        
        Mat srcImage;
        Mat grayImage;
        Mat binaryImage;

        cap >> srcImage;
        //判断是否读取到
        if (srcImage.empty())
        {
            break;
        }

        //转化为灰度图
        cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);

        //计算相机中心坐标
        Point cameraCenter(grayImage.cols / 2, grayImage.rows / 2);

        //获取符合条件的所有散点外接矩阵
        vector<Rect> targetRects = getTargetRects(grayImage, binaryThreshold, rectMinLength, rectMaxLength);
        if (targetRects.size() < 1) {
            cout << "未检测到目标散点外接矩阵" << endl;
            continue;
        }

        //获取所有目标ROI矩阵
        vector<Rect> ROIRects = getLandmarkRois(targetRects,minDisValue,maxDisValue, fixedPointsNum);
        if (ROIRects.size() < 1 ) {
            cout << "未检测到目标ROI矩阵" << endl;
            continue;
        }

        for (int i = 0; i < ROIRects.size(); i++) {
            
            //定位所有圆心并找出定位顶点
            vector<Vec3f> circles = getAbsoluteCircles(grayImage, ROIRects[i], dp, minDist, param1, param2, minRadius, maxRadius);

            vector<Vec3f> points = getThreeFixedPoints(circles, minSideLength, maxSideLength);

            //矫正
            correctFixedPoints(points);

            //计算坐标
            double *coordinate_value = getCoordinateValue(points, cameraCenter, realSideLength);
            double x = coordinate_value[0];
            double y = coordinate_value[1];

            //计算旋转角度
            float theta = getRotationAngle(points);

            //仿射变换所有坐标点


            //放射变换后定位点对应的新坐标，theta旋转角度为逆时针
            for (int i = 0; i < circles.size(); i++) {
                circles[i] = getPointAffinedPos(circles[i], cameraCenter, theta);
            }

            points[0] = getPointAffinedPos(points[0], cameraCenter, theta);
            points[1] = getPointAffinedPos(points[1], cameraCenter, theta);
            points[2] = getPointAffinedPos(points[2], cameraCenter, theta);

            //识别路标ID
            int id = getLandmarkId(circles, points);

            endTime = clock();//计时结束
            cout << "The run time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

            cout << "检测到圆数目:" << circles.size() << endl;
            cout << "相机实际坐标:(" << x << "," << y << ")" << endl;
            cout << "图像旋转角度(顺时针):" << theta << endl;
            cout << "识别到ID:" << id << endl;
            cout << "========================================================" << endl;
        }
        
    }


    return 0;
}
