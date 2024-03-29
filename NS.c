/*
NS.c 动态识别传感器算法
用途：	识别柔性传感器的特征点，直接使用可作为开关识别，
		增加中间态的处理可作为连续拉伸识别
移植：	复制NS结构体，MIN_STEP宏定义，NS_zone函数即可
时间：	2019年8月21日19:48:08
作者：	meetwit		 
版本：	NS当前版本V1.0.2
		V1.0.0	首次建立版本，实现单个开关检测
		V1.0.1	将静态变量first改为结构体成员，实现多个检测重入问题
		V1.0.2	增加结构体成员adcv，使得结构体具有adc采集的数值，delete函数中5的返回值
*/

#include "stdio.h"

/*
MIN_STEP adc采用作为单位，设置为拉伸一次传感器，量程的一半即可 
*/
#define MIN_STEP 100

struct NS{
	int adcv;
	int N_v;			//动态最大值 
	int S_v;			//动态最小值 
	int N_S;			//动态量程 
	int state_NS;		//状态
	int first[3];
};

/*
函数名：	NS_zone
传	参：	ns1：	算法识别结构体
			adc_v：	adc采集数值【0,4096】
返回值：	状态描述 	= 0 第一次初始化
						= 3 等待拉伸，属于首次初始化
						= 1 数值为低的区域
						= 2 数值为高的区域
						=10 首次进入低区域，可作为开关值 
						=20 首次进入高区域，可作为开关值 
说	明：	当识别开关时，可以直接使用返回值10&20，
			此时分别进入低高区域，选择其中一个即可识别开关 
*/	
int NS_zone(struct NS * ns,int adc_v){

	ns->adcv = adc_v;

	/*step 1 : first init*/
	if(ns->first[0]){
		ns->first[0]=0;
		ns->N_v=ns->adcv;
		ns->S_v=ns->adcv;
		ns->N_S=0;
		return 0;	//first init
	}
	
	/*step 2 : init N_v & S_v*/
	if(ns->N_S<MIN_STEP){
		if(ns->N_v<ns->adcv){
			ns->N_v = ns->adcv;
			ns->N_S = ns->N_v - ns->S_v;
		}
		if(ns->S_v>ns->adcv){
			ns->S_v = ns->adcv;
			ns->N_S = ns->N_v - ns->S_v;
		}
	
		return 3;	//with refresh
	}
	
	/*step 3 : find state_NS and refresh N_S*/
	if(ns->adcv>(ns->N_S*0.67+ns->S_v)){
		ns->state_NS = 2;
		if(ns->first[2]){
			ns->first[2]=0;
			ns->first[1]=1;
			ns->N_S = ns->N_v - ns->S_v; 
			ns->N_v = ns->adcv;
			return 20;
		}else{
			if(ns->adcv>ns->N_v){
				ns->N_v = ns->adcv;
				ns->N_S = ns->N_v - ns->S_v; 
			}
			return 2;
		}
	}else if(ns->adcv<(ns->N_S*0.33+ns->S_v)){
		ns->state_NS = 1;
		if(ns->first[1]){
			ns->first[1]=0;
			ns->first[2]=1;
			ns->N_S = ns->N_v - ns->S_v; 
			ns->S_v  = ns->adcv;
			return 10;
		}else{
			if(ns->adcv<ns->S_v){
				ns->S_v = ns->adcv;
				ns->N_S = ns->N_v - ns->S_v; 
			}
			return 1;
		}
	}else{
		return 4;
	}
	
}

void scope(struct NS * ns,int start,int end){
	static int j=0;
	int i=0,p=0;
	j++;
	
	if(start<end)
	for(i=start;i<end;i++){
		p = NS_zone(ns,i);
		printf("j=%d,i=%d,output=%d,N_S=%d,N_v=%d,S_v=%d,\r\n",j,i,p,ns->N_S,ns->N_v,ns->S_v);
	}
	if(end<start)
	for(i=start;i>end;i--){
		p = NS_zone(ns,i);
		printf("j=%d,i=%d,output=%d,N_S=%d,N_v=%d,S_v=%d,\r\n",j,i,p,ns->N_S,ns->N_v,ns->S_v);
	}
}

int main(){
	
	struct NS ns2={
		.first={1,1,1}
	};
	
	scope(&ns2,0,200);
	scope(&ns2,200,50);
	scope(&ns2,50,250);
	scope(&ns2,250,0);
	scope(&ns2,0,200);
	scope(&ns2,200,50);
	scope(&ns2,50,250);
	scope(&ns2,250,0);
	
	return 0;
} 
