#include "coordinate_value.h"

double * getCoordinateValue(vector<Vec3f> points, Point cameraCenter, double realSideLength) {

    double coordinate_value[2];
    
    Vec3f LeftTop = points[0];
    Vec3f RightTop = points[1];
    Vec3f LeftBottom = points[2];

    //计算图像中相机中心距离左上顶点的距离
    double imgDis = sqrt(pow((cameraCenter.x - LeftTop[0]), 2) + pow((cameraCenter.y - LeftTop[1]), 2));

    //计算相机较landmark的方向
    float theta2 = atan2(LeftTop[0] - LeftBottom[0], LeftTop[1] - LeftBottom[1]) - atan2(cameraCenter.x - LeftTop[0], cameraCenter.y - LeftTop[1]);
    if (theta2 > CV_PI)
        theta2 -= 2 * CV_PI;
    if (theta2 < -CV_PI)
        theta2 += 2 * CV_PI;

    //计算相对于landmark的像素坐标
    double cameraX = imgDis * sin(theta2);
    double cameraY = imgDis * cos(theta2);

    //度数转为顺时针方向正值表达
    /*theta2 = theta2 * 180.0 / CV_PI;
    if (theta2 < 0) {
        theta2 += 360.0;
    }*/

    //计算比例因子
    double disLeft = sqrt(pow(LeftTop[0] - LeftBottom[0], 2) + pow(LeftTop[1] - LeftBottom[1], 2));
    double disTop = sqrt(pow(LeftTop[0] - RightTop[0], 2) + pow(LeftTop[1] - RightTop[1], 2));
 
    double factor1 = realSideLength / disTop;
    double factor2 = realSideLength / disLeft;

    //换算为实际位置
    coordinate_value[0] = cameraX * factor1;
    coordinate_value[1] = cameraY * factor2;

    return coordinate_value;
}

