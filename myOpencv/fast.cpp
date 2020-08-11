//#include <iostream>
//#include <string>
//#include <opencv2/opencv.hpp>
//
//using namespace std;
//using namespace cv;
//
//Mat srcImage;
//Mat grayImage;
//Mat binaryImage;
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
//    clock_t startTime, endTime;
//
//    //读取图片
//    srcImage = imread("C:/Users/a/Desktop/mixthree.png");
//
//    startTime = clock();//计时开始
//
//    //转化为灰度图
//    cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);
//
//    //Mat element = getStructuringElement(MORPH_RECT, Size(30, 30));
//    //dilate(srcImage, binaryImage, element);
//
//
//    //转化二值图像
//    threshold(grayImage, binaryImage, 200, 255, THRESH_BINARY);
//
//
//    //adaptiveThreshold(grayImage, binaryImage, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 41, 17);
//
//
//
//    //Canny(binaryImage, binaryImage, 90, 100, 3);
//    //vector<vector<Point>> contours;
//    //vector<Vec4i> hierarchy;
//    //findContours(binaryImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());
//
//    //vector<Rect> boundRect(contours.size());  //定义外接矩形集合
//    //vector<Rect> targetRects;//定义目标矩阵集合
//    //for (int i = 0; i < contours.size(); i++)
//    //{
//    //    boundRect[i] = boundingRect(Mat(contours[i]));
//    //    if (boundRect[i].width < 20 && boundRect[i].width > 8 && boundRect[i].height < 20 && boundRect[i].height > 8) {
//    //        targetRects.push_back(boundRect[i]);
//    //        rectangle(srcImage, Point(boundRect[i].x, boundRect[i].y), Point(boundRect[i].x + boundRect[i].width, boundRect[i].y + boundRect[i].height), Scalar(0, 255, 0), 1, 8);
//    //    }
//    //}
//
//    //int maxDisValue = 160;
//    //int minDisValue = 15;
//    //vector<vector<Rect>> ROIs;//定义ROI集合
//    //while (targetRects.size() != 0) {
//    //    Rect temp = targetRects[0];
//    //    vector<Rect> tempRects;
//    //    tempRects.push_back(temp);
//    //    targetRects.erase(targetRects.begin());
//    //    for (int i = 0; i < targetRects.size(); i++) {
//    //        double distance = sqrt(pow(targetRects[i].x - temp.x, 2) + pow(targetRects[i].y - temp.y, 2));
//    //        if (distance < maxDisValue && distance > minDisValue) {
//    //            tempRects.push_back(targetRects[i]);
//    //            targetRects.erase(targetRects.begin() + i);
//    //            i--;
//    //        }
//    //    }
//    //    if (tempRects.size() > 3) { //超过3点才可能是路标
//    //        ROIs.push_back(tempRects);
//    //    }
//    //}
//
//    //if (ROIs.size() < 1) {
//    //    cout << "未检测到目标ROI" << endl;
//    //}
//
//    ////对每个路标进行检测
//    //for (int i = 0; i < ROIs.size(); i++) {
//    //    vector<Rect> landmark = ROIs[i];
//    //    int minX = 2000;
//    //    int minY = 2000;
//    //    int maxX = 0;
//    //    int maxY = 0;
//    //    for (int i = 0; i < landmark.size(); i++) {
//    //        if (landmark[i].x < minX) {
//    //            minX = landmark[i].x;
//    //        }
//    //        if (landmark[i].y < minY) {
//    //            minY = landmark[i].y;
//    //        }
//    //        if (landmark[i].x > maxX) {
//    //            maxX = landmark[i].x;
//    //        }
//    //        if (landmark[i].y > maxY) {
//    //            maxY = landmark[i].y;
//    //        }
//    //    }
//    //    minX -= 10;
//    //    minY -= 10;
//    //    maxX = maxX - minX + 20;
//    //    maxY = maxY - minY + 20;
//    //    Rect roiRect = Rect(minX, minY, maxX, maxY);
//    //    rectangle(srcImage, Point(roiRect.x, roiRect.y), Point(roiRect.x + roiRect.width, roiRect.y + roiRect.height), Scalar(0, 0, 255), 1, 8);
//    //    Mat ROIimage = grayImage(roiRect);
//
//    //    //定位所有圆心并找出定位顶点
//    //    vector<Vec3f> circles;
//    //    HoughCircles(ROIimage, circles, HOUGH_GRADIENT, 1, 10, 200, 15, 0, 10);
//    //    if (circles.empty()) {
//    //        cout << "未检测到圆形" << endl;
//    //        return 0;
//    //    }
//    //    cout << "检测到圆数目:" << circles.size() << endl;
//
//    //    endTime = clock();//计时结束
//    //    cout << "The run time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
//
//    //}
//
//    imshow("srcImage", srcImage);
//    waitKey(0);
//
//
//    //bool loop = true;
//    //for (int b_row = 0; b_row < 10; b_row++) {
//    //    for (int b_col = 0; b_col < 6; b_col++) {
//    //        Rect rect(b_row * 192,b_col * 180, 192, 180);
//    //        Mat ROIimageG = grayImage(rect);
//    //        /*int count = 0;
//    //        for (int i = 0; i < ROIimageG.rows; i++) {
//    //            for (int j = 0; j < ROIimageG.cols; j++) {
//    //                if (ROIimageG.at<uchar>(i, j) > 100) {
//    //                    count++;
//    //                }
//    //            }
//    //            if (count > 100 && count <900) {
//    //                cout << "count：" << count << endl;
//    //            }
//    //        }*/
//    //        rectangle(srcImage, rect, Scalar(255, 0, 0), 1, 1, 0);
//    //        vector<Vec3f> circles;
//    //        HoughCircles(ROIimageG, circles, HOUGH_GRADIENT, 1, 10, 200, 15, 0, 10);
//    //        if (!circles.empty()) {
//    //            cout << "b_row,b_col:" << b_row << ","<< b_col << endl;
//    //            cout << "circle_size:" << circles.size() << endl;
//    //            loop = false;
//    //            break;
//    //        }
//    //    }
//    //    if (!loop) {
//    //        break;
//    //    }
//    //}
//
//
//    //cout << "count：" << count << endl;
//    //cout << "面积占比：" << setprecision(5) << s << endl;
//
//
//    //imshow("srcImage", srcImage);
//    //imshow("correctImage", correctImage);
//    //waitKey(0);
//
//    return 0;
//}
