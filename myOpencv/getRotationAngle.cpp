#include "rotation.h"

float getRotationAngle(vector<Vec3f> points) {
    Vec3f LeftTop = points[0];
    Vec3f RightTop = points[1];
    Vec3f LeftBottom = points[2];

    //绘制定位线,0和2向量决定旋转角
    Point pt0(LeftTop[0], LeftTop[1]);
    Point pt2(LeftBottom[0], LeftBottom[1]);

    //为竖直向量取点
    Point pt3(pt0.x, pt0.y - 1);

    //求取俩向量夹角，即为旋转角度（顺时针方向为正）
    float theta = atan2(pt3.x - pt0.x, pt3.y - pt0.y) - atan2(pt0.x - pt2.x, pt0.y - pt2.y);
    if (theta > CV_PI)
        theta -= 2 * CV_PI;
    if (theta < -CV_PI)
        theta += 2 * CV_PI;
    theta = theta * 180.0 / CV_PI;

    //转为顺时针方向正值表达
    if (theta < 0) {
        theta += 360.0;
    }
    
    return theta;
}

