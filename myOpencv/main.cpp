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
    cap.set(3, 1920);//��� 
    cap.set(4, 1080);//�߶�
    if (!cap.isOpened())
    {
        cout << "����ͷδ�ɹ���" << endl;
    }
    Mat resultImg;

    //��ȡ����ͷ�е�֡
    clock_t startTime, endTime;
    for (; ; )
    {
        startTime = clock();//��ʱ��ʼ
        
        Mat srcImage;
        Mat grayImage;
        Mat binaryImage;

        cap >> srcImage;
        //�ж��Ƿ��ȡ��
        if (srcImage.empty())
        {
            break;
        }

        //ת��Ϊ�Ҷ�ͼ
        cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);

        //���������������
        Point cameraCenter(grayImage.cols / 2, grayImage.rows / 2);

        //��ȡ��������������ɢ����Ӿ���
        vector<Rect> targetRects = getTargetRects(grayImage, binaryThreshold, rectMinLength, rectMaxLength);
        if (targetRects.size() < 1) {
            cout << "δ��⵽Ŀ��ɢ����Ӿ���" << endl;
            continue;
        }

        //��ȡ����Ŀ��ROI����
        vector<Rect> ROIRects = getLandmarkRois(targetRects,minDisValue,maxDisValue, fixedPointsNum);
        if (ROIRects.size() < 1 ) {
            cout << "δ��⵽Ŀ��ROI����" << endl;
            continue;
        }

        for (int i = 0; i < ROIRects.size(); i++) {
            
            //��λ����Բ�Ĳ��ҳ���λ����
            vector<Vec3f> circles = getAbsoluteCircles(grayImage, ROIRects[i], dp, minDist, param1, param2, minRadius, maxRadius);

            vector<Vec3f> points = getThreeFixedPoints(circles, minSideLength, maxSideLength);

            //����
            correctFixedPoints(points);

            //��������
            double *coordinate_value = getCoordinateValue(points, cameraCenter, realSideLength);
            double x = coordinate_value[0];
            double y = coordinate_value[1];

            //������ת�Ƕ�
            float theta = getRotationAngle(points);

            //����任���������


            //����任��λ���Ӧ�������꣬theta��ת�Ƕ�Ϊ��ʱ��
            for (int i = 0; i < circles.size(); i++) {
                circles[i] = getPointAffinedPos(circles[i], cameraCenter, theta);
            }

            points[0] = getPointAffinedPos(points[0], cameraCenter, theta);
            points[1] = getPointAffinedPos(points[1], cameraCenter, theta);
            points[2] = getPointAffinedPos(points[2], cameraCenter, theta);

            //ʶ��·��ID
            int id = getLandmarkId(circles, points);

            endTime = clock();//��ʱ����
            cout << "The run time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

            cout << "��⵽Բ��Ŀ:" << circles.size() << endl;
            cout << "���ʵ������:(" << x << "," << y << ")" << endl;
            cout << "ͼ����ת�Ƕ�(˳ʱ��):" << theta << endl;
            cout << "ʶ��ID:" << id << endl;
            cout << "========================================================" << endl;
        }
        
    }


    return 0;
}
