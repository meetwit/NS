/*
 * NS 动态识别传感器算法
 * 用途:    识别柔性传感器的特征点，直接使用可作为开关识别，
 *         增加中间态的处理可作为连续拉伸识别
 * 移植:    复制NS类，MIN_STEP宏定义，NS_zone函数即可
 * 作者:    meetwit
 * 首版本时间:     2019年08月21日19:48:08
 * 版本:
 *         V1.0.7    将类的定义已经实现分别放在H文件以及CPP文件
 *         V1.0.6    增加设置最大值最小值的接口setLimit
 *         V1.0.5    增加百分比输出NS_zone_p
 *         V1.0.4    C++版本使用更标准的写法，使用class定义数据以及功能，将NS功能体现在H文件
 *         V1.0.3    定义C++版本
 *         V1.0.2    增加结构体成员adcv，使得结构体具有adc采集的数值，delete函数中5的返回值
 *         V1.0.1    将静态变量first改为结构体成员，实现多个检测重入问题
 *         V1.0.0    首次建立版本，实现单个开关检测
 */

#ifndef NSCPP_H
#define NSCPP_H

#include "stdio.h"
/*
MIN_STEP adc采用作为单位，设置为拉伸一次传感器，量程的一半即可
*/
#define MIN_STEP 100

class NS
{
private:
    /* data */
    struct NSData
    {
        int adcv;
        int N_v;      // 动态最大值
        int S_v;      // 动态最小值
        int N_S;      // 动态量程
        int state_NS; // 状态，高低状态
        int first[3]; // 0：初始化标志位， 1&2：高低翻转状态位
        int minValue; // 最大值
        int maxValue; // 最小值
    };
    struct NSData ns;

public:
    NS();                    // 构造函数，初始化结构体变量
    ~NS();                   // 析构函数
    int adcvLimit(int adcv); // 设置adcv范围，内部NS_zone调用
    void setLimit(int minValue, int maxValue); // 设置adcv最大值最小值
    int get_N_v() { return ns.N_v; }           // 获取动态最大值
    int get_S_v() { return ns.S_v; }           // 获取动态最小值
    int get_N_S() { return ns.N_S; }           // 获取动态量程
    int NS_zone(int adc_v);                    // 输入adcv，动态划分区域
    double NS_zone_p(void);                    // 返回拉伸量百分比%
};

#endif