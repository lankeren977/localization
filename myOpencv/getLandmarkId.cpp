#include "identify.h"

int getLandmarkId(vector<Vec3f> circles, vector<Vec3f> points) {

    Vec3f LeftTop = points[0];
    Vec3f RightTop = points[1];
    Vec3f LeftBottom = points[2];

    int bits[bit_size];
    int id = 0;

    //x��y���������
    double dx = (RightTop[0] - LeftTop[0]) / (n - 1);
    double dy = (LeftBottom[1] - LeftTop[1]) / (n - 1);

    //��λ���Բ�Ƿ����
    int seq = 0;
    int circlesNum = circles.size() - fixednum;
    int bitNum = 0;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            if ((i == 1 && j == 1) || (i == 1 && j == n) || (i == n && j == 1) || (i == n && j == n)) {
                continue;
            }
            double bitx = LeftTop[0] + (j - 1) * dx;
            double bity = LeftTop[1] + (i - 1) * dy;
            bits[seq] = 0;
            for (int k = 0; k < circles.size(); k++) {
                if (fabs(circles[k][0] - bitx) <= e && fabs(circles[k][1] - bity) <= e) {
                    bits[seq] = 1;
                    bitNum++;
                    circles.erase(circles.begin() + k);
                    break;
                }
            }
            seq++;
        }
    }

    //�ж϶�����Ϊ1����Ŀ��ԲȦ��Ŀ�Ƿ����
    if (circlesNum != bitNum) {
        cout << "IDʶ���쳣����©ԲȦ" << endl;
    }

    //����λֵ�������
    for (int i = 0; i < bit_size; i++) {
        id += bits[i] * pow(2, i);
    }
    return id;
}