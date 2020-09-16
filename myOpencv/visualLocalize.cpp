#include "visualLocalize.h"
#include "Landmark.h"
#include "loadConfig.h"

int binaryThreshold; //二值图像处理阈值
int binaryMaxValue; //二值图像处理最大值
int targetRectMin; //目标外接矩阵最小边界
int targetRectMax; //目标外接矩阵最大边界
int rectDistanceMin; //外接矩阵间最小距离
int rectDistanceMax; //外接矩阵间最大距离
double houghDp; //霍夫曼圆参数1
double houghMinDist; //霍夫曼圆参数2
double houghParam1; //霍夫曼圆参数3
double houghParam2; //霍夫曼圆参数4
int houghMinRadius; //霍夫曼圆参数5
int houghMaxRadius; //霍夫曼圆参数6
int maxSideLength; //图像中路标最大边长（像素）
int minSideLength; //图像中路标最小边长（像素）
float realSideLength; //实际路标边长
int dim; //路标维度
double e; //圆心确认误差范围

double getParam(String key){
    map<string,double>::iterator iter;
    iter = globalParams.find(key);
    if(iter != globalParams.end()){
        return iter->second;
    }else{
        cout << key << "未配置" << endl;
        return 0;
    }
}

void loadVisualParams(){
    binaryThreshold = getParam("binaryThreshold");
    binaryMaxValue = getParam("binaryMaxValue");
    targetRectMin = getParam("targetRectMin");
    targetRectMax = getParam("targetRectMax");
    rectDistanceMin = getParam("rectDistanceMin");
    rectDistanceMax = getParam("rectDistanceMax");
    houghDp = getParam("houghDp");
    houghMinDist = getParam("houghMinDist");
    houghParam1 = getParam("houghParam1");
    houghParam2 = getParam("houghParam2");
    houghMinRadius = getParam("houghMinRadius");
    houghMaxRadius = getParam("houghMaxRadius");
    maxSideLength = getParam("maxSideLength");
    minSideLength = getParam("minSideLength");
    realSideLength = getParam("realSideLength");
    dim = getParam("dim");
    e = getParam("e");
}

Vec3f getPointAffinedPos(Vec3f& src, const Point center, double angle)
{
    angle = angle * CV_PI / 180;
    int x = src[0] - center.x;
    int y = src[1] - center.y;
    src[0] = cvRound(x * cos(angle) + y * sin(angle) + center.x);
    src[1] = cvRound(-x * sin(angle) + y * cos(angle) + center.y);
    return src;
}

LocalizeData getVisualLocalizeData(Mat srcImage){
    //取配置参数
    loadVisualParams();

    Mat grayImage,binaryImage;
    //转化灰度图
    cvtColor(srcImage, grayImage, COLOR_RGB2GRAY);

    //计算相机中心坐标
    Point cameraCenter(grayImage.cols / 2, grayImage.rows / 2);

    //转化二值图像
    threshold(grayImage, binaryImage, binaryThreshold, binaryMaxValue, THRESH_BINARY);
    
    //寻找轮廓
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(binaryImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());

    //获取外接矩形集合、目标矩阵集合
    vector<Rect> boundRect(contours.size());
    vector<Rect> targetRects;
    for (int i = 0; i < contours.size(); i++){
        boundRect[i] = boundingRect(Mat(contours[i]));
        if (boundRect[i].width < targetRectMax && boundRect[i].width > targetRectMin && boundRect[i].height < targetRectMax && boundRect[i].height > targetRectMin) { //圆形外界矩阵长宽限制一致
            targetRects.push_back(boundRect[i]);
        }
    }

    //将目标矩阵分堆
    vector<vector<Rect>> rectStacks;
    while (targetRects.size() != 0) {
        vector<Rect> tempRectStacks; //以参考矩阵划分的矩阵堆
        Rect temp = targetRects[0]; //随机抽取一个矩阵为参考
        tempRectStacks.push_back(temp);
        targetRects.erase(targetRects.begin());
        for (int i = 0; i < targetRects.size(); i++) {
            double distance = sqrt(pow(targetRects[i].x - temp.x, 2) + pow(targetRects[i].y - temp.y, 2));
            if (distance > rectDistanceMin && distance < rectDistanceMax) {
                tempRectStacks.push_back(targetRects[i]);
                targetRects.erase(targetRects.begin() + i);
                i--;
            }
        }
        if (tempRectStacks.size() > 3) { //超过3点才可能是路标
            rectStacks.push_back(tempRectStacks);
        }
    }
    
    //将各堆矩阵整合为预选ROI
    vector<Rect> oriROIs;
    if (rectStacks.size() > 0) {
        for (int i = 0; i < rectStacks.size(); i++) {
            vector<Rect> rects = rectStacks[i];
            int minX = 2000, minY = 2000; //考虑到像素为1980x1080
            int maxX = 0, maxY = 0;
            for (int j = 0; j < rects.size(); j++) {
                minX = (rects[j].x < minX) ? rects[j].x : minX;
                minY = (rects[j].y < minY) ? rects[j].y : minY;
                maxX = (rects[j].x > maxX) ? rects[j].x : maxX;
                maxY = (rects[j].y > maxY) ? rects[j].y : maxY;
            }
            //避免遗漏尝试扩大边界5像素
            minX = (minX >= 10) ? minX-10 : minX;
            minY = (minX >= 10) ? minY-10 : minY;
            maxX = maxX - minX + 20;
            maxY = maxY - minY + 20;
            Rect newRect = Rect(minX, minY, maxX, maxY);
            oriROIs.push_back(newRect);
        }
    }
    
    //在预选ROI中进行霍夫曼圆检测，确定最终参考路标
    vector<Landmark> landmarks;
    
    for(int i = 0; i < oriROIs.size(); i++){
        Mat roiImage = grayImage(oriROIs[i]);
        vector<Vec3f> circles;
        HoughCircles(roiImage, circles, HOUGH_GRADIENT, houghDp, houghMinDist, houghParam1, houghParam2, houghMinRadius, houghMaxRadius);
        
        if (!circles.empty() && circles.size() > 3) {
            
            //还原圆在原图的像素坐标
            for (int j = 0; j < circles.size(); j++) {
                circles[j][0] += oriROIs[i].x;
                circles[j][1] += oriROIs[i].y;
            }
            
            //确定定位三点
            Vec3f LeftTop, RightTop, LeftBottom;
            for (int j = 0; j < circles.size(); j++) {
                int num = 0;
                //筛选三点
                for (int k = 0; k < circles.size(); k++) {
                    if(j != k){
                        float distance = sqrt(pow(circles[j][0] - circles[k][0], 2) + pow(circles[j][1] - circles[k][1], 2));
                        if (distance <= maxSideLength && distance >= minSideLength) {
                            num++;
                            if (num == 1) {
                                RightTop = circles[k];
                            }
                            if (num == 2) {
                                LeftBottom = circles[k];
                                break;
                            }
                        }
                    }
                }
                
                //确认左下角、右上角定位点
                if (num == 2) {
                    LeftTop = circles[j];
                    bool flag = true;
                    if (LeftTop[1] <= RightTop[1] && LeftTop[1] <= LeftBottom[1]) { //上0<=angle<90逆时针旋转
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
                        cout << "旋转角度检测异常" << endl;
                        break;
                    }
                    if (!flag) {
                        Vec3f temp = RightTop;
                        RightTop = LeftBottom;
                        LeftBottom = temp;
                    }
                    
                    Landmark tempLandmark;
                    tempLandmark.setLeftTop(LeftTop);
                    tempLandmark.setRightTop(RightTop);
                    tempLandmark.setLeftBottom(LeftBottom);
                    tempLandmark.setCircles(circles);
                    landmarks.push_back(tempLandmark);
                    break;
                }
            }
        }
    }
    
    vector<LocalizeData> localizeDatas;
    if(landmarks.size() > 0){
        
        for(int i = 0; i < landmarks.size(); i++){
            
            Vec3f LeftTop = landmarks[i].getLeftTop();
            Vec3f RightTop = landmarks[i].getRightTop();
            Vec3f LeftBottom = landmarks[i].getLeftBottom();
            
            //矫正三点(图像中三点可能不垂直)
            if (LeftTop[0] == LeftBottom[0]) {
                RightTop[1] = LeftTop[1];
                cout << "定位点坐标矫正1" << endl;
            }
            else if (LeftTop[1] == LeftBottom[1]) {
                RightTop[0] = LeftTop[0];
                cout << "定位点坐标矫正2" << endl;
            }
            else if (LeftTop[0] == RightTop[0]) {
                LeftBottom[1] = LeftTop[1];
                cout << "定位点坐标矫正3" << endl;
            }
            else if (LeftTop[1] == RightTop[1]) {
                LeftBottom[0] = LeftTop[0];
                cout << "定位点坐标矫正4" << endl;
            }
            
            /* 获取坐标 */
            //计算图像中相机距离leftTop的距离
            double imgDis = sqrt(pow((cameraCenter.x - LeftTop[0]), 2) + pow((cameraCenter.y - LeftTop[1]), 2));
            //计算相机较landmark的方向
            float theta_z = atan2(LeftTop[0] - LeftBottom[0], LeftTop[1] - LeftBottom[1]) - atan2(cameraCenter.x - LeftTop[0], cameraCenter.y - LeftTop[1]);
            if (theta_z > CV_PI)
                theta_z -= 2 * CV_PI;
            if (theta_z < -CV_PI)
                theta_z += 2 * CV_PI;
            //计算相对于landmark的坐标
            double cameraX = imgDis * sin(theta_z);
            double cameraY = imgDis * cos(theta_z);
            //计算比例因子
            double disLeft = sqrt(pow(LeftTop[0] - LeftBottom[0], 2) + pow(LeftTop[1] - LeftBottom[1], 2));
            double disTop = sqrt(pow(LeftTop[0] - RightTop[0], 2) + pow(LeftTop[1] - RightTop[1], 2));
            double factor1 = realSideLength / disTop;
            double factor2 = realSideLength / disLeft;
            //换算为实际位置
            double cameraRealX = cameraX * factor1;
            double cameraRealY = cameraY * factor2;
            
            /* 获取偏转角度 */
            Point pt0(LeftTop[0], LeftTop[1]);
            Point pt2(LeftBottom[0], LeftBottom[1]);
            //为竖直向量取点
            Point pt3(pt0.x, pt0.y - 1);
            //求取俩向量夹角角度
            float theta_p = atan2(pt3.x - pt0.x, pt3.y - pt0.y) - atan2(pt0.x - pt2.x, pt0.y - pt2.y);
            if (theta_p > CV_PI)
                theta_p -= 2 * CV_PI;
            if (theta_p < -CV_PI)
                theta_p += 2 * CV_PI;
            theta_p = theta_p * 180.0 / CV_PI;
            //全部改为顺时针方向,正值表达
            theta_p = (theta_p < 0) ? (theta_p + 360.0) : theta_p;
            
            /* 获取id */
            int size = dim * dim - 4;
            int bits[size];
            //放射变换后定位点对应的新坐标，theta旋转角度为逆时针
            vector<Vec3f> circles = landmarks[i].getCircles();
            for (int j = 0; j < circles.size(); j++) {
                circles[j] = getPointAffinedPos(circles[j], cameraCenter, theta_p);
            }
            LeftTop = getPointAffinedPos(LeftTop, cameraCenter, theta_p);
            RightTop = getPointAffinedPos(RightTop, cameraCenter, theta_p);
            LeftBottom = getPointAffinedPos(LeftBottom, cameraCenter, theta_p);
            //x和y轴分量计算
            double dx = (RightTop[0] - LeftTop[0]) / (dim - 1);
            double dy = (LeftBottom[1] - LeftTop[1]) / (dim - 1);
            //按位检测圆是否存在
            int seq = 0, bitNum = 0, id = 0;
            int circlesNum = circles.size() - 3;
            for (int m = 1; m <= dim; m++) {
                for (int j = 1; j <= dim; j++) {
                    if ((m == 1 && j == 1) || (m == 1 && j == dim) || (m == dim && j == 1) || (m == dim && j == dim)) {
                        continue;
                    }
                    double bitx = LeftTop[0] + (j - 1) * dx;
                    double bity = LeftTop[1] + (m - 1) * dy;
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
            //判断二进制为1的数目和圆圈数目是否相等
            if (circlesNum != bitNum) {
                cout << "ID识别异常" << endl;
            }
            //根据位值计算序号
            for (int q = 0; q < dim * dim - 4; q++) {
                id += bits[q] * pow(2, q);
            }
            
            /* 保存定位数据 */
            LocalizeData data;
            theta_p = (int)(theta_p * 10) / 10;
            data.setLandmarkId(id);
            data.setVisualTheta(theta_p);
            data.setVisualX(cameraRealX);
            data.setVisualY(cameraRealY);
            localizeDatas.push_back(data);
        }
    }else{
        cout << "未检测到定位点" << endl;
    }
    
    //平均各路标数据
    if(localizeDatas.size() > 0){
        float v_x = 0, v_y = 0;
        int num = 0;
        for(int i = 0; i < localizeDatas.size(); i++){
            map<int,double*>::iterator iter;
            iter = globalLandmarks.find(localizeDatas[i].getLandmarkId());
            if(iter != globalLandmarks.end()){
                v_x += iter->second[0] + localizeDatas[i].getVisualX();
                v_y += iter->second[1] + localizeDatas[i].getVisualY();
                num++;
            }else{
                cout << "id:" << localizeDatas[i].getLandmarkId() << " 配置异常" <<endl;
            }
        }
        v_x /= num;
        v_y /= num;
        LocalizeData final_data;
        final_data.setLandmarkId(1);
        final_data.setVisualX(v_x);
        final_data.setVisualY(v_y);
        return final_data;
    }else{
        cout << "未检测到路标" << endl;
        LocalizeData null_data;
        null_data.setLandmarkId(-1);
        return null_data;
    }
}



