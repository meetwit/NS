#include "iostream"
#include "NSCPP.h"

NS::NS()
{
    ns.first[0] = 1;
    ns.first[1] = 1;
    ns.first[2] = 1;
    ns.minValue = -1;
    ns.maxValue = -1;
}

NS::~NS()
{
}

int NS::adcvLimit(int adcv)
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

void NS::setLimit(int minValue, int maxValue)
{
    ns.minValue = minValue;
    ns.maxValue = maxValue;
}

/*
������:     NS_zone
��  ��:     adc_v:    adc�ɼ���ֵ��0,4096��
����ֵ:     ״̬����     = 0  ��һ�γ�ʼ��
                        = 3  �ȴ����죬�����״γ�ʼ��
                        = 1  ��ֵΪ�͵�����
                        = 2  ��ֵΪ�ߵ�����
                        = 10 �״ν�������򣬿���Ϊ����ֵ
                        = 20 �״ν�������򣬿���Ϊ����ֵ
˵  ��:     ��ʶ�𿪹�ʱ������ֱ��ʹ�÷���ֵ10 or 20��
            ��ʱ�ֱ����͸�����ѡ������һ������ʶ�𿪹�
*/
int NS::NS_zone(int adc_v)
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

double NS::NS_zone_p(void)
{
    double rtv;
    ns.N_S != 0 ? rtv = (ns.adcv - ns.S_v) / (double)ns.N_S : rtv = 0;
    return rtv;
}