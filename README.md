# stock_info
### 0.工程说明

实时获取A股信息，查看上涨股票行情

A股信息代码是靠程序自动生成的，因此存在号码段没有股票的情况

测试时为了缩短测试时间，把下面的值改为10 ，则股票各个代码最大到10，最大是1000
修改 #define MAX_CODE_NUM 10  //test_get_up_stock_info.cpp

### 1.程序编译

```
mkdir -p build && cd build
cmake ..  && make
```

### 2.程序运行

```
cd build/bin

运行一：查看当前深交所和沪交所 所有股票的上涨详情
./up_stock_info

运行二：
./up_stock_info 具体的代码
例如： 
./up_stock_info sz000001  //查看深交所 中国平安 当前是否上涨
./up_stock_info sh601006  //查看沪交所 大秦铁路 当前是否上涨
```

