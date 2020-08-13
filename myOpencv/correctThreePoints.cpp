#include "correct_fixed_points.h"

void correctFixedPoints(vector<Vec3f> points) {
    //矫正前
    float vertical = atan2(points[1][0] - points[0][0], points[1][1] - points[0][1]) - atan2(points[2][0] - points[0][0], points[2][1] - points[0][1]);
    vertical = vertical * 180.0 / CV_PI;

    //整度偏转下矫正一下三点坐标
    if (points[0][0] == points[2][0]) {
        points[1][1] = points[0][1];
        cout << "整度偏转矫正1" << endl;
    }
    else if (points[0][1] == points[2][1]) {
        points[1][0] = points[0][0];
        cout << "整度偏转矫正2" << endl;
    }
    else if (points[0][0] == points[1][0]) {
        points[2][1] = points[0][1];
        cout << "整度偏转矫正3" << endl;
    }
    else if (points[0][1] == points[1][1]) {
        points[2][0] = points[0][0];
        cout << "整度偏转矫正4" << endl;
    }

    //矫正后
    float vertical2 = atan2(points[1][0] - points[0][0], points[1][1] - points[0][1]) - atan2(points[2][0] - points[0][0], points[2][1] - points[0][1]);
    vertical2 = vertical2 * 180.0 / CV_PI;

    cout << "矫正前三点夹角：" << vertical << endl;
    cout << "矫正后三点夹角：" << vertical2 << endl;
}