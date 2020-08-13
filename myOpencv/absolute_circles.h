#pragma once
#ifndef ABSOLUTE_CIRCLES_H
#define ABSOLUTE_CIRCLES_H

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<Vec3f> getAbsoluteCircles(Mat grayImage, Rect ROIRect, double dp, double minDist, double param1, double param2, int minRadius, int maxRadius);

#endif