#include "fixed_points.h"

vector<Vec3f> getThreeFixedPoints(vector<Vec3f> circles, int minSideLength, int maxSideLength) {

    vector<Vec3f> fixedPoints;
    Vec3f LeftTop;
    Vec3f RightTop;
    Vec3f LeftBottom;
    Vec3f CheckBit;
 
    bool checkFlag = false;
    for (int i = 0; i < circles.size(); i++) {
        int num = 0;
        for (int j = 0; j < circles.size(); j++) {
            float distance = sqrt(pow(circles[i][0] - circles[j][0], 2) + pow(circles[i][1] - circles[j][1], 2));
            if (distance <= maxSideLength && distance >= minSideLength) {
                num++;
                if (num == 1) {
                    RightTop = circles[j];
                }
                if (num == 2) {
                    LeftBottom = circles[j];
                    break;
                }
            }
        }
        if (num == 2) {
            checkFlag = true;
            LeftTop = circles[i];
            bool flag = true;
            if (LeftTop[1] <= RightTop[1] && LeftTop[1] <= LeftBottom[1]) { //上，0<=angle<90逆时针旋转
                if (LeftBottom[0] > RightTop[0]) {
                    flag = false;
                }
            }
            else if (LeftTop[0] >= RightTop[0] && LeftTop[0] >= LeftBottom[0]) {//右90<=angle<180
                if (RightTop[1] < LeftBottom[1]) {
                    flag = false;
                }
            }
            else if (LeftTop[1] >= RightTop[1] && LeftTop[1] >= LeftBottom[1]) {//下180<=angle<270
                if (RightTop[0] > LeftBottom[0]) {
                    flag = false;
                }
            }
            else if (LeftTop[0] <= RightTop[0] && LeftTop[0] <= LeftBottom[0]) {//左270<=angle<360
                if (RightTop[1] > LeftBottom[1]) {
                    flag = false;
                }
            }
            else {
                cout << "定位点坐标异常" << endl;
            }
            if (!flag) {
                Vec3f temp = RightTop;
                RightTop = LeftBottom;
                LeftBottom = temp;
            }
            break;
        }
    }

    fixedPoints.push_back(LeftTop);//0
    fixedPoints.push_back(RightTop);//1
    fixedPoints.push_back(LeftBottom);//2

    if (!checkFlag) {
        cout << "未找到符合条件的定位点" << endl;
        fixedPoints.push_back(CheckBit);
    }

    return fixedPoints;
}