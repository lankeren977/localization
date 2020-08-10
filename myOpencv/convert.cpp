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
//const double realLength = 550.0; //cm
//const double realWidth = 309.4;
//
//const float minSideLength = 69; //200
//const float maxSideLength = 73; //212
//
//double cameraX;
//double cameraY;
//
//double cameraRealX;
//double cameraRealY;
//
//
//int main() {
//
//    //��ȡͼƬ
//    srcImage = imread("C:/Users/a/Desktop/photo2.jpg");
//
//    //ת��Ϊ�Ҷ�ͼ
//    cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);
//
//    //�������λ��
//    Point cameraCenter(grayImage.cols / 2, grayImage.rows / 2);
//
//    //��λ����Բ�Ĳ��ҳ���λ����
//    vector<Vec3f> circles;
//    HoughCircles(grayImage, circles, HOUGH_GRADIENT, 2, 5, 200, 16, 0, 10);
//    if (circles.empty()) {
//        cout << "δ��⵽Բ��" << endl;
//        return 0;
//    }
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
//            if (LeftTop[0] < RightTop[0] && LeftTop[0] <= LeftBottom[0]) { //0<=angle<90
//                if (!(LeftTop[1] >= RightTop[1] && LeftTop[1] < LeftBottom[1])) {
//                    flag = false;
//                }
//            }
//            else if (LeftTop[0] >= RightTop[0] && LeftTop[0] < LeftBottom[0]) {//90<=angle<180
//                if (!(LeftTop[1] > RightTop[1] && LeftTop[1] >= LeftBottom[1])) {
//                    flag = false;
//                }
//            }
//            else if (LeftTop[0] > RightTop[0] && LeftTop[0] <= LeftBottom[0]) {//180<=angle<270
//                if (!(LeftTop[1] <= RightTop[1] && LeftTop[1] > LeftBottom[1])) {
//                    flag = false;
//                }
//            }
//            else {//270<=angle<360
//                if (!(LeftTop[1] < RightTop[1] && LeftTop[1] <= LeftBottom[1])) {
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
//    //����ͼ��������������϶���ľ���
//    double imgDis = sqrt(pow((cameraCenter.x - LeftTop[1]), 2) + pow((cameraCenter.y - LeftTop[0]), 2));
//
//    //�����������߶�
//    Point pt0(cvRound(LeftTop[0]), cvRound(LeftTop[1]));
//    Point pt1(cvRound(LeftBottom[0]), cvRound(LeftBottom[1]));
//    line(srcImage, pt0, pt1, Scalar(0, 0, 255), 2, 8, 0);
//    line(srcImage, pt0, cameraCenter, Scalar(0, 0, 255), 2, 8, 0);
//
//    //���������landmark�ķ���
//    float theta = atan2(LeftTop[0] - LeftBottom[0], LeftTop[1] - LeftBottom[1]) - atan2(cameraCenter.x - LeftTop[0], cameraCenter.y - LeftTop[1]);
//    if (theta > CV_PI)
//        theta -= 2 * CV_PI;
//    if (theta < -CV_PI)
//        theta += 2 * CV_PI;
//    theta = theta * 180.0 / CV_PI;
//
//    //ȫ��תΪ˳ʱ�뷽��
//    if (theta < 0) {
//        theta += 360.0;
//    }
//
//    cout << "theta:" << theta << endl;
//
//    theta = theta / 180.0 * CV_PI;
//    //���������landmark������
//    cameraX = imgDis * sin(theta);
//    cameraY = imgDis * cos(theta);
//
//    //����Ϊʵ��λ��
//    cameraRealX = cameraX * realLength / srcImage.cols;
//    cameraRealY = cameraY * realLength / srcImage.cols;
//
//    cout << "���ͼ������:(" << cameraX << "," << cameraY << ")" << endl;
//    cout << "���ʵ������:(" << cameraRealX << "," << cameraRealY << ")" << endl;
//
//    imshow("processImg", srcImage);
//    waitKey(0);
//
//    return 0;
//}
