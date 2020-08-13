#pragma once
#ifndef IDENTIFY_H
#define IDENTIFY_H

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const int landmark_dim = 4;
const int fixednum = 3;
const double e = 5;
const int bit_size = landmark_dim * landmark_dim - fixednum -1;

int getLandmarkId(vector<Vec3f> circles, vector<Vec3f> points);

#endif