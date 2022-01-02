/*
 * NS 动态识别传感器算法
 * 用途:    识别柔性传感器的特征点，直接使用可作为开关识别，
 *         增加中间态的处理可作为连续拉伸识别
 * 移植:    复制NS类，MIN_STEP宏定义，NS_zone函数即可
 * 作者:    meetwit
 * 首版本时间:     2019年08月21日19:48:08
 * 版本:
 *         V1.0.6    增加设置最大值最小值的接口setLimit
 *         V1.0.5    增加百分比输出NS_zone_p
 *         V1.0.4    C++版本使用更标准的写法，使用class定义数据以及功能，将NS功能体现在H文件
 *         V1.0.3    定义C++版本
 *         V1.0.2    增加结构体成员adcv，使得结构体具有adc采集的数值，delete函数中5的返回值
 *         V1.0.1    将静态变量first改为结构体成员，实现多个检测重入问题
 *         V1.0.0    首次建立版本，实现单个开关检测
 */

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
        int N_v;      //动态最大值
        int S_v;      //动态最小值
        int N_S;      //动态量程
        int state_NS; //状态
        int first[3];
        int minValue;
        int maxValue;
    };
    struct NSData ns;
    int adcvLimit(int adcv)
    {
        if (ns.minValue != -1)
        {
            if (adcv < ns.minValue)
            {
                adcv = ns.minValue;
            }
        }
        if (ns.maxValue != -1)
        {
            if (adcv > ns.maxValue)
            {
                adcv = ns.maxValue;
            }
        }
        return adcv;
    }

public:
    NS()
    {
        ns.first[0] = 1;
        ns.first[1] = 1;
        ns.first[2] = 1;
        ns.minValue = -1;
        ns.maxValue = -1;
    }
    void setLimit(int minValue, int maxValue)
    {
        ns.minValue = minValue;
        ns.maxValue = maxValue;
    }
    int get_N_v() { return ns.N_v; }
    int get_S_v() { return ns.S_v; }
    int get_N_S() { return ns.N_S; }
    /*
    函数名:     NS_zone
    传  参:     adc_v:    adc采集数值【0,4096】
    返回值:     状态描述     = 0  第一次初始化
                            = 3  等待拉伸，属于首次初始化
                            = 1  数值为低的区域
                            = 2  数值为高的区域
                            = 10 首次进入低区域，可作为开关值
                            = 20 首次进入高区域，可作为开关值
    说  明:     当识别开关时，可以直接使用返回值10 or 20，
                此时分别进入低高区域，选择其中一个即可识别开关
    */
    int NS_zone(int adc_v)
    {

        ns.adcv = adcvLimit(adc_v);

        /*step 1 : first init*/
        if (ns.first[0])
        {
            ns.first[0] = 0;
            ns.N_v = ns.adcv;
            ns.S_v = ns.adcv;
            ns.N_S = 0;
            return 0; // first init
        }

        /*step 2 : init N_v & S_v*/
        if (ns.N_S < MIN_STEP)
        {
            if (ns.N_v < ns.adcv)
            {
                ns.N_v = ns.adcv;
                ns.N_S = ns.N_v - ns.S_v;
            }
            if (ns.S_v > ns.adcv)
            {
                ns.S_v = ns.adcv;
                ns.N_S = ns.N_v - ns.S_v;
            }

            return 3; // with refresh
        }

        /*step 3 : find state_NS and refresh N_S*/
        if (ns.adcv > (ns.N_S * 0.67 + ns.S_v))
        {
            ns.state_NS = 2;
            if (ns.first[2])
            {
                ns.first[2] = 0;
                ns.first[1] = 1;
                ns.N_S = ns.N_v - ns.S_v;
                ns.N_v = ns.adcv;
                return 20;
            }
            else
            {
                if (ns.adcv > ns.N_v)
                {
                    ns.N_v = ns.adcv;
                    ns.N_S = ns.N_v - ns.S_v;
                }
                return 2;
            }
        }
        else if (ns.adcv < (ns.N_S * 0.33 + ns.S_v))
        {
            ns.state_NS = 1;
            if (ns.first[1])
            {
                ns.first[1] = 0;
                ns.first[2] = 1;
                ns.N_S = ns.N_v - ns.S_v;
                ns.S_v = ns.adcv;
                return 10;
            }
            else
            {
                if (ns.adcv < ns.S_v)
                {
                    ns.S_v = ns.adcv;
                    ns.N_S = ns.N_v - ns.S_v;
                }
                return 1;
            }
        }
        else
        {
            return 4;
        }
    }
    // NS_zone_p 返回拉伸量百分比%
    double NS_zone_p(void)
    {
        double rtv;
        ns.N_S != 0 ? rtv = (ns.adcv - ns.S_v) / (double)ns.N_S : rtv = 0;
        return rtv;
    }
};
