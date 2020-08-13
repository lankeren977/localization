#pragma once
#ifndef LANDMARK_ROIS_H
#define LANDMARK_ROIS_H

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<Rect> getLandmarkROIs(vector<Rect> targetRects, int minDisValue, int maxDisValue, int fixedPointsNum);

#endif