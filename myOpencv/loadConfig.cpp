#include "loadConfig.h"

map<int,double*> globalLandmarks;
map<string,double> globalParams;

bool isSpace(char c) // 空格or制表符s
{
    if (' ' == c || '\t' == c)
        return true;
    return false;
}

void trim(string & str)
{
    if (str.empty()) {
        return;
    }
    int i, start_pos, end_pos;
    for (i = 0; i < str.size(); ++i) {
        if (!isSpace(str[i])) {
            break;
        }
    }
    if (i == str.size()) { // 全部是空白字符串
        str = "";
        return;
    }
    
    start_pos = i;
    
    for (i = str.size() - 1; i >= 0; --i) {
        if (!isSpace(str[i])) {
            break;
        }
    }
    end_pos = i;
    
    str = str.substr(start_pos, end_pos - start_pos + 1);
}

void loadConfig(){
    
    clock_t startTime, endTime;
    
    //加载landmark信息
    startTime = clock();
    ifstream infile_1(landmarks_path);
       if(!infile_1){
           cout<<"landmarks配置文件打开失败"<<endl;
       }
       string line_1;
       while(getline(infile_1,line_1)){
           int id;
           double *value = new double[2];
           if(!line_1.empty()){
               int start_pos = 0, end_pos, interval_pos;
               if((interval_pos = line_1.find(":")) != -1){
                   if(interval_pos != 0){
                       end_pos = interval_pos - 1;
                       string key = line_1.substr(start_pos,end_pos-start_pos+1);
                       id = atoi(key.c_str());
                   }
               }
               start_pos = interval_pos + 1;
               if((interval_pos = line_1.find(",")) != -1){
                   if(interval_pos != 0){
                       end_pos = interval_pos - 1;
                       string x_value = line_1.substr(start_pos,end_pos-start_pos+1);
                       trim(x_value);
                       double x = atof(x_value.c_str());
                       value[0] = x;
                       
                       start_pos = interval_pos + 1;
                       end_pos = line_1.size() - 1;
                       string y_value = line_1.substr(start_pos,end_pos-start_pos+1);
                       trim(y_value);
                       double y = atof(y_value.c_str());
                       value[1] = y;
                   }
               }
               globalLandmarks.insert(pair<int,double *>(id,value));
           }
       }
       infile_1.close();
       endTime = clock();//计时结束
       cout << "已加载landmarkes配置文件: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    
    //加载参数信息
    startTime = clock();
    ifstream infile_2(params_path);
       if(!infile_2){
           cout<<"params配置文件打开失败"<<endl;
       }
       string line_2;
       while(getline(infile_2,line_2)){
           string key;
           double value;
           if(!line_2.empty()){
               int start_pos = 0, end_pos, interval_pos;
               if((interval_pos = line_2.find(":")) != -1){
                   if(interval_pos != 0){
                       end_pos = interval_pos - 1;
                       key = line_2.substr(start_pos,end_pos-start_pos+1);
                       trim(key);
                   }
               }
               if((start_pos = interval_pos + 1) <= (end_pos = line_2.size() - 1)){
                   string ori_value = line_2.substr(start_pos,end_pos-start_pos+1);
                   trim(ori_value);
                   value = atof(ori_value.c_str());
               }
               globalParams.insert(pair<string,double>(key,value));
           }
       }
       infile_2.close();
       endTime = clock();//计时结束
       cout << "已加载params配置文件: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
    
}
