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
//double cameraX;
//double cameraY;
//
//double cameraRealX;
//double cameraRealY;
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
//
//        imwrite("C:/Users/a/Desktop/photo10.jpg", srcImage);
//
//        //ת��Ϊ�Ҷ�ͼ
//        cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);
//
//        //�������λ��
//        Point cameraCenter(grayImage.cols / 2, grayImage.rows / 2);
//
//        //��λ����Բ�Ĳ��ҳ���λ����
//        vector<Vec3f> circles;
//        HoughCircles(grayImage, circles, HOUGH_GRADIENT, 1, 10, 200, 15, 0, 10);
//
//        if (circles.empty()) {
//            cout << "δ��⵽Բ��" << endl;
//        }
//        else {
//            cout << "��⵽Բ��Ŀ:" << circles.size() << endl;
//
//            //ɸѡ������������Բ�����Բ�ľ���Ϊ�뾶�ĵ�
//            Vec3f LeftTop;
//            Vec3f RightTop;
//            Vec3f LeftBottom;
//            float distance;
//            int i = 0;
//            int j = 0;
//            for (i = 0; i < circles.size(); i++) {
//                int num = 0;
//                for (j = 0; j < circles.size(); j++) {
//                    distance = sqrt(pow(circles[i][0] - circles[j][0], 2) + pow(circles[i][1] - circles[j][1], 2));
//                    if (distance <= maxSideLength && distance >= minSideLength) {
//                        num++;
//                        if (num == 1) {
//                            RightTop = circles[j];
//                        }
//                        if (num == 2) {
//                            LeftBottom = circles[j];
//                            break;
//                        }
//                    }
//                }
//                if (num == 2) {
//                    LeftTop = circles[i];
//                    bool flag = true;
//                    if (LeftTop[0] < RightTop[0] && LeftTop[0] <= LeftBottom[0]) { //0<=angle<90��ʱ����ת
//                        if (!(LeftTop[1] >= RightTop[1] || LeftTop[1] < LeftBottom[1])) {
//                            flag = false;
//                        }
//                    }
//                    else if (LeftTop[0] >= RightTop[0] && LeftTop[0] < LeftBottom[0]) {//90<=angle<180
//                        if (!(LeftTop[1] > RightTop[1] || LeftTop[1] >= LeftBottom[1])) {
//                            flag = false;
//                        }
//                    }
//                    else if (LeftTop[0] > RightTop[0] && LeftTop[0] >= LeftBottom[0]) {//180<=angle<270
//                        if (!(LeftTop[1] <= RightTop[1] || LeftTop[1] > LeftBottom[1])) {
//                            flag = false;
//                        }
//                    }
//                    else {// if(LeftTop[0] <= RightTop[0] && LeftTop[0] > LeftBottom[0])270<=angle<360
//                        if (!(LeftTop[1] < RightTop[1] || LeftTop[1] <= LeftBottom[1])) {
//                            flag = false;
//                        }
//                    }
//                    if (!flag) {
//                        Vec3f temp = RightTop;
//                        RightTop = LeftBottom;
//                        LeftBottom = temp;
//                    }
//                    break;
//                }
//            }
//
//            //���ƶ�λ��,0��2����������ת��
//            Point pt0(cvRound(LeftTop[0]), cvRound(LeftTop[1]));
//            Point pt1(cvRound(RightTop[0]), cvRound(RightTop[1]));
//            Point pt2(cvRound(LeftBottom[0]), cvRound(LeftBottom[1]));
//
//            //����ͼ��������������϶���ľ���
//            double imgDis = sqrt(pow((cameraCenter.x - LeftTop[1]), 2) + pow((cameraCenter.y - LeftTop[0]), 2));
//
//            //���������landmark�ķ���
//            float theta2 = atan2(LeftTop[0] - LeftBottom[0], LeftTop[1] - LeftBottom[1]) - atan2(cameraCenter.x - LeftTop[0], cameraCenter.y - LeftTop[1]);
//            if (theta2 > CV_PI)
//                theta2 -= 2 * CV_PI;
//            if (theta2 < -CV_PI)
//                theta2 += 2 * CV_PI;
//            theta2 = theta2 * 180.0 / CV_PI;
//
//            //ȫ��תΪ˳ʱ�뷽��
//            if (theta2 < 0) {
//                theta2 += 360.0;
//            }
//
//            cout << "theta2:" << theta2 << endl;
//
//            theta2 = theta2 / 180.0 * CV_PI;
//            //���������landmark������
//            cameraX = imgDis * sin(theta2);
//            cameraY = imgDis * cos(theta2);
//
//            //����Ϊʵ��λ��
//            cameraRealX = cameraX * factor;
//            cameraRealY = cameraY * factor;
//
//            cout << "���ͼ������:(" << cameraX << "," << cameraY << ")" << endl;
//            cout << "���ʵ������:(" << cameraRealX << "," << cameraRealY << ")" << endl;
//
//            //Ϊ��ֱ����ȡ��
//            Point pt3(pt0.x, pt0.y - 1);
//
//            //��ȡ�������нǣ���Ϊ��ת�Ƕȣ�˳ʱ�뷽��Ϊ����
//            float theta = atan2(pt3.x - pt0.x, pt3.y - pt0.y) - atan2(pt0.x - pt2.x, pt0.y - pt2.y);
//            if (theta > CV_PI)
//                theta -= 2 * CV_PI;
//            if (theta < -CV_PI)
//                theta += 2 * CV_PI;
//            theta = theta * 180.0 / CV_PI;
//            cout << "theta:" << theta << endl;
//
//            //��ת����
//            Point center(srcImage.cols / 2, srcImage.rows / 2);
//            Mat rotMat = getRotationMatrix2D(center, theta, 1);
//            warpAffine(srcImage, correctImage, rotMat, Size(srcImage.size()));
//
//            //����任��λ���Ӧ�������꣬theta��ת�Ƕ�Ϊ��ʱ��
//            for (int i = 0; i < circles.size(); i++) {
//                circles[i] = getPointAffinedPos(circles[i], center, theta);
//            }
//            //������circles�����ȥ������λ��
//            LeftTop = getPointAffinedPos(LeftTop, center, theta);
//            RightTop = getPointAffinedPos(RightTop, center, theta);
//            LeftBottom = getPointAffinedPos(LeftBottom, center, theta);
//
//            //x��y���������
//            double dx = (RightTop[0] - LeftTop[0]) / (n - 1);
//            double dy = (LeftBottom[1] - LeftTop[1]) / (n - 1);
//
//            //��λ���Բ�Ƿ����
//            int seq = 0;
//            for (int i = 1; i <= n; i++) {
//                for (int j = 1; j <= n; j++) {
//                    if ((i == 1 && j == 1) || (i == 1 && j == n) || (i == n && j == 1) || (i == n && j == n)) {
//                        continue;
//                    }
//                    double bitx = LeftTop[0] + (j - 1) * dx;
//                    double bity = LeftTop[1] + (i - 1) * dy;
//                    bits[seq] = 0;
//                    for (int k = 0; k < circles.size(); k++) {
//                        if (fabs(circles[k][0] - bitx) <= e && fabs(circles[k][1] - bity) <= e) {
//                            bits[seq] = 1;
//                            circles.erase(circles.begin() + k);
//                            break;
//                        }
//                    }
//                    seq++;
//                }
//            }
//
//            //����λֵ�������
//            for (int i = 0; i < bitnum; i++) {
//                id += bits[i] * pow(2, i);
//            }
//
//            cout << "ʶ��ID:" << id << endl;
//
//            endTime = clock();//��ʱ����
//            cout << "The run time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
//
//            imshow("camera", srcImage);
//            if (waitKey(300) >= 0)
//            {
//                break;
//            }
//        }
//    }
//
//
//    return 0;
//}
