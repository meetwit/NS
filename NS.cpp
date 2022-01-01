/*
 * NS 动态识别传感器算法演示代码
 * 实际调用只需要定义类NS，然后调用NS_zone(adcv)即可
 */

#include "iostream"
#include "NSCPP.h"

int algData[] = {0, 200, 50, 250, 0, 200, 50, 250, 0};

void showMsg(int j, int i, int p, NS ns)
{
    std::cout << "j=" << j << ",i=" << i << ",output=" << p;
    std::cout << ",N_S=" << ns.get_N_S() << ",N_v=" << ns.get_N_v() << ",S_v=" << ns.get_S_v();
    std::cout << ",%=" << ns.NS_zone_p() << ",\r\n";
}

void scope(NS &ns2, int start, int end)
{
    static int j = 0;
    int adcv = start, p = 0;
    j++;
    int dis = 0;
    start > end ? dis = -1 : dis = 1;
    while (adcv != end)
    {
        p = ns2.NS_zone(adcv);
        showMsg(j, adcv, p, ns2);
        adcv += dis;
    }
}
int main()
{
    NS ns2;
    int last = -1;
    for (auto data : algData)
    {
        if (last != -1)
        {
            scope(ns2, last, data);
        }
        last = data;
    }
    return 0;
}
