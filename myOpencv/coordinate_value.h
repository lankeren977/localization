#pragma once
#ifndef COORDINATE_H
#define COORDINATE_H

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

double * getCoordinateValue(vector<Vec3f> points, Point cameraCenter, double realSideLength);

#endif
