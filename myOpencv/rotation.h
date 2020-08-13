#pragma once
#ifndef ROTATION_H
#define ROTATION_H

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

float getRotationAngle(vector<Vec3f> points);

#endif