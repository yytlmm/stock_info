#ifndef _STOCK_INFO_H
#define _STOCK_INFO_H
#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#define HTTP_URL "http://hq.sinajs.cn/list="

namespace stock_info
{
    using std::string;
    using std::vector;
    using std::map;

    class StockInfo
    {
        public:
            StockInfo();
            ~StockInfo();


        private:
            int num;


    };
}//end of ns
#endif