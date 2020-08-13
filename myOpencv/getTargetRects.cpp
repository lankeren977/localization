#include "target_rects.h"

vector<Rect> getTargetRects(Mat grayImage,int thresholdValue,int rectMinLength,int rectMaxLength) {

    Mat binaryImage;
    vector<Rect> targetRects;//定义目标矩阵集合

    //根据阈值将灰度图转化为二值图像
    threshold(grayImage, binaryImage, thresholdValue, 255, THRESH_BINARY);

    //寻找边缘
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(binaryImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());

    //寻找符合条件的外接矩阵
    vector<Rect> boundRect(contours.size());  //定义外接矩形集合
    for (int i = 0; i < contours.size(); i++)
    {
        boundRect[i] = boundingRect(Mat(contours[i]));
        if (boundRect[i].width < rectMaxLength && boundRect[i].width > rectMinLength && boundRect[i].height < rectMaxLength && boundRect[i].height > rectMinLength) {
            targetRects.push_back(boundRect[i]);
            //rectangle(srcImage, Point(boundRect[i].x, boundRect[i].y), Point(boundRect[i].x + boundRect[i].width, boundRect[i].y + boundRect[i].height), Scalar(0, 255, 0), 1, 8);
        }
    }
    return targetRects;
}