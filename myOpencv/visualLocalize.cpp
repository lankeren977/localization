#include "visualLocalize.h"
#include "Landmark.h"
#include "loadConfig.h"

int binaryThreshold; //二值图像处理阈值
int binaryMaxValue; //二值图像处理最大值
int targetRectMin; //目标外接矩阵最小边界
int targetRectMax; //目标外接矩阵最大边界
int rectDistanceMin; //外接矩阵间最小距离
int rectDistanceMax; //外接矩阵间最大距离
float realSideLength; //实际路标边长
int dim; //路标维度
double e; //圆心确认误差范围
float past_theta = 0;
LocalizeData preVisualData; //上一次正常帧的数据

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
            rectangle(srcImage, Point(boundRect[i].x,boundRect[i].y), Point(boundRect[i].x+boundRect[i].width,boundRect[i].y+boundRect[i].height),Scalar(0,255,0));
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
    
    //在各堆矩阵中找出圆心
    vector<vector<Vec3f>> all_circles;
    if (rectStacks.size() > 0) {
        for (int i = 0; i < rectStacks.size(); i++) {
            vector<Vec3f> temp_circles;
            vector<Rect> tempRectStacks = rectStacks[i];
            for(int j = 0; j < tempRectStacks.size(); j++){
                float c_x = tempRectStacks[j].x + (tempRectStacks[j].width / 2);
                float c_y = tempRectStacks[j].y + (tempRectStacks[j].height / 2);
                float c_r = (tempRectStacks[j].width + tempRectStacks[j].height) / 2;
                Vec3f circle = Vec3f(c_x,c_y,c_r);
                temp_circles.push_back(circle);
            }
            all_circles.push_back(temp_circles);
        }
    }
    
    //根据各路标的圆心数据，确定最终参考路标
    vector<Landmark> landmarks;
    for(int i = 0; i < all_circles.size(); i++){
        vector<Vec3f> circles = all_circles[i];
        if (!circles.empty()) {
            //最长边确定左下和右上顶点
            Vec3f LeftTop, RightTop, LeftBottom;
            int max_dis,rt_i,lb_i;
            max_dis = rt_i = lb_i = 0;
            for (int j = 0; j < circles.size(); j++) {
                for (int k = 0; k < circles.size(); k++) {
                    float distance = sqrt(pow(circles[j][0] - circles[k][0], 2) + pow(circles[j][1] - circles[k][1], 2));
                    if(distance > max_dis){
                        max_dis = distance;
                        RightTop = circles[j];
                        LeftBottom = circles[k];
                        rt_i = j;
                        lb_i = k;
                    }
                }
            }
            //垂直确定左上顶点
            for (int j = 0; j < circles.size(); j++) {
                if(j != rt_i && j != lb_i){
                    float theta = atan2(RightTop[0] - circles[j][0], RightTop[1] - circles[j][1]) - atan2(LeftBottom[0] - circles[j][0], LeftBottom[1] - circles[j][1]);
                    theta = theta * 180.0 / CV_PI;
                    theta = (theta < 0) ? (theta + 360.0) : theta;
                    if(fabs(theta - 90) < 10 || fabs(theta - 270) < 10){
                        LeftTop = circles[j];
                        break;
                    }
                }
            }
            //调整左上和右上顶点
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
                cout << "--------------旋转角度检测异常" << endl;
                break;
            }
            if (!flag) {
                Vec3f temp = RightTop;
                RightTop = LeftBottom;
                LeftBottom = temp;
            }
            
            cv::circle(srcImage, Point(LeftTop[0],LeftTop[1]), 2, Scalar(255,0,0),2);
            cv::circle(srcImage, Point(RightTop[0],RightTop[1]), 2, Scalar(255,0,0),2);
            cv::circle(srcImage, Point(LeftBottom[0],LeftBottom[1]), 2, Scalar(255,0,0),2);
            
            Landmark tempLandmark;
            tempLandmark.setLeftTop(LeftTop);
            tempLandmark.setRightTop(RightTop);
            tempLandmark.setLeftBottom(LeftBottom);
            tempLandmark.setCircles(circles);
            landmarks.push_back(tempLandmark);
        }
    }
    
    vector<LocalizeData> localizeDatas;
    if(landmarks.size() > 0){
        
        //for(int i = 0; i < landmarks.size(); i++){
            
            Vec3f LeftTop = landmarks[0].getLeftTop();
            Vec3f RightTop = landmarks[0].getRightTop();
            Vec3f LeftBottom = landmarks[0].getLeftBottom();
            
            //矫正三点(图像中三点可能不垂直)
            if (LeftTop[0] == LeftBottom[0]) {
                RightTop[1] = LeftTop[1];
                //cout << "定位点坐标矫正1" << endl;
            }
            else if (LeftTop[1] == LeftBottom[1]) {
                RightTop[0] = LeftTop[0];
                //cout << "定位点坐标矫正2" << endl;
            }
            else if (LeftTop[0] == RightTop[0]) {
                LeftBottom[1] = LeftTop[1];
                //cout << "定位点坐标矫正3" << endl;
            }
            else if (LeftTop[1] == RightTop[1]) {
                LeftBottom[0] = LeftTop[0];
                //cout << "定位点坐标矫正4" << endl;
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
            theta_p = (theta_p < 0) ? ( 360.0 + theta_p ) : (theta_p);
            
            /* 获取id */
            int size = dim * dim - 4;
            int bits[size];
            //放射变换后定位点对应的新坐标，theta旋转角度为逆时针
            vector<Vec3f> circles = landmarks[0].getCircles();
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
                cout << "--------------ID识别异常" << endl;
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
        //}
    }
    
    //选取一个路标z为参考
    if(localizeDatas.size() > 0){
        float v_x = 0, v_y = 0, v_theta = 0;
        //int num = 0;
        //for(int i = 0; i < localizeDatas.size(); i++){
            map<int,double*>::iterator iter;
            iter = globalLandmarks.find(localizeDatas[0].getLandmarkId());
            if(iter != globalLandmarks.end()){
                v_x += iter->second[0] + localizeDatas[0].getVisualX();
                v_y += iter->second[1] + localizeDatas[0].getVisualY();
                v_theta += localizeDatas[0].getVisualTheta();
                //num++;
                LocalizeData final_data;
                final_data.setLandmarkId(localizeDatas[0].getLandmarkId());
                final_data.setVisualTheta(v_theta);
                final_data.setVisualX(v_x);
                final_data.setVisualY(v_y);
                final_data.setDTheta(v_theta - past_theta);
                
                //保存上一帧
                past_theta = v_theta;
                preVisualData.setVisualTheta(final_data.getVisualTheta());
                preVisualData.setVisualX(final_data.getVisualX());
                preVisualData.setVisualY(final_data.getVisualY());
                return final_data;
            }else{
                //cout << "--------------id:" << localizeDatas[0].getLandmarkId() << " 配置异常" <<endl;
                preVisualData.setLandmarkId(0);
                return preVisualData;
            }
        //}
    }else{
        //cout << "--------------未检测到路标" << endl;
        preVisualData.setLandmarkId(-1);
        return preVisualData;
    }
}
