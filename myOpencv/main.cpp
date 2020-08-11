#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//Mat srcImage;

const double realLength = 550.0; //24cm=90pixels
const double realWidth = 309.4;
const double factor = 0.26667;

float circleRadius = 0;
const float minSideLength = 89;
const float maxSideLength = 93;

const int n = 4;
const double e = 5;
const int bitnum = n * n - 4;


Vec3f getPointAffinedPos(Vec3f& src, const Point center, double angle)
{
    angle = angle * CV_PI / 180;
    int x = src[0] - center.x;
    int y = src[1] - center.y;
    src[0] = cvRound(x * cos(angle) + y * sin(angle) + center.x);
    src[1] = cvRound(-x * sin(angle) + y * cos(angle) + center.y);
    return src;
}

int main() {

    VideoCapture cap(0);
    cap.set(3, 1920);//��� 
    cap.set(4, 1080);//�߶�
    if (!cap.isOpened())
    {
        cout << "����ͷδ�ɹ���" << endl;
    }
    Mat resultImg;

    //��ȡ����ͷ�е�֡
    clock_t startTime, endTime;
    for (; ; )
    {
        startTime = clock();//��ʱ��ʼ
        int bits[bitnum];
        int circleSize = 0;
        Mat srcImage;
        Mat grayImage;
        Mat binaryImage;
        Mat correctImage;

        cap >> srcImage;
        //�ж��Ƿ��ȡ��
        if (srcImage.empty())
        {
            break;
        }

        //ת��Ϊ�Ҷ�ͼ
        cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);

        //���������������
        Point cameraCenter(grayImage.cols / 2, grayImage.rows / 2);

        //ת����ֵͼ��
        threshold(grayImage, binaryImage, 230, 255, THRESH_BINARY);

        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(binaryImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());

        vector<Rect> boundRect(contours.size());  //������Ӿ��μ���
        vector<Rect> targetRects;//����Ŀ����󼯺�
        for (int i = 0; i < contours.size(); i++)
        {
            boundRect[i] = boundingRect(Mat(contours[i]));
            if (boundRect[i].width < 20 && boundRect[i].width > 8 && boundRect[i].height < 20 && boundRect[i].height > 8) {
                targetRects.push_back(boundRect[i]);
                rectangle(srcImage, Point(boundRect[i].x, boundRect[i].y), Point(boundRect[i].x + boundRect[i].width, boundRect[i].y + boundRect[i].height), Scalar(0, 255, 0), 1, 8);
            }
        }

        int maxDisValue = 160;
        int minDisValue = 15;
        vector<vector<Rect>> ROIs;//����ROI����
        while (targetRects.size() != 0) {
            Rect temp = targetRects[0];
            vector<Rect> tempRects;
            tempRects.push_back(temp);
            targetRects.erase(targetRects.begin());
            for (int m = 0; m < targetRects.size(); m++) {
                double distance = sqrt(pow(targetRects[m].x - temp.x, 2) + pow(targetRects[m].y - temp.y, 2));
                if (distance < maxDisValue && distance > minDisValue) {
                    tempRects.push_back(targetRects[m]);
                    targetRects.erase(targetRects.begin() + m);
                    m--;
                }
            }
            if (tempRects.size() > 3) { //����3��ſ�����·��
                ROIs.push_back(tempRects);
            }
        }

        if (ROIs.size() > 0) {
            for (int p = 0; p < ROIs.size(); p++) {

                vector<Rect> landmark = ROIs[p];
                int minX = 2000;
                int minY = 2000;
                int maxX = 0;
                int maxY = 0;
                for (int q = 0; q < landmark.size(); q++) {
                    if (landmark[q].x < minX) {
                        minX = landmark[q].x;
                    }
                    if (landmark[q].y < minY) {
                        minY = landmark[q].y;
                    }
                    if (landmark[q].x > maxX) {
                        maxX = landmark[q].x;
                    }
                    if (landmark[q].y > maxY) {
                        maxY = landmark[q].y;
                    }
                }
                minX -= 10;
                minY -= 10;
                maxX = maxX - minX + 20;
                maxY = maxY - minY + 20;
                Rect roiRect = Rect(minX, minY, maxX, maxY);
                rectangle(srcImage, Point(roiRect.x, roiRect.y), Point(roiRect.x + roiRect.width, roiRect.y + roiRect.height), Scalar(0, 0, 255), 1, 8);
                Mat ROIimage = grayImage(roiRect);

                //��λ����Բ�Ĳ��ҳ���λ����
                vector<Vec3f> circles;
                HoughCircles(ROIimage, circles, HOUGH_GRADIENT, 1, 10, 200, 15, 0, 10);
                if (circles.empty()) {
                    cout << "δ��⵽Բ��" << endl;
                    continue;
                }

                for (int i = 0; i < circles.size(); i++) {
                    circles[i][0] += roiRect.x;
                    circles[i][1] += roiRect.y;
                }


                //��ÿ��·����м��
                //ɸѡ������������Բ�����Բ�ľ���Ϊ�뾶�ĵ�
                Vec3f LeftTop;
                Vec3f RightTop;
                Vec3f LeftBottom;
                float distance;
                int i = 0;
                int j = 0;
                bool loop = true;
                for (i = 0; i < circles.size(); i++) {
                    int num = 0;
                    for (j = 0; j < circles.size(); j++) {
                        distance = sqrt(pow(circles[i][0] - circles[j][0], 2) + pow(circles[i][1] - circles[j][1], 2));
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
                        LeftTop = circles[i];
                        bool flag = true;
                        if (LeftTop[1] <= RightTop[1] && LeftTop[1] <= LeftBottom[1]) { //�ϣ�0<=angle<90��ʱ����ת
                            if (LeftBottom[0] > RightTop[0]) {
                                flag = false;
                            }
                        }
                        else if (LeftTop[0] >= RightTop[0] && LeftTop[0] >= LeftBottom[0]) {//��90<=angle<180
                            if (RightTop[1] < LeftBottom[1]) {
                                flag = false;
                            }
                        }
                        else if (LeftTop[1] >= RightTop[1] && LeftTop[1] >= LeftBottom[1]) {//��180<=angle<270
                            if (RightTop[0] > LeftBottom[0]) {
                                flag = false;
                            }
                        }
                        else if (LeftTop[0] <= RightTop[0] && LeftTop[0] <= LeftBottom[0]) {//��270<=angle<360
                            if (RightTop[1] > LeftBottom[1]) {
                                flag = false;
                            }
                        }
                        else {
                            loop = false;
                            break;
                        }
                        if (!flag) {
                            Vec3f temp = RightTop;
                            RightTop = LeftBottom;
                            LeftBottom = temp;
                        }
                        break;
                    }
                }

                if (!loop) {
                    cout << "��λ�������쳣" << endl;
                    continue;
                }

                //����ǰ
                float vertical = atan2(RightTop[0] - LeftTop[0], RightTop[1] - LeftTop[1]) - atan2(LeftBottom[0] - LeftTop[0], LeftBottom[1] - LeftTop[1]);
                vertical = vertical * 180.0 / CV_PI;


                //����ƫת�½���һ����������
                if (LeftTop[0] == LeftBottom[0]) {
                    RightTop[1] = LeftTop[1];
                    cout << "����ƫת����1" << endl;
                }
                else if (LeftTop[1] == LeftBottom[1]) {
                    RightTop[0] = LeftTop[0];
                    cout << "����ƫת����2" << endl;
                }
                else if (LeftTop[0] == RightTop[0]) {
                    LeftBottom[1] = LeftTop[1];
                    cout << "����ƫת����3" << endl;
                }
                else if (LeftTop[1] == RightTop[1]) {
                    LeftBottom[0] = LeftTop[0];
                    cout << "����ƫת����4" << endl;
                }

                //������
                float vertical2 = atan2(RightTop[0] - LeftTop[0], RightTop[1] - LeftTop[1]) - atan2(LeftBottom[0] - LeftTop[0], LeftBottom[1] - LeftTop[1]);
                vertical2 = vertical2 * 180.0 / CV_PI;


                //���ƶ�λ��,0��2����������ת��
                Point pt0(LeftTop[0], LeftTop[1]);
                Point pt1(RightTop[0], RightTop[1]);
                Point pt2(LeftBottom[0], LeftBottom[1]);

                //����ͼ��������������϶���ľ���
                double imgDis = sqrt(pow((cameraCenter.x - LeftTop[0]), 2) + pow((cameraCenter.y - LeftTop[1]), 2));

                //���������landmark�ķ���
                float theta2 = atan2(LeftTop[0] - LeftBottom[0], LeftTop[1] - LeftBottom[1]) - atan2(cameraCenter.x - LeftTop[0], cameraCenter.y - LeftTop[1]);
                if (theta2 > CV_PI)
                    theta2 -= 2 * CV_PI;
                if (theta2 < -CV_PI)
                    theta2 += 2 * CV_PI;

                //���������landmark������
                double cameraX = imgDis * sin(theta2);
                double cameraY = imgDis * cos(theta2);

                //ȫ��תΪ˳ʱ�뷽��
                theta2 = theta2 * 180.0 / CV_PI;
                if (theta2 < 0) {
                    theta2 += 360.0;
                }
                //cout << "���������·������ϵ�ļн�:" << theta2 << endl;

                //�����������
                double disLeft = sqrt(pow(LeftTop[0] - LeftBottom[0], 2) + pow(LeftTop[1] - LeftBottom[1], 2));
                double disTop = sqrt(pow(LeftTop[0] - RightTop[0], 2) + pow(LeftTop[1] - RightTop[1], 2));
                //cout << "x�᳤�ȣ�" << disTop << " pixels�� y�᳤�ȣ�" << disLeft << " pixels" << endl;
                double factor1 = 24 / disTop;
                double factor2 = 24 / disLeft;

                //����Ϊʵ��λ��
                double cameraRealX = cameraX * factor1;
                double cameraRealY = cameraY * factor2;

                //Ϊ��ֱ����ȡ��
                Point pt3(pt0.x, pt0.y - 1);

                //��ȡ�������нǣ���Ϊ��ת�Ƕȣ�˳ʱ�뷽��Ϊ����
                float theta = atan2(pt3.x - pt0.x, pt3.y - pt0.y) - atan2(pt0.x - pt2.x, pt0.y - pt2.y);
                if (theta > CV_PI)
                    theta -= 2 * CV_PI;
                if (theta < -CV_PI)
                    theta += 2 * CV_PI;
                theta = theta * 180.0 / CV_PI;
                if (theta < 0) {
                    theta += 360.0;
                }

                //����任��λ���Ӧ�������꣬theta��ת�Ƕ�Ϊ��ʱ��
                for (int i = 0; i < circles.size(); i++) {
                    circles[i] = getPointAffinedPos(circles[i], cameraCenter, theta);
                }
                //������circles�����ȥ������λ��
                LeftTop = getPointAffinedPos(LeftTop, cameraCenter, theta);
                RightTop = getPointAffinedPos(RightTop, cameraCenter, theta);
                LeftBottom = getPointAffinedPos(LeftBottom, cameraCenter, theta);

                //x��y���������
                double dx = (RightTop[0] - LeftTop[0]) / (n - 1);
                double dy = (LeftBottom[1] - LeftTop[1]) / (n - 1);

                //��λ���Բ�Ƿ����
                int seq = 0;
                int circlesNum = circles.size() - 3;
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

                int id = 0;
                //����λֵ�������
                for (int i = 0; i < bitnum; i++) {
                    id += bits[i] * pow(2, i);
                }

                endTime = clock();//��ʱ����
                cout << "The run time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
                cout << "��⵽Բ��Ŀ:" << circles.size() << endl;
                cout << "����ǰ����нǣ�" << vertical << endl;
                cout << "����������нǣ�" << vertical2 << endl;
                cout << "������ĵ���·�����ϵ����ؾ��룺" << imgDis << "pixels, ʵ�ʾ��룺" << imgDis * (factor1 + factor2) / 2 << "cm" << endl;
                cout << "���ͼ������:(" << cameraX << "," << cameraY << ")" << endl;
                cout << "���ʵ������:(" << cameraRealX << "," << cameraRealY << ")" << endl;
                cout << "ͼ����ת�Ƕ�(˳ʱ��):" << theta << endl;
                cout << "ʶ��ID:" << id << endl;
                cout << "========================================================" << endl;
            }
            imshow("camera", srcImage);
            if (waitKey(300) >= 0)
            {
                break;
            }
        }else {
            cout << "δ��⵽Ŀ��ROI" << endl;
        }
    }


    return 0;
}
