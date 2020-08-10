//#include <iostream>
//#include <string>
//#include <opencv2/opencv.hpp>
//
//using namespace std;
//using namespace cv;
//
////Mat srcImage;
//
//const double realLength = 550.0; //24cm=90pixels
//const double realWidth = 309.4;
//const double factor = 0.26667;
//
//float circleRadius = 0;
//const float minSideLength = 89;
//const float maxSideLength = 93;
//
//const int n = 4;
//const double e = 5;
//const int bitnum = n * n - 4;
//
//
//Vec3f getPointAffinedPos(Vec3f& src, const Point center, double angle)
//{
//    angle = angle * CV_PI / 180;
//    int x = src[0] - center.x;
//    int y = src[1] - center.y;
//    src[0] = cvRound(x * cos(angle) + y * sin(angle) + center.x);
//    src[1] = cvRound(-x * sin(angle) + y * cos(angle) + center.y);
//    return src;
//}
//
//int main() {
//
//    VideoCapture cap(0);
//    cap.set(3, 1920);//��� 
//    cap.set(4, 1080);//�߶�
//    if (!cap.isOpened())
//    {
//        cout << "����ͷδ�ɹ���" << endl;
//    }
//    Mat resultImg;
//    //��ȡ����ͷ�е�֡
//
//    clock_t startTime, endTime;
//    for (; ; )
//    {
//        startTime = clock();//��ʱ��ʼ
//        int id = 0;
//        int bits[bitnum];
//        Mat srcImage;
//        Mat grayImage;
//        Mat correctImage;
//
//        cap >> srcImage;
//        //�ж��Ƿ��ȡ��
//        if (srcImage.empty())
//        {
//            break;
//        }
//        Point cameraCenter(srcImage.cols / 2, srcImage.rows / 2);
//        circle(srcImage, cameraCenter, 3, Scalar(0, 255, 0), -1, 8, 0);
//        endTime = clock();//��ʱ����
//        cout << "The run time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
//
//        imshow("camera", srcImage);
//        if (waitKey(300) >= 0)
//        {
//            break;
//        }
//    }
//
//
//    return 0;
//}
