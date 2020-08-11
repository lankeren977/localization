#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//Mat srcImage;

const double realLength = 550.0; //24cm=90pixels
const double realWidth = 309.4;
const double factor = 0.26667;

float circleRadius = 0;
const float minSideLength = 89;
const float maxSideLength = 93;

const int n = 4;
const double e = 5;
const int bitnum = n * n - 4;


Vec3f getPointAffinedPos(Vec3f& src, const Point center, double angle)
{
    angle = angle * CV_PI / 180;
    int x = src[0] - center.x;
    int y = src[1] - center.y;
    src[0] = cvRound(x * cos(angle) + y * sin(angle) + center.x);
    src[1] = cvRound(-x * sin(angle) + y * cos(angle) + center.y);
    return src;
}

int main() {

    VideoCapture cap(0);
    cap.set(3, 1920);//宽度 
    cap.set(4, 1080);//高度
    if (!cap.isOpened())
    {
        cout << "摄像头未成功打开" << endl;
    }
    Mat resultImg;

    //读取摄像头中的帧
    clock_t startTime, endTime;
    for (; ; )
    {
        startTime = clock();//计时开始
        int bits[bitnum];
        int circleSize = 0;
        Mat srcImage;
        Mat grayImage;
        Mat binaryImage;
        Mat correctImage;

        cap >> srcImage;
        //判断是否读取到
        if (srcImage.empty())
        {
            break;
        }

        //转化为灰度图
        cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);

        //计算相机中心坐标
        Point cameraCenter(grayImage.cols / 2, grayImage.rows / 2);

        //转化二值图像
        threshold(grayImage, binaryImage, 230, 255, THRESH_BINARY);

        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(binaryImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());

        vector<Rect> boundRect(contours.size());  //定义外接矩形集合
        vector<Rect> targetRects;//定义目标矩阵集合
        for (int i = 0; i < contours.size(); i++)
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            if (boundRect[i].width < 20 && boundRect[i].width > 8 && boundRect[i].height < 20 && boundRect[i].height > 8) {
                targetRects.push_back(boundRect[i]);
                rectangle(srcImage, Point(boundRect[i].x, boundRect[i].y), Point(boundRect[i].x + boundRect[i].width, boundRect[i].y + boundRect[i].height), Scalar(0, 255, 0), 1, 8);
            }
        }

        int maxDisValue = 160;
        int minDisValue = 15;
        vector<vector<Rect>> ROIs;//定义ROI集合
        while (targetRects.size() != 0) {
            Rect temp = targetRects[0];
            vector<Rect> tempRects;
            tempRects.push_back(temp);
            targetRects.erase(targetRects.begin());
            for (int m = 0; m < targetRects.size(); m++) {
                double distance = sqrt(pow(targetRects[m].x - temp.x, 2) + pow(targetRects[m].y - temp.y, 2));
                if (distance < maxDisValue && distance > minDisValue) {
                    tempRects.push_back(targetRects[m]);
                    targetRects.erase(targetRects.begin() + m);
                    m--;
                }
            }
            if (tempRects.size() > 3) { //超过3点才可能是路标
                ROIs.push_back(tempRects);
            }
        }

        if (ROIs.size() > 0) {
            for (int p = 0; p < ROIs.size(); p++) {

                vector<Rect> landmark = ROIs[p];
                int minX = 2000;
                int minY = 2000;
                int maxX = 0;
                int maxY = 0;
                for (int q = 0; q < landmark.size(); q++) {
                    if (landmark[q].x < minX) {
                        minX = landmark[q].x;
                    }
                    if (landmark[q].y < minY) {
                        minY = landmark[q].y;
                    }
                    if (landmark[q].x > maxX) {
                        maxX = landmark[q].x;
                    }
                    if (landmark[q].y > maxY) {
                        maxY = landmark[q].y;
                    }
                }
                minX -= 10;
                minY -= 10;
                maxX = maxX - minX + 20;
                maxY = maxY - minY + 20;
                Rect roiRect = Rect(minX, minY, maxX, maxY);
                rectangle(srcImage, Point(roiRect.x, roiRect.y), Point(roiRect.x + roiRect.width, roiRect.y + roiRect.height), Scalar(0, 0, 255), 1, 8);
                Mat ROIimage = grayImage(roiRect);

                //定位所有圆心并找出定位顶点
                vector<Vec3f> circles;
                HoughCircles(ROIimage, circles, HOUGH_GRADIENT, 1, 10, 200, 15, 0, 10);
                if (circles.empty()) {
                    cout << "未检测到圆形" << endl;
                    continue;
                }

                for (int i = 0; i < circles.size(); i++) {
                    circles[i][0] += roiRect.x;
                    circles[i][1] += roiRect.y;
                }


                //对每个路标进行检测
                //筛选出存在其它两圆心与该圆心距离为半径的点
                Vec3f LeftTop;
                Vec3f RightTop;
                Vec3f LeftBottom;
                float distance;
                int i = 0;
                int j = 0;
                bool loop = true;
                for (i = 0; i < circles.size(); i++) {
                    int num = 0;
                    for (j = 0; j < circles.size(); j++) {
                        distance = sqrt(pow(circles[i][0] - circles[j][0], 2) + pow(circles[i][1] - circles[j][1], 2));
                        if (distance <= maxSideLength && distance >= minSideLength) {
                            num++;
                            if (num == 1) {
                                RightTop = circles[j];
                            }
                            if (num == 2) {
                                LeftBottom = circles[j];
                                break;
                            }
                        }
                    }
                    if (num == 2) {
                        LeftTop = circles[i];
                        bool flag = true;
                        if (LeftTop[1] <= RightTop[1] && LeftTop[1] <= LeftBottom[1]) { //上，0<=angle<90逆时针旋转
                            if (LeftBottom[0] > RightTop[0]) {
                                flag = false;
                            }
                        }
                        else if (LeftTop[0] >= RightTop[0] && LeftTop[0] >= LeftBottom[0]) {//右90<=angle<180
                            if (RightTop[1] < LeftBottom[1]) {
                                flag = false;
                            }
                        }
                        else if (LeftTop[1] >= RightTop[1] && LeftTop[1] >= LeftBottom[1]) {//下180<=angle<270
                            if (RightTop[0] > LeftBottom[0]) {
                                flag = false;
                            }
                        }
                        else if (LeftTop[0] <= RightTop[0] && LeftTop[0] <= LeftBottom[0]) {//左270<=angle<360
                            if (RightTop[1] > LeftBottom[1]) {
                                flag = false;
                            }
                        }
                        else {
                            loop = false;
                            break;
                        }
                        if (!flag) {
                            Vec3f temp = RightTop;
                            RightTop = LeftBottom;
                            LeftBottom = temp;
                        }
                        break;
                    }
                }

                if (!loop) {
                    cout << "定位点坐标异常" << endl;
                    continue;
                }

                //矫正前
                float vertical = atan2(RightTop[0] - LeftTop[0], RightTop[1] - LeftTop[1]) - atan2(LeftBottom[0] - LeftTop[0], LeftBottom[1] - LeftTop[1]);
                vertical = vertical * 180.0 / CV_PI;


                //整点偏转下矫正一下三点坐标
                if (LeftTop[0] == LeftBottom[0]) {
                    RightTop[1] = LeftTop[1];
                    cout << "整点偏转矫正1" << endl;
                }
                else if (LeftTop[1] == LeftBottom[1]) {
                    RightTop[0] = LeftTop[0];
                    cout << "整点偏转矫正2" << endl;
                }
                else if (LeftTop[0] == RightTop[0]) {
                    LeftBottom[1] = LeftTop[1];
                    cout << "整点偏转矫正3" << endl;
                }
                else if (LeftTop[1] == RightTop[1]) {
                    LeftBottom[0] = LeftTop[0];
                    cout << "整点偏转矫正4" << endl;
                }

                //矫正后
                float vertical2 = atan2(RightTop[0] - LeftTop[0], RightTop[1] - LeftTop[1]) - atan2(LeftBottom[0] - LeftTop[0], LeftBottom[1] - LeftTop[1]);
                vertical2 = vertical2 * 180.0 / CV_PI;


                //绘制定位线,0和2向量决定旋转角
                Point pt0(LeftTop[0], LeftTop[1]);
                Point pt1(RightTop[0], RightTop[1]);
                Point pt2(LeftBottom[0], LeftBottom[1]);

                //计算图像中相机距离左上顶点的距离
                double imgDis = sqrt(pow((cameraCenter.x - LeftTop[0]), 2) + pow((cameraCenter.y - LeftTop[1]), 2));

                //计算相机较landmark的方向
                float theta2 = atan2(LeftTop[0] - LeftBottom[0], LeftTop[1] - LeftBottom[1]) - atan2(cameraCenter.x - LeftTop[0], cameraCenter.y - LeftTop[1]);
                if (theta2 > CV_PI)
                    theta2 -= 2 * CV_PI;
                if (theta2 < -CV_PI)
                    theta2 += 2 * CV_PI;

                //计算相对于landmark的坐标
                double cameraX = imgDis * sin(theta2);
                double cameraY = imgDis * cos(theta2);

                //全部转为顺时针方向
                theta2 = theta2 * 180.0 / CV_PI;
                if (theta2 < 0) {
                    theta2 += 360.0;
                }
                //cout << "相机中心与路标坐标系的夹角:" << theta2 << endl;

                //计算比例因子
                double disLeft = sqrt(pow(LeftTop[0] - LeftBottom[0], 2) + pow(LeftTop[1] - LeftBottom[1], 2));
                double disTop = sqrt(pow(LeftTop[0] - RightTop[0], 2) + pow(LeftTop[1] - RightTop[1], 2));
                //cout << "x轴长度：" << disTop << " pixels， y轴长度：" << disLeft << " pixels" << endl;
                double factor1 = 24 / disTop;
                double factor2 = 24 / disLeft;

                //换算为实际位置
                double cameraRealX = cameraX * factor1;
                double cameraRealY = cameraY * factor2;

                //为竖直向量取点
                Point pt3(pt0.x, pt0.y - 1);

                //求取俩向量夹角，即为旋转角度（顺时针方向为正）
                float theta = atan2(pt3.x - pt0.x, pt3.y - pt0.y) - atan2(pt0.x - pt2.x, pt0.y - pt2.y);
                if (theta > CV_PI)
                    theta -= 2 * CV_PI;
                if (theta < -CV_PI)
                    theta += 2 * CV_PI;
                theta = theta * 180.0 / CV_PI;
                if (theta < 0) {
                    theta += 360.0;
                }

                //放射变换后定位点对应的新坐标，theta旋转角度为逆时针
                for (int i = 0; i < circles.size(); i++) {
                    circles[i] = getPointAffinedPos(circles[i], cameraCenter, theta);
                }
                //可以在circles里面除去三个定位点
                LeftTop = getPointAffinedPos(LeftTop, cameraCenter, theta);
                RightTop = getPointAffinedPos(RightTop, cameraCenter, theta);
                LeftBottom = getPointAffinedPos(LeftBottom, cameraCenter, theta);

                //x和y轴分量计算
                double dx = (RightTop[0] - LeftTop[0]) / (n - 1);
                double dy = (LeftBottom[1] - LeftTop[1]) / (n - 1);

                //按位检测圆是否存在
                int seq = 0;
                int circlesNum = circles.size() - 3;
                int bitNum = 0;
                for (int i = 1; i <= n; i++) {
                    for (int j = 1; j <= n; j++) {
                        if ((i == 1 && j == 1) || (i == 1 && j == n) || (i == n && j == 1) || (i == n && j == n)) {
                            continue;
                        }
                        double bitx = LeftTop[0] + (j - 1) * dx;
                        double bity = LeftTop[1] + (i - 1) * dy;
                        bits[seq] = 0;
                        for (int k = 0; k < circles.size(); k++) {
                            if (fabs(circles[k][0] - bitx) <= e && fabs(circles[k][1] - bity) <= e) {
                                bits[seq] = 1;
                                bitNum++;
                                circles.erase(circles.begin() + k);
                                break;
                            }
                        }
                        seq++;
                    }
                }

                //判断二进制为1的数目和圆圈数目是否相等
                if (circlesNum != bitNum) {
                    cout << "ID识别异常，遗漏圆圈" << endl;
                }

                int id = 0;
                //根据位值计算序号
                for (int i = 0; i < bitnum; i++) {
                    id += bits[i] * pow(2, i);
                }

                endTime = clock();//计时结束
                cout << "The run time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
                cout << "检测到圆数目:" << circles.size() << endl;
                cout << "矫正前三点夹角：" << vertical << endl;
                cout << "矫正后三点夹角：" << vertical2 << endl;
                cout << "相机中心点与路标左上点像素距离：" << imgDis << "pixels, 实际距离：" << imgDis * (factor1 + factor2) / 2 << "cm" << endl;
                cout << "相机图像坐标:(" << cameraX << "," << cameraY << ")" << endl;
                cout << "相机实际坐标:(" << cameraRealX << "," << cameraRealY << ")" << endl;
                cout << "图像旋转角度(顺时针):" << theta << endl;
                cout << "识别到ID:" << id << endl;
                cout << "========================================================" << endl;
            }
            imshow("camera", srcImage);
            if (waitKey(300) >= 0)
            {
                break;
            }
        }else {
            cout << "未检测到目标ROI" << endl;
        }
    }


    return 0;
}
