#pragma once
#ifndef FIXED_POINTS_H
#define FIXED_POINTS_H

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<Vec3f> getThreeFixedPoints(vector<Vec3f> circles, int minSideLength, int maxSideLength);

#endif