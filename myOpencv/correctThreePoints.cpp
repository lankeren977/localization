#include "correct_fixed_points.h"

void correctFixedPoints(vector<Vec3f> points) {
    //����ǰ
    float vertical = atan2(points[1][0] - points[0][0], points[1][1] - points[0][1]) - atan2(points[2][0] - points[0][0], points[2][1] - points[0][1]);
    vertical = vertical * 180.0 / CV_PI;

    //����ƫת�½���һ����������
    if (points[0][0] == points[2][0]) {
        points[1][1] = points[0][1];
        cout << "����ƫת����1" << endl;
    }
    else if (points[0][1] == points[2][1]) {
        points[1][0] = points[0][0];
        cout << "����ƫת����2" << endl;
    }
    else if (points[0][0] == points[1][0]) {
        points[2][1] = points[0][1];
        cout << "����ƫת����3" << endl;
    }
    else if (points[0][1] == points[1][1]) {
        points[2][0] = points[0][0];
        cout << "����ƫת����4" << endl;
    }

    //������
    float vertical2 = atan2(points[1][0] - points[0][0], points[1][1] - points[0][1]) - atan2(points[2][0] - points[0][0], points[2][1] - points[0][1]);
    vertical2 = vertical2 * 180.0 / CV_PI;

    cout << "����ǰ����нǣ�" << vertical << endl;
    cout << "����������нǣ�" << vertical2 << endl;
}