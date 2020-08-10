//#include <iostream>
//#include <vector>
//#include <math.h>
//#include <opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/highgui/highgui.hpp>
//
//using namespace std;
//using namespace cv;
//
//Mat srcImage;
//Mat grayImage;
//Mat binaryImage;
//Mat cannyImage;
////const float circleRadius = 28.0;
////const float minSideLength = 190; //200
////const float maxSideLength = 220; //212
//float circleRadius = 0;
//float minSideLength = 0;
//float maxSideLength = 0;
//
//int main() {
//
//    //读取图片
//    srcImage = imread("C:/Users/a/Desktop/pics/pics/large.jpg");
//
//    //转化为灰度图
//    cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);
//
//    //转化为二值图像
//    //threshold(scrImage, binaryImage,128, 255, THRESH_BINARY);
//
//    //进行边缘检测（从RGB图像直接转化为二值图像）
//    //Canny(scrImage,cannyImage,128, 200, 3);
//
//    //定位所有圆心并找出定位顶点
//    vector<Vec3f> circles;
//    HoughCircles(grayImage, circles, HOUGH_GRADIENT, 2, 10, 200, 100, 0, 50);
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
//        //动态设置边长
//        circleRadius = circles[i][2];
//        minSideLength = circleRadius * 6.5;
//        maxSideLength = circleRadius * 7.5;
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
//    //绘制定位线,0和2向量决定旋转角
//    Point pt0(cvRound(LeftTop[0]), cvRound(LeftTop[1]));
//    Point pt1(cvRound(RightTop[0]), cvRound(RightTop[1]));
//    Point pt2(cvRound(LeftBottom[0]), cvRound(LeftBottom[1]));
//    line(srcImage, pt0, pt1, Scalar(0, 0, 255), 2, 8, 0);
//    line(srcImage, pt0, pt2, Scalar(0, 0, 255), 2, 8, 0);
//
//    //为竖直向量取点
//    Point pt3(pt0.x, pt0.y - 1);
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
//    //找出xy轴拥有极值的圆心
//    //    vector<Vec3f> verTexs;
//    //    Vec3f maxx;
//    //    Vec3f maxy;
//    //    Vec3f minx;
//    //    Vec3f miny;
//    //    for(int i=0; i<circles.size(); i++){
//    //        if(i==0){
//    //            maxx = maxy = minx = miny = circles[0];
//    //        }else{
//    //            if(maxx[0]<circles[i][0]){
//    //                maxx = circles[i];
//    //            }
//    //            if(maxy[0]<circles[i][1]){
//    //                maxy = circles[i];
//    //            }
//    //            if(minx[0]>circles[i][0]){
//    //                minx = circles[i];
//    //            }
//    //            if(miny[0]>circles[i][1]){
//    //                miny = circles[i];
//    //            }
//    //        }
//    //    }
//    //    double d1 = pow((maxx[0] - minx[0]),2) + pow((maxx[1] - minx[1]),2);
//    //    double d2 = pow((maxy[0] - miny[0]),2) + pow((maxy[1] - miny[1]),2);
//
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
//    namedWindow("processImg", WINDOW_NORMAL);
//    imshow("processImg", correctImage);
//    //    imshow("correctImg", correctImage);
//    waitKey(0);
//
//    return 0;
//}
