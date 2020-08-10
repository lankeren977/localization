//#include <iostream>
//#include <string>
//#include <opencv2/opencv.hpp>
//
//using namespace std;
//using namespace cv;
//
//Mat srcImage;
//Mat grayImage;
//
//double factor1 = 0;//��������
//double factor2 = 0;//��������
//
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
//int bits[bitnum];
//int id = 0;
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
//    //��ȡͼƬ
//    srcImage = imread("C:/Users/a/Desktop/photo5.jpg");
//
//    //ת��Ϊ�Ҷ�ͼ
//    cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);
//
//    //�������λ��
//    Point cameraCenter(grayImage.cols / 2, grayImage.rows / 2);
//
//    //��λ����Բ�Ĳ��ҳ���λ����
//    vector<Vec3f> circles;
//    HoughCircles(grayImage, circles, HOUGH_GRADIENT, 1, 10, 200, 15, 0, 10);
//    if (circles.empty()) {
//        cout << "δ��⵽Բ��" << endl;
//        return 0;
//    }
//    cout << "��⵽Բ��Ŀ:" << circles.size() << endl;
//
//    //ɸѡ������������Բ�����Բ�ľ���Ϊ�뾶�ĵ�
//    Vec3f LeftTop;
//    Vec3f RightTop;
//    Vec3f LeftBottom;
//    float distance;
//    int i = 0;
//    int j = 0;
//    for (i = 0; i < circles.size(); i++) {
//        int num = 0;
//        for (j = 0; j < circles.size(); j++) {
//            distance = sqrt(pow(circles[i][0] - circles[j][0], 2) + pow(circles[i][1] - circles[j][1], 2));
//            if (distance <= maxSideLength && distance >= minSideLength) {
//                num++;
//                if (num == 1) {
//                    RightTop = circles[j];
//                }
//                if (num == 2) {
//                    LeftBottom = circles[j];
//                    break;
//                }
//            }
//        }
//        if (num == 2) {
//            LeftTop = circles[i];
//            bool flag = true;
//            if (LeftTop[1] <= RightTop[1] && LeftTop[1] <= LeftBottom[1]) { //�ϣ�0<=angle<90��ʱ����ת
//                if (LeftBottom[0] > RightTop[0]) {
//                    flag = false;
//                }
//            }
//            else if (LeftTop[0] >= RightTop[0] && LeftTop[0] >= LeftBottom[0]) {//��90<=angle<180
//                if (RightTop[1] < LeftBottom[1]) {
//                    flag = false;
//                }
//            }
//            else if (LeftTop[1] >= RightTop[1] && LeftTop[1] >= LeftBottom[1]) {//��180<=angle<270
//                if (RightTop[0] > LeftBottom[0]) {
//                    flag = false;
//                }
//            }
//            else if (LeftTop[0] <= RightTop[0] && LeftTop[0] <= LeftBottom[0]) {//��270<=angle<360
//                if (RightTop[1] > LeftBottom[1]) {
//                    flag = false;
//                }
//            }
//            if (!flag) {
//                Vec3f temp = RightTop;
//                RightTop = LeftBottom;
//                LeftBottom = temp;
//            }
//            break;
//        }
//    }
//
//    //����ǰ
//    float vertical = atan2(RightTop[0] - LeftTop[0], RightTop[1] - LeftTop[1]) - atan2(LeftBottom[0] - LeftTop[0], LeftBottom[1] - LeftTop[1]);
//    vertical = vertical * 180.0 / CV_PI;
//    cout << "����ǰ����нǣ�" << vertical << endl;
//
//    //����ƫת�½���һ����������
//    if (LeftTop[0] == LeftBottom[0]) {
//        RightTop[1] = LeftTop[1];
//        cout << "����ƫת����1" << endl;
//    }
//    else if (LeftTop[1] == LeftBottom[1]) {
//        RightTop[0] = LeftTop[0];
//        cout << "����ƫת����2" << endl;
//    }
//    else if (LeftTop[0] == RightTop[0]) {
//        LeftBottom[1] = LeftTop[1];
//        cout << "����ƫת����3" << endl;
//    }
//    else if (LeftTop[1] == RightTop[1]) {
//        LeftBottom[0] = LeftTop[0];
//        cout << "����ƫת����4" << endl;
//    }
//
//    //������
//    float vertical2 = atan2(RightTop[0] - LeftTop[0], RightTop[1] - LeftTop[1]) - atan2(LeftBottom[0] - LeftTop[0], LeftBottom[1] - LeftTop[1]);
//    vertical2 = vertical2 * 180.0 / CV_PI;
//    cout << "����������нǣ�" << vertical2 << endl;
//
//    //����ԲȦ
//    vector<Vec3f> drawCircles;
//    drawCircles.push_back(LeftTop);
//    drawCircles.push_back(RightTop);
//    drawCircles.push_back(LeftBottom);
//    for (int i = 0; i < drawCircles.size(); i++) {
//        Point center(cvRound(drawCircles[i][0]), cvRound(drawCircles[i][1]));
//        int radius = cvRound(drawCircles[i][2]);
//        //����Բ��
//        circle(srcImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
//        //����Բ����
//        circle(srcImage, center, radius, Scalar(155, 50, 255), 2, 8, 0);
//    }
//
//    //���ƶ�λ��,0��2����������ת��
//    Point pt0(cvRound(LeftTop[0]), cvRound(LeftTop[1]));
//    Point pt1(cvRound(RightTop[0]), cvRound(RightTop[1]));
//    Point pt2(cvRound(LeftBottom[0]), cvRound(LeftBottom[1]));
//    line(srcImage, pt0, pt1, Scalar(0, 0, 255), 2, 8, 0);
//    line(srcImage, pt0, pt2, Scalar(0, 0, 255), 2, 8, 0);
//
//    //����ͼ��������������϶���ľ���
//    double imgDis = sqrt(pow((cameraCenter.x - LeftTop[0]), 2) + pow((cameraCenter.y - LeftTop[1]), 2));
//
//    //���������landmark�ķ���
//    float theta2 = atan2(LeftTop[0] - LeftBottom[0], LeftTop[1] - LeftBottom[1]) - atan2(cameraCenter.x - LeftTop[0], cameraCenter.y - LeftTop[1]);
//    if (theta2 > CV_PI)
//        theta2 -= 2 * CV_PI;
//    if (theta2 < -CV_PI)
//        theta2 += 2 * CV_PI;
//    theta2 = theta2 * 180.0 / CV_PI;
//    //ȫ��תΪ˳ʱ�뷽��
//    if (theta2 < 0) {
//        theta2 += 360.0;
//    }
//
//    cout << "theta2:" << theta2 << endl;
//    theta2 = theta2 / 180.0 * CV_PI;
//    //���������landmark������
//    cameraX = imgDis * sin(theta2);
//    cameraY = imgDis * cos(theta2);
//
//    //�����������
//    double disLeft = sqrt(pow(LeftTop[0] - LeftBottom[0], 2) + pow(LeftTop[1] - LeftBottom[1], 2));
//    double disTop = sqrt(pow(LeftTop[0] - RightTop[0], 2) + pow(LeftTop[1] - RightTop[1], 2));
//    cout << "x�᳤�ȣ�" << disTop << " y�᳤�ȣ�" << disLeft << endl;
//    factor1 = 24 / disTop;
//    factor2 = 24 / disLeft;
//
//    //����Ϊʵ��λ��
//    cameraRealX = cameraX * factor1;
//    cameraRealY = cameraY * factor2;
//
//    cout << "ͼ�����ĵ����ر����ϵ���룺" << imgDis << "pixels," << imgDis * (factor1 + factor2) / 2 << "cm" << endl;
//    cout << "���ͼ������:(" << cameraX << "," << cameraY << ")" << endl;
//    cout << "���ʵ������:(" << cameraRealX << "," << cameraRealY << ")" << endl;
//
//    //Ϊ��ֱ����ȡ��
//    Point pt3(pt0.x, pt0.y - 1);
//
//    //��ȡ�������нǣ���Ϊ��ת�Ƕȣ�˳ʱ�뷽��Ϊ����
//    float theta = atan2(pt3.x - pt0.x, pt3.y - pt0.y) - atan2(pt0.x - pt2.x, pt0.y - pt2.y);
//    if (theta > CV_PI)
//        theta -= 2 * CV_PI;
//    if (theta < -CV_PI)
//        theta += 2 * CV_PI;
//    theta = theta * 180.0 / CV_PI;
//    cout << "theta:" << theta << endl;
//
//    //��ת����
//    Mat correctImage;
//    Point center(srcImage.cols / 2, srcImage.rows / 2);
//    Mat rotMat = getRotationMatrix2D(center, theta, 1);
//    warpAffine(srcImage, correctImage, rotMat, Size(srcImage.size()));
//
//    //����任��λ���Ӧ�������꣬theta��ת�Ƕ�Ϊ��ʱ��
//    for (int i = 0; i < circles.size(); i++) {
//        circles[i] = getPointAffinedPos(circles[i], center, theta);
//    }
//    //������circles�����ȥ������λ��
//    LeftTop = getPointAffinedPos(LeftTop, center, theta);
//    RightTop = getPointAffinedPos(RightTop, center, theta);
//    LeftBottom = getPointAffinedPos(LeftBottom, center, theta);
//
//    //����ԲȦ
//    vector<Vec3f> drawCircles2;
//    drawCircles2.push_back(LeftTop);
//    drawCircles2.push_back(RightTop);
//    drawCircles2.push_back(LeftBottom);
//    for (int i = 0; i < drawCircles2.size(); i++) {
//        Point center(cvRound(drawCircles2[i][0]), cvRound(drawCircles2[i][1]));
//        int radius = cvRound(drawCircles2[i][2]);
//        //����Բ��
//        circle(correctImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
//        //����Բ����
//        circle(correctImage, center, radius, Scalar(155, 50, 255), 2, 8, 0);
//    }
//
//    //x��y���������
//    double dx = (RightTop[0] - LeftTop[0]) / (n - 1);
//    double dy = (LeftBottom[1] - LeftTop[1]) / (n - 1);
//
//    //��λ���Բ�Ƿ����
//    int seq = 0;
//    int circlesNum = circles.size() - 3;
//    int bitNum = 0;
//    for (int i = 1; i <= n; i++) {
//        for (int j = 1; j <= n; j++) {
//            if ((i == 1 && j == 1) || (i == 1 && j == n) || (i == n && j == 1) || (i == n && j == n)) {
//                continue;
//            }
//            double bitx = LeftTop[0] + (j - 1) * dx;
//            double bity = LeftTop[1] + (i - 1) * dy;
//            bits[seq] = 0;
//            for (int k = 0; k < circles.size(); k++) {
//                if (fabs(circles[k][0] - bitx) <= e && fabs(circles[k][1] - bity) <= e) {
//                    bits[seq] = 1;
//                    bitNum++;
//                    circles.erase(circles.begin() + k);
//                    break;
//                }
//            }
//            seq++;
//        }
//    }
//
//    //�ж϶�����Ϊ1����Ŀ��ԲȦ��Ŀ�Ƿ����
//    if (circlesNum != bitNum) {
//        cout << "IDʶ���쳣����©ԲȦ" << endl;
//    }
//
//    //����λֵ�������
//    for (int i = 0; i < bitnum; i++) {
//        id += bits[i] * pow(2, i);
//    }
//
//    cout << "ʶ��ID:" << id << endl;
//
//    //�����������߶�
//    line(srcImage, pt0, pt2, Scalar(0, 0, 255), 2, 8, 0);
//    line(srcImage, pt0, cameraCenter, Scalar(0, 0, 255), 2, 8, 0);
//
//    //imshow("srcImage", srcImage);
//    //imshow("correctImage", correctImage);
//    //waitKey(0);
//
//    return 0;
//}
