//#include <iostream>
//#include <string>
//#include <opencv2/opencv.hpp>
//
//using namespace std;
//using namespace cv;
//
//Mat srcImage;
//Mat grayImage;
//float circleRadius = 0;
//float minSideLength = 0;
//float maxSideLength = 0;
//const int n = 4;
//const double e = 5;
//const int bitnum = n * n - 4;
//int bits[bitnum];
//int id = 0;
//
//
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
//int main()
//{
//    //��ȡͼƬ
//    srcImage = imread("C:/Users/a/Desktop/photo3.jpg");
//
//    //ת��Ϊ�Ҷ�ͼ
//    cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);
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
//    for (int i = 0; i < circles.size(); i++) {
//        //��̬���ñ߳�
//        circleRadius = circles[i][2];
//        //minSideLength = circleRadius * 6.5;//7,6.5
//        //maxSideLength = circleRadius * 7.5;//8,7.5
//        minSideLength = 69;
//        maxSideLength = 73;
//        int num = 0;
//        for (int j = 0; j < circles.size(); j++) {
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
//    //��ȡ�������нǣ���Ϊ��ת�Ƕȣ�˳ʱ�뷽��Ϊ����
//    float theta = atan2(LeftTop[0] - LeftTop[0], (LeftTop[1] - 1) - LeftTop[1]) - atan2(LeftTop[0] - LeftBottom[0], LeftTop[1] - LeftBottom[1]);
//    if (theta > CV_PI)
//        theta -= 2 * CV_PI;
//    if (theta < -CV_PI)
//        theta += 2 * CV_PI;
//    theta = theta * 180.0 / CV_PI;
//    cout << "theta:" << theta << endl;
//
//    //��ת����,theta����Ϊ����ʾ��˳ʱ�뷽����ת
//    Mat correctImage;
//    Point center(srcImage.cols / 2, srcImage.rows / 2);
//    Mat rotMat = getRotationMatrix2D(center, theta, 1);
//    warpAffine(srcImage, correctImage, rotMat, Size(srcImage.size()));
//
//    cout << center.x << ";" << center.y << endl;
//    cout << correctImage.cols / 2 << ";" << correctImage.rows / 2 << endl;
//
//    //����任��λ���Ӧ�������꣬theta��ת�Ƕ�Ϊ��ʱ��
//    for (int i = 0; i < circles.size(); i++) {
//        //cout << circles[i][0]<<","<< circles[i][1] << "," << circles[i][2]<< endl;
//        circles[i] = getPointAffinedPos(circles[i], center, theta);
//        //cout << circles[i][0] << "," << circles[i][1] << "," << circles[i][2] << endl;
//    }
//    //������circles�����ȥ������λ��
//    LeftTop = getPointAffinedPos(LeftTop, center, theta);
//    RightTop = getPointAffinedPos(RightTop, center, theta);
//    LeftBottom = getPointAffinedPos(LeftBottom, center, theta);
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
//        circle(correctImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
//        //����Բ����
//        circle(correctImage, center, radius, Scalar(155, 50, 255), 2, 8, 0);
//    }
//
//    namedWindow("processimg", WINDOW_NORMAL);
//    imshow("processimg", correctImage);
//    waitKey(0);
//
//
//    //x��y���������
//    double dx = (RightTop[0] - LeftTop[0]) / (n - 1);
//    double dy = (LeftBottom[1] - LeftTop[1]) / (n - 1);
//
//    //��λ���Բ�Ƿ����
//    int seq = 0;
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
//                    circles.erase(circles.begin() + k);
//                    break;
//                }
//            }
//            seq++;
//        }
//    }
//
//    //����λֵ�������
//    for (int i = 0; i < bitnum; i++) {
//        id += bits[i] * pow(2, i);
//    }
//
//    cout << "ʶ��ID:" << id << endl;
//
//    return 0;
//}
