#pragma once
#ifndef TARGET_RECTS_H
#define TARGET_RECTS_H

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<Rect> getTargetRects(Mat grayImage, int thresholdValue, int rectMinLength, int rectMaxLength);

#endif
