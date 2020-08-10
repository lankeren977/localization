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
//    //读取图片
//    srcImage = imread("C:/Users/a/Desktop/photo2.jpg");
//
//    //转化为灰度图
//    cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);
//
//    //计算相机位置
//    Point cameraCenter(grayImage.cols / 2, grayImage.rows / 2);
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
//    //计算图像中相机距离左上顶点的距离
//    double imgDis = sqrt(pow((cameraCenter.x - LeftTop[1]), 2) + pow((cameraCenter.y - LeftTop[0]), 2));
//
//    //画出两向量线段
//    Point pt0(cvRound(LeftTop[0]), cvRound(LeftTop[1]));
//    Point pt1(cvRound(LeftBottom[0]), cvRound(LeftBottom[1]));
//    line(srcImage, pt0, pt1, Scalar(0, 0, 255), 2, 8, 0);
//    line(srcImage, pt0, cameraCenter, Scalar(0, 0, 255), 2, 8, 0);
//
//    //计算相机较landmark的方向
//    float theta = atan2(LeftTop[0] - LeftBottom[0], LeftTop[1] - LeftBottom[1]) - atan2(cameraCenter.x - LeftTop[0], cameraCenter.y - LeftTop[1]);
//    if (theta > CV_PI)
//        theta -= 2 * CV_PI;
//    if (theta < -CV_PI)
//        theta += 2 * CV_PI;
//    theta = theta * 180.0 / CV_PI;
//
//    //全部转为顺时针方向
//    if (theta < 0) {
//        theta += 360.0;
//    }
//
//    cout << "theta:" << theta << endl;
//
//    theta = theta / 180.0 * CV_PI;
//    //计算相对于landmark的坐标
//    cameraX = imgDis * sin(theta);
//    cameraY = imgDis * cos(theta);
//
//    //换算为实际位置
//    cameraRealX = cameraX * realLength / srcImage.cols;
//    cameraRealY = cameraY * realLength / srcImage.cols;
//
//    cout << "相机图像坐标:(" << cameraX << "," << cameraY << ")" << endl;
//    cout << "相机实际坐标:(" << cameraRealX << "," << cameraRealY << ")" << endl;
//
//    imshow("processImg", srcImage);
//    waitKey(0);
//
//    return 0;
//}
