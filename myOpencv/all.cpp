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
//double factor1 = 0;//左上左下
//double factor2 = 0;//左上右上
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
//    //读取图片
//    srcImage = imread("C:/Users/a/Desktop/photo5.jpg");
//
//    //转化为灰度图
//    cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);
//
//    //计算相机位置
//    Point cameraCenter(grayImage.cols / 2, grayImage.rows / 2);
//
//    //定位所有圆心并找出定位顶点
//    vector<Vec3f> circles;
//    HoughCircles(grayImage, circles, HOUGH_GRADIENT, 1, 10, 200, 15, 0, 10);
//    if (circles.empty()) {
//        cout << "未检测到圆形" << endl;
//        return 0;
//    }
//    cout << "检测到圆数目:" << circles.size() << endl;
//
//    //筛选出存在其它两圆心与该圆心距离为半径的点
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
//            if (LeftTop[1] <= RightTop[1] && LeftTop[1] <= LeftBottom[1]) { //上，0<=angle<90逆时针旋转
//                if (LeftBottom[0] > RightTop[0]) {
//                    flag = false;
//                }
//            }
//            else if (LeftTop[0] >= RightTop[0] && LeftTop[0] >= LeftBottom[0]) {//右90<=angle<180
//                if (RightTop[1] < LeftBottom[1]) {
//                    flag = false;
//                }
//            }
//            else if (LeftTop[1] >= RightTop[1] && LeftTop[1] >= LeftBottom[1]) {//下180<=angle<270
//                if (RightTop[0] > LeftBottom[0]) {
//                    flag = false;
//                }
//            }
//            else if (LeftTop[0] <= RightTop[0] && LeftTop[0] <= LeftBottom[0]) {//左270<=angle<360
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
//    //矫正前
//    float vertical = atan2(RightTop[0] - LeftTop[0], RightTop[1] - LeftTop[1]) - atan2(LeftBottom[0] - LeftTop[0], LeftBottom[1] - LeftTop[1]);
//    vertical = vertical * 180.0 / CV_PI;
//    cout << "矫正前三点夹角：" << vertical << endl;
//
//    //整点偏转下矫正一下三点坐标
//    if (LeftTop[0] == LeftBottom[0]) {
//        RightTop[1] = LeftTop[1];
//        cout << "整点偏转矫正1" << endl;
//    }
//    else if (LeftTop[1] == LeftBottom[1]) {
//        RightTop[0] = LeftTop[0];
//        cout << "整点偏转矫正2" << endl;
//    }
//    else if (LeftTop[0] == RightTop[0]) {
//        LeftBottom[1] = LeftTop[1];
//        cout << "整点偏转矫正3" << endl;
//    }
//    else if (LeftTop[1] == RightTop[1]) {
//        LeftBottom[0] = LeftTop[0];
//        cout << "整点偏转矫正4" << endl;
//    }
//
//    //矫正后
//    float vertical2 = atan2(RightTop[0] - LeftTop[0], RightTop[1] - LeftTop[1]) - atan2(LeftBottom[0] - LeftTop[0], LeftBottom[1] - LeftTop[1]);
//    vertical2 = vertical2 * 180.0 / CV_PI;
//    cout << "矫正后三点夹角：" << vertical2 << endl;
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
//        circle(srcImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
//        //绘制圆轮廓
//        circle(srcImage, center, radius, Scalar(155, 50, 255), 2, 8, 0);
//    }
//
//    //绘制定位线,0和2向量决定旋转角
//    Point pt0(cvRound(LeftTop[0]), cvRound(LeftTop[1]));
//    Point pt1(cvRound(RightTop[0]), cvRound(RightTop[1]));
//    Point pt2(cvRound(LeftBottom[0]), cvRound(LeftBottom[1]));
//    line(srcImage, pt0, pt1, Scalar(0, 0, 255), 2, 8, 0);
//    line(srcImage, pt0, pt2, Scalar(0, 0, 255), 2, 8, 0);
//
//    //计算图像中相机距离左上顶点的距离
//    double imgDis = sqrt(pow((cameraCenter.x - LeftTop[0]), 2) + pow((cameraCenter.y - LeftTop[1]), 2));
//
//    //计算相机较landmark的方向
//    float theta2 = atan2(LeftTop[0] - LeftBottom[0], LeftTop[1] - LeftBottom[1]) - atan2(cameraCenter.x - LeftTop[0], cameraCenter.y - LeftTop[1]);
//    if (theta2 > CV_PI)
//        theta2 -= 2 * CV_PI;
//    if (theta2 < -CV_PI)
//        theta2 += 2 * CV_PI;
//    theta2 = theta2 * 180.0 / CV_PI;
//    //全部转为顺时针方向
//    if (theta2 < 0) {
//        theta2 += 360.0;
//    }
//
//    cout << "theta2:" << theta2 << endl;
//    theta2 = theta2 / 180.0 * CV_PI;
//    //计算相对于landmark的坐标
//    cameraX = imgDis * sin(theta2);
//    cameraY = imgDis * cos(theta2);
//
//    //计算比例因子
//    double disLeft = sqrt(pow(LeftTop[0] - LeftBottom[0], 2) + pow(LeftTop[1] - LeftBottom[1], 2));
//    double disTop = sqrt(pow(LeftTop[0] - RightTop[0], 2) + pow(LeftTop[1] - RightTop[1], 2));
//    cout << "x轴长度：" << disTop << " y轴长度：" << disLeft << endl;
//    factor1 = 24 / disTop;
//    factor2 = 24 / disLeft;
//
//    //换算为实际位置
//    cameraRealX = cameraX * factor1;
//    cameraRealY = cameraY * factor2;
//
//    cout << "图像中心点距离地标左上点距离：" << imgDis << "pixels," << imgDis * (factor1 + factor2) / 2 << "cm" << endl;
//    cout << "相机图像坐标:(" << cameraX << "," << cameraY << ")" << endl;
//    cout << "相机实际坐标:(" << cameraRealX << "," << cameraRealY << ")" << endl;
//
//    //为竖直向量取点
//    Point pt3(pt0.x, pt0.y - 1);
//
//    //求取俩向量夹角，即为旋转角度（顺时针方向为正）
//    float theta = atan2(pt3.x - pt0.x, pt3.y - pt0.y) - atan2(pt0.x - pt2.x, pt0.y - pt2.y);
//    if (theta > CV_PI)
//        theta -= 2 * CV_PI;
//    if (theta < -CV_PI)
//        theta += 2 * CV_PI;
//    theta = theta * 180.0 / CV_PI;
//    cout << "theta:" << theta << endl;
//
//    //旋转矫正
//    Mat correctImage;
//    Point center(srcImage.cols / 2, srcImage.rows / 2);
//    Mat rotMat = getRotationMatrix2D(center, theta, 1);
//    warpAffine(srcImage, correctImage, rotMat, Size(srcImage.size()));
//
//    //放射变换后定位点对应的新坐标，theta旋转角度为逆时针
//    for (int i = 0; i < circles.size(); i++) {
//        circles[i] = getPointAffinedPos(circles[i], center, theta);
//    }
//    //可以在circles里面除去三个定位点
//    LeftTop = getPointAffinedPos(LeftTop, center, theta);
//    RightTop = getPointAffinedPos(RightTop, center, theta);
//    LeftBottom = getPointAffinedPos(LeftBottom, center, theta);
//
//    //绘制圆圈
//    vector<Vec3f> drawCircles2;
//    drawCircles2.push_back(LeftTop);
//    drawCircles2.push_back(RightTop);
//    drawCircles2.push_back(LeftBottom);
//    for (int i = 0; i < drawCircles2.size(); i++) {
//        Point center(cvRound(drawCircles2[i][0]), cvRound(drawCircles2[i][1]));
//        int radius = cvRound(drawCircles2[i][2]);
//        //绘制圆心
//        circle(correctImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
//        //绘制圆轮廓
//        circle(correctImage, center, radius, Scalar(155, 50, 255), 2, 8, 0);
//    }
//
//    //x和y轴分量计算
//    double dx = (RightTop[0] - LeftTop[0]) / (n - 1);
//    double dy = (LeftBottom[1] - LeftTop[1]) / (n - 1);
//
//    //按位检测圆是否存在
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
//    //判断二进制为1的数目和圆圈数目是否相等
//    if (circlesNum != bitNum) {
//        cout << "ID识别异常，遗漏圆圈" << endl;
//    }
//
//    //根据位值计算序号
//    for (int i = 0; i < bitnum; i++) {
//        id += bits[i] * pow(2, i);
//    }
//
//    cout << "识别到ID:" << id << endl;
//
//    //画出两向量线段
//    line(srcImage, pt0, pt2, Scalar(0, 0, 255), 2, 8, 0);
//    line(srcImage, pt0, cameraCenter, Scalar(0, 0, 255), 2, 8, 0);
//
//    //imshow("srcImage", srcImage);
//    //imshow("correctImage", correctImage);
//    //waitKey(0);
//
//    return 0;
//}
