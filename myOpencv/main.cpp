#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat srcImage;
Mat grayImage;
Mat binaryImage;

double factor1 = 0;//左上左下
double factor2 = 0;//左上右上

const float minSideLength = 89;
const float maxSideLength = 93;

double cameraX;
double cameraY;

double cameraRealX;
double cameraRealY;

const int n = 4;
const double e = 5;
const int bitnum = n * n - 4;
int bits[bitnum];
int id = 0;


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

    clock_t startTime, endTime;
    
    //读取图片
    srcImage = imread("C:/Users/a/Desktop/center.jpg");

    //转化为灰度图
    cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);

    //Mat element = getStructuringElement(MORPH_RECT, Size(30, 30));
    //dilate(srcImage, binaryImage, element);
    

    //转化二值图像
    //threshold(grayImage, binaryImage, 230, 255, THRESH_BINARY);
    //imshow("binaryImage", binaryImage);
    //waitKey(0);
    startTime = clock();//计时开始
    adaptiveThreshold(grayImage, binaryImage, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 39, 15);

    endTime = clock();//计时结束
    cout << "The run time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

    imshow("binaryImage", binaryImage);
    waitKey(0);


    //Canny(binaryImage, binaryImage, 90, 100, 3);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(binaryImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());

    vector<Rect> boundRect(contours.size());  //定义外接矩形集合
    Point2f rect[4];
    Rect roiRect;
    for (int i = 0; i < contours.size(); i++)
    {
        boundRect[i] = boundingRect(Mat(contours[i]));
        if (boundRect[i].width < 160 && boundRect[i].width > 100 && boundRect[i].height < 160 && boundRect[i].height > 100) {
            roiRect = boundRect[i];
            rectangle(srcImage, Point(boundRect[i].x, boundRect[i].y), Point(boundRect[i].x + boundRect[i].width, boundRect[i].y + boundRect[i].height), Scalar(0, 255, 0), 2, 8);
        }
    }
   

    imshow("binaryImage", srcImage);
    waitKey(0);

    Mat ROIimage = grayImage(roiRect);
    //bool loop = true;
    //for (int b_row = 0; b_row < 10; b_row++) {
    //    for (int b_col = 0; b_col < 6; b_col++) {
    //        Rect rect(b_row * 192,b_col * 180, 192, 180);
    //        Mat ROIimageG = grayImage(rect);
    //        /*int count = 0;
    //        for (int i = 0; i < ROIimageG.rows; i++) {
    //            for (int j = 0; j < ROIimageG.cols; j++) {
    //                if (ROIimageG.at<uchar>(i, j) > 100) {
    //                    count++;
    //                }
    //            }
    //            if (count > 100 && count <900) {
    //                cout << "count：" << count << endl;
    //            }
    //        }*/
    //        rectangle(srcImage, rect, Scalar(255, 0, 0), 1, 1, 0);
    //        vector<Vec3f> circles;
    //        HoughCircles(ROIimageG, circles, HOUGH_GRADIENT, 1, 10, 200, 15, 0, 10);
    //        if (!circles.empty()) {
    //            cout << "b_row,b_col:" << b_row << ","<< b_col << endl;
    //            cout << "circle_size:" << circles.size() << endl;
    //            loop = false;
    //            break;
    //        }
    //    }
    //    if (!loop) {
    //        break;
    //    }
    //}


    //cout << "count：" << count << endl;
    //cout << "面积占比：" << setprecision(5) << s << endl;
   
    //定位所有圆心并找出定位顶点
    vector<Vec3f> circles;
    HoughCircles(ROIimage, circles, HOUGH_GRADIENT, 1, 10, 200, 15, 0, 10);

    if (circles.empty()) {
        cout << "未检测到圆形" << endl;
        return 0;
    }
    
    cout << "检测到圆数目:" << circles.size() << endl;

    //imshow("srcImage", srcImage);
    //imshow("correctImage", correctImage);
    //waitKey(0);
    
    return 0;
}
