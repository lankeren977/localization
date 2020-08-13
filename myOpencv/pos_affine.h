#pragma once
#ifndef POS_AFFINE_H
#define POS_AFFINE_H

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Vec3f getPointAffinedPos(Vec3f& src, const Point center, double angle);

#endif