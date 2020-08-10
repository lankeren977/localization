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
//    cap.set(3, 1920);//宽度 
//    cap.set(4, 1080);//高度
//    if (!cap.isOpened())
//    {
//        cout << "摄像头未成功打开" << endl;
//    }
//    Mat resultImg;
//    //读取摄像头中的帧
//
//    clock_t startTime, endTime;
//    for (; ; )
//    {
//        startTime = clock();//计时开始
//        int id = 0;
//        int bits[bitnum];
//        Mat srcImage;
//        Mat grayImage;
//        Mat correctImage;
//
//        cap >> srcImage;
//        //判断是否读取到
//        if (srcImage.empty())
//        {
//            break;
//        }
//
//        imwrite("C:/Users/a/Desktop/photo10.jpg", srcImage);
//
//        //转化为灰度图
//        cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);
//
//        //计算相机位置
//        Point cameraCenter(grayImage.cols / 2, grayImage.rows / 2);
//
//        //定位所有圆心并找出定位顶点
//        vector<Vec3f> circles;
//        HoughCircles(grayImage, circles, HOUGH_GRADIENT, 1, 10, 200, 15, 0, 10);
//
//        if (circles.empty()) {
//            cout << "未检测到圆形" << endl;
//        }
//        else {
//            cout << "检测到圆数目:" << circles.size() << endl;
//
//            //筛选出存在其它两圆心与该圆心距离为半径的点
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
//                    if (LeftTop[0] < RightTop[0] && LeftTop[0] <= LeftBottom[0]) { //0<=angle<90逆时针旋转
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
//            //绘制定位线,0和2向量决定旋转角
//            Point pt0(cvRound(LeftTop[0]), cvRound(LeftTop[1]));
//            Point pt1(cvRound(RightTop[0]), cvRound(RightTop[1]));
//            Point pt2(cvRound(LeftBottom[0]), cvRound(LeftBottom[1]));
//
//            //计算图像中相机距离左上顶点的距离
//            double imgDis = sqrt(pow((cameraCenter.x - LeftTop[1]), 2) + pow((cameraCenter.y - LeftTop[0]), 2));
//
//            //计算相机较landmark的方向
//            float theta2 = atan2(LeftTop[0] - LeftBottom[0], LeftTop[1] - LeftBottom[1]) - atan2(cameraCenter.x - LeftTop[0], cameraCenter.y - LeftTop[1]);
//            if (theta2 > CV_PI)
//                theta2 -= 2 * CV_PI;
//            if (theta2 < -CV_PI)
//                theta2 += 2 * CV_PI;
//            theta2 = theta2 * 180.0 / CV_PI;
//
//            //全部转为顺时针方向
//            if (theta2 < 0) {
//                theta2 += 360.0;
//            }
//
//            cout << "theta2:" << theta2 << endl;
//
//            theta2 = theta2 / 180.0 * CV_PI;
//            //计算相对于landmark的坐标
//            cameraX = imgDis * sin(theta2);
//            cameraY = imgDis * cos(theta2);
//
//            //换算为实际位置
//            cameraRealX = cameraX * factor;
//            cameraRealY = cameraY * factor;
//
//            cout << "相机图像坐标:(" << cameraX << "," << cameraY << ")" << endl;
//            cout << "相机实际坐标:(" << cameraRealX << "," << cameraRealY << ")" << endl;
//
//            //为竖直向量取点
//            Point pt3(pt0.x, pt0.y - 1);
//
//            //求取俩向量夹角，即为旋转角度（顺时针方向为正）
//            float theta = atan2(pt3.x - pt0.x, pt3.y - pt0.y) - atan2(pt0.x - pt2.x, pt0.y - pt2.y);
//            if (theta > CV_PI)
//                theta -= 2 * CV_PI;
//            if (theta < -CV_PI)
//                theta += 2 * CV_PI;
//            theta = theta * 180.0 / CV_PI;
//            cout << "theta:" << theta << endl;
//
//            //旋转矫正
//            Point center(srcImage.cols / 2, srcImage.rows / 2);
//            Mat rotMat = getRotationMatrix2D(center, theta, 1);
//            warpAffine(srcImage, correctImage, rotMat, Size(srcImage.size()));
//
//            //放射变换后定位点对应的新坐标，theta旋转角度为逆时针
//            for (int i = 0; i < circles.size(); i++) {
//                circles[i] = getPointAffinedPos(circles[i], center, theta);
//            }
//            //可以在circles里面除去三个定位点
//            LeftTop = getPointAffinedPos(LeftTop, center, theta);
//            RightTop = getPointAffinedPos(RightTop, center, theta);
//            LeftBottom = getPointAffinedPos(LeftBottom, center, theta);
//
//            //x和y轴分量计算
//            double dx = (RightTop[0] - LeftTop[0]) / (n - 1);
//            double dy = (LeftBottom[1] - LeftTop[1]) / (n - 1);
//
//            //按位检测圆是否存在
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
//            //根据位值计算序号
//            for (int i = 0; i < bitnum; i++) {
//                id += bits[i] * pow(2, i);
//            }
//
//            cout << "识别到ID:" << id << endl;
//
//            endTime = clock();//计时结束
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
