#include "landmark_rois.h"

vector<Rect> getLandmarkRois(vector<Rect> targetRects, int minDisValue, int maxDisValue, int fixedPointsNum) {

    vector<Rect> targetROIs;//定义LandmarkROI集合
    vector<vector<Rect>> ROIs;//定义ROI集合
    while (targetRects.size() != 0) {
        vector<Rect> tempRects;//该堆外接矩阵集合
        Rect temp = targetRects[0];
        tempRects.push_back(temp);
        targetRects.erase(targetRects.begin());

        for (int i = 0; i < targetRects.size(); i++) {
            double distance = sqrt(pow(targetRects[i].x - temp.x, 2) + pow(targetRects[i].y - temp.y, 2));
            if (distance < maxDisValue && distance > minDisValue) {
                tempRects.push_back(targetRects[i]);
                targetRects.erase(targetRects.begin() + i);
                i--;
            }
        }
        if (tempRects.size() > fixedPointsNum) { //路标ROI包含矩阵需大于固定点数目
            ROIs.push_back(tempRects);
        }
    }

    //寻找该堆最大外围矩阵
    if (ROIs.size() > 0) {
        for (int i = 0; i < ROIs.size(); i++) {
            vector<Rect> landmark = ROIs[i];
            int minX = 2000; //考虑到像素值最大1980
            int minY = 2000;
            int maxX = 0;
            int maxY = 0;
            for (int i = 0; i < landmark.size(); i++) {
                if (landmark[i].x < minX) {
                    minX = landmark[i].x;
                }
                if (landmark[i].y < minY) {
                    minY = landmark[i].y;
                }
                if (landmark[i].x > maxX) {
                    maxX = landmark[i].x;
                }
                if (landmark[i].y > maxY) {
                    maxY = landmark[i].y;
                }
            }
            //适当放大一些范围
            if (minX >= 10) {
                minX -= 10;
            }
            if (minY >= 10) {
                minY -= 10;
            }
            maxX = maxX - minX + 10;
            maxY = maxY - minY + 10;
            Rect roiRect = Rect(minX, minY, maxX, maxY);
            //rectangle(srcImage, Point(roiRect.x, roiRect.y), Point(roiRect.x + roiRect.width, roiRect.y + roiRect.height), Scalar(0, 0, 255), 1, 8);
            //Mat ROIimage = grayImage(roiRect);
            targetROIs.push_back(roiRect);
        }
        return targetROIs;
    }
}