#include "absolute_circles.h"

vector<Vec3f> getAbsoluteCircles(Mat grayImage,Rect ROIRect, double dp, double minDist, double param1, double param2, int minRadius, int maxRadius) {

    Mat ROIimage = grayImage(ROIRect);

    //ROI内检测圆形
    vector<Vec3f> circles;
    HoughCircles(ROIimage, circles, HOUGH_GRADIENT, dp, minDist, param1, param2, minRadius, maxRadius);
    if (!circles.empty()) {
        for (int i = 0; i < circles.size(); i++) {
            circles[i][0] += ROIRect.x;
            circles[i][1] += ROIRect.y;
        }
    }else {
        cout << "ROI内未检测到圆形" << endl;
    }
    return circles;
}