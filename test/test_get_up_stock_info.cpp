#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <boost/locale/encoding.hpp>
#define HTTP_URL "http://hq.sinajs.cn/list="
#define MAX_CODE_NUM 1000

std::string RETSULT;

struct stock_info
{
    //代码标识
    std::string code_name;
    //中文名词
    std::string name_zh;
    //今日开盘价格
    float price_open_today;
    //昨日收盘价格
    float price_close_lastday;
    //当前价格
    float price_current;
    //今日最高价
    float price_max;
    //今日最低价
    float price_min;
    //成交的股票数，除以100为手
    int total_num_turnover;
    //成交金额，单位元
    double total_money_turnover;

    std::string date_str;
    std::string time_str;

};

std::string UTF8toGBK(const std::string & str)
{
    return boost::locale::conv::between(str, "GBK", "UTF-8");
}

std::string GBKtoUTF8(const std::string & str)
{
    return boost::locale::conv::between(str, "UTF-8", "GBK");
}

//字符串分割
void strSplit(const std::string &s, std::vector<std::string> &v, const std::string &c)
{
    std::string::size_type pos1, pos2;
    if(s.size() == 0)
        return;
    pos2 = s.find(c);
    pos1 = 0;
    while (std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));
};

bool vector2StockInfo(std::vector<std::string> &v, struct stock_info *info)
{   
    if(v.size() <10)
    {
        std::cerr<<"当前数组中数据信息不完整，退出转换！"<<std::endl;
        return false;
    }
    //形如 var hq_str_sz000998="隆平高科, 样式
    int index0= v[0].find_last_of("_")+1;
    int index1= v[0].find_first_of("=");
    std::string codeName = v[0].substr(index0,index1-index0);
    std::string name_zh = v[0].substr(index1+1+1,v[0].size()-index1);
    info->code_name = codeName;
    info->name_zh = name_zh;
    info->price_open_today = atof(v[1].c_str());
    info->price_close_lastday = atof(v[2].c_str());
    info->price_current = atof(v[3].c_str());
    info->price_max = atof(v[4].c_str());
    info->price_min = atof(v[5].c_str());
    info->total_num_turnover = atoi(v[8].c_str());
    info->total_money_turnover = atof(v[9].c_str());
    
    //日期时间
    if(v.size() >=32)
    {
        info->date_str = v[30];
        info->time_str = v[31];
    }
    return true;

};

//这个函数是为了符合CURLOPT_WRITEFUNCTION而构造的  完成数据保存功能
size_t handle_data_callback(void *ptr, size_t size, size_t nmemb, std::string &stream)  
{
    //std::cout<<"[size]"<<size<<std::endl;
    //std::cout<<"[nmemb]"<<nmemb<<std::endl;
    const char * con_ptr = (char*)ptr;
    stream.assign(con_ptr,nmemb);
    stream = GBKtoUTF8(stream);//这样转完后可正常显示中文
    //std::cout<<"[data]"<<stream<<std::endl;
    return size;
};

CURL * initCurl()
{
    CURL *curl;                     //定义CURL类型的指针
    CURLcode res; 

    curl = curl_easy_init();        //初始化一个CURL类型的指针
    if(curl!=NULL)
    {
        //将返回的http头输出字符串，采用的方式数据可以返回 
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &RETSULT); 
        //CURLOPT_WRITEFUNCTION 将后继的动作交给write_data函数处理
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_data_callback);     
    }
    return curl;
};

std::string getStockDataForCode2(CURL *curl, std::string code)
{
    std::string ret;
    CURLcode res; 
    if(curl!=NULL)
    {
        std::string url = HTTP_URL+code;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());  
        res = curl_easy_perform(curl);
        if(res !=0)
        {
            ret = RETSULT;
        } 
    }
    return ret;
};


std::string getStockDataForCode(std::string code)
{
    std::string result;
    CURL *curl;                     //定义CURL类型的指针
    CURLcode res; 
    curl = curl_easy_init();        //初始化一个CURL类型的指针
    if(curl!=NULL)
    {
        //设置curl选项. 其中CURLOPT_URL是让用户指 定url. argv[1]中存放的命令行传进来的网址
        std::string url = HTTP_URL+code;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());  
        //将返回的http头输出字符串，采用的方式数据可以返回 
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result); 
        //CURLOPT_WRITEFUNCTION 将后继的动作交给write_data函数处理
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_data_callback);     
        //调用curl_easy_perform 执行我们的设置.并进行相关的操作. 在这 里只在屏幕上显示出来.
        res = curl_easy_perform(curl);
        //清除curl操作.
        if(res !=0)
        {
            curl_easy_cleanup(curl);
        }
        
    }
    return result;
};

bool getStockInfo(CURL *curl, const std::string &code,struct stock_info *info)
{
    //std::cout<<"[getStockInfo]:the code="<<code<<std::endl;
    std::vector<std::string> v;
    //std::string infoStr = getStockDataForCode(code);
    std::string infoStr = getStockDataForCode2(curl,code);
    strSplit(infoStr,v,",");
    return vector2StockInfo(v,info);
};

// 以600/601/603/605开头-总计6位
//返回总个数
int genSHCode(std::vector<std::string> &codeList)
{
    std::string preCode = "sh";
    int num_600 = 600*1000;
    int num_601 = 601*1000;
    int num_603 = 603*1000;
    int num_605 = 605*1000;
    for(int i=0;i<MAX_CODE_NUM;i++)
    {
        int num = num_600+i;
        std::string tmp = preCode+std::to_string(num);
        codeList.push_back(tmp);
    }

    for(int i=0;i<MAX_CODE_NUM;i++)
    {
        int num = num_601+i;
        std::string tmp = preCode+std::to_string(num);
        codeList.push_back(tmp);
    }

    for(int i=0;i<MAX_CODE_NUM;i++)
    {
        int num = num_603+i;
        std::string tmp = preCode+std::to_string(num);
        codeList.push_back(tmp);
    }

    for(int i=0;i<MAX_CODE_NUM;i++)
    {
        int num = num_605+i;
        std::string tmp = preCode+std::to_string(num);
        codeList.push_back(tmp);
    }
    return codeList.size();
};

//深圳 以000开头 001 002 003
int genSZCode(std::vector<std::string> &codeList)
{
    std::string preCode = "sz000";
    std::string preCode1 = "sz001";
    std::string preCode2 = "sz002";
    std::string preCode3 = "sz003";
    codeList.push_back("sz001000");
    codeList.push_back("sz002000");
    codeList.push_back("sz003000");

    for(int i=1;i<MAX_CODE_NUM;i++)
    {
        std::string tmp,tmp1,tmp2,tmp3;
        if(i<10)
        {
            tmp = preCode+"00"+std::to_string(i);
            tmp1 = preCode1+"00"+std::to_string(i);
            tmp2 = preCode2+"00"+std::to_string(i);
            tmp3 = preCode3+"00"+std::to_string(i);
        }
            
        else if(i<100)
        {
            tmp = preCode+"0"+std::to_string(i);
            tmp1 = preCode1+"0"+std::to_string(i);
            tmp2 = preCode2+"0"+std::to_string(i);
            tmp3 = preCode3+"0"+std::to_string(i);
        }
            
        else
        {
            tmp = preCode+std::to_string(i);
            tmp1 = preCode1+std::to_string(i);
            tmp2 = preCode2+std::to_string(i);
            tmp3 = preCode3+std::to_string(i);
        }
            
        codeList.push_back(tmp);
        codeList.push_back(tmp1);
        codeList.push_back(tmp2);
        codeList.push_back(tmp3);
    }
    return codeList.size();
};

//创业板 以300开头
int genCYCode(std::vector<std::string> &codeList)
{
    std::string preCode = "sz";
    int sz_300 = 300*1000;
    for(int i=0;i<MAX_CODE_NUM;i++)
    {
        int num = sz_300+i;
        std::string tmp = preCode+std::to_string(num);
        codeList.push_back(tmp);
    }

    preCode = "sh";
    int sh_688 = 688*1000;
    for(int i=0;i<MAX_CODE_NUM;i++)
    {
        int num = sh_688+i;
        std::string tmp = preCode+std::to_string(num);
        codeList.push_back(tmp);
    }
    return codeList.size();
};

void printInfo(struct stock_info info)
{
    printf("----------------------------------\n");
    std::cout<<"[ 代码标识]："<<info.code_name<<std::endl;
    std::cout<<"[ 中文名称]："<<info.name_zh<<std::endl;
    std::cout<<"[今日开盘价]："<<info.price_open_today<<std::endl;
    std::cout<<"[昨日收盘价]："<<info.price_close_lastday<<std::endl;
    std::cout<<"[ 当 前 价]："<<info.price_current<<std::endl;
    std::cout<<"[今日最高价]："<<info.price_max<<std::endl;
    std::cout<<"[今日最低价]："<<info.price_min<<std::endl;
    std::cout<<"[成交量-股]："<<info.total_num_turnover<<std::endl;
    std::cout<<"[今日日期]："<<info.date_str<<std::endl;
    std::cout<<"[当前时刻]："<<info.time_str<<std::endl;
    printf("----------------------------------\n");

};

int main(int argc, char *argv[])
{
    CURL *curl;
    std::string stock_code;
    std::vector<std::string> codeList;//存放编码
    std::vector<struct stock_info> stockList;//存放今日上涨数据
    
    if(argc==2)
    {
        stock_code.assign(argv[1],strlen(argv[1]));
    }

    //step1: 构造A股股票信息
    //此处可以只查看某一支股票信息也可以查看A股所有
    if(stock_code.size() == 0)
    {
        int size0 = genSZCode(codeList);
        int size1 = genSHCode(codeList);
        int size2 = genCYCode(codeList);
    }else{
        codeList.push_back(stock_code);
    }
    //step2: 初始化curl接口
    curl = initCurl();

    //step3: 遍历已有的股票代码获取实时数据
    for(int i=0;i<codeList.size();i++)
    {
        struct stock_info info;
        bool flag = getStockInfo(curl, codeList.at(i), &info);
        if(flag)
        {
            //std::cout<<"获取代码为["<<codeList[i]<<"],股票信息成功。"<<std::endl;
            if(info.price_current > info.price_close_lastday)
            {
                stockList.push_back(info);
            }
        }
        else{
            std::cout<<"[ERROR]:获取代码为["<<codeList[i]<<"],股票信息失败，请检查！"<<std::endl;
        }
    }
    
    //step4: curl清空关闭
    if(curl)
    {
        curl_easy_cleanup(curl);
    }
    
    //step5: 今日上涨股票进行打印
    std::cout<<"当前共有"<<stockList.size()<<"支代码今日上涨"<<std::endl;
    for(int i=0;i<stockList.size();i++)
    {
        std::cout<<"第["<<i+1<<"] 支上涨的股票信息:"<<std::endl;
        printInfo(stockList[i]);
    }

    //step6: game over
    std::cout<<"系统运行结束"<<std::endl;

    return 0;
}