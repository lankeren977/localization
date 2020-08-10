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
//    //读取图片
//    srcImage = imread("C:/Users/a/Desktop/photo3.jpg");
//
//    //转化为灰度图
//    cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);
//
//    //定位所有圆心并找出定位顶点
//    vector<Vec3f> circles;
//    HoughCircles(grayImage, circles, HOUGH_GRADIENT, 2, 5, 200, 16, 0, 10);
//    if (circles.empty()) {
//        cout << "未检测到圆形" << endl;
//        return 0;
//    }
//
//    //筛选出存在其它两圆心与该圆心距离为半径的点
//    Vec3f LeftTop;
//    Vec3f RightTop;
//    Vec3f LeftBottom;
//    float distance;
//    for (int i = 0; i < circles.size(); i++) {
//        //动态设置边长
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
//    //求取俩向量夹角，即为旋转角度（顺时针方向为正）
//    float theta = atan2(LeftTop[0] - LeftTop[0], (LeftTop[1] - 1) - LeftTop[1]) - atan2(LeftTop[0] - LeftBottom[0], LeftTop[1] - LeftBottom[1]);
//    if (theta > CV_PI)
//        theta -= 2 * CV_PI;
//    if (theta < -CV_PI)
//        theta += 2 * CV_PI;
//    theta = theta * 180.0 / CV_PI;
//    cout << "theta:" << theta << endl;
//
//    //旋转矫正,theta参数为负表示朝顺时针方向旋转
//    Mat correctImage;
//    Point center(srcImage.cols / 2, srcImage.rows / 2);
//    Mat rotMat = getRotationMatrix2D(center, theta, 1);
//    warpAffine(srcImage, correctImage, rotMat, Size(srcImage.size()));
//
//    cout << center.x << ";" << center.y << endl;
//    cout << correctImage.cols / 2 << ";" << correctImage.rows / 2 << endl;
//
//    //放射变换后定位点对应的新坐标，theta旋转角度为逆时针
//    for (int i = 0; i < circles.size(); i++) {
//        //cout << circles[i][0]<<","<< circles[i][1] << "," << circles[i][2]<< endl;
//        circles[i] = getPointAffinedPos(circles[i], center, theta);
//        //cout << circles[i][0] << "," << circles[i][1] << "," << circles[i][2] << endl;
//    }
//    //可以在circles里面除去三个定位点
//    LeftTop = getPointAffinedPos(LeftTop, center, theta);
//    RightTop = getPointAffinedPos(RightTop, center, theta);
//    LeftBottom = getPointAffinedPos(LeftBottom, center, theta);
//
//    //绘制圆圈
//    vector<Vec3f> drawCircles;
//    drawCircles.push_back(LeftTop);
//    drawCircles.push_back(RightTop);
//    drawCircles.push_back(LeftBottom);
//    for (int i = 0; i < drawCircles.size(); i++) {
//        Point center(cvRound(drawCircles[i][0]), cvRound(drawCircles[i][1]));
//        int radius = cvRound(drawCircles[i][2]);
//        //绘制圆心
//        circle(correctImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
//        //绘制圆轮廓
//        circle(correctImage, center, radius, Scalar(155, 50, 255), 2, 8, 0);
//    }
//
//    namedWindow("processimg", WINDOW_NORMAL);
//    imshow("processimg", correctImage);
//    waitKey(0);
//
//
//    //x和y轴分量计算
//    double dx = (RightTop[0] - LeftTop[0]) / (n - 1);
//    double dy = (LeftBottom[1] - LeftTop[1]) / (n - 1);
//
//    //按位检测圆是否存在
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
//    //根据位值计算序号
//    for (int i = 0; i < bitnum; i++) {
//        id += bits[i] * pow(2, i);
//    }
//
//    cout << "识别到ID:" << id << endl;
//
//    return 0;
//}
