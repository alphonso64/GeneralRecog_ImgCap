#ifndef __RecognizerII_H__
#define __RecognizerII_H__
#include "NetParam.h"

#define C1SIZE 66*66
#define S1SIZE 22*22
#define C2SIZE 18*18
#define S2SIZE 6*6

class Recognizer{
public:
	int init();
	Recognizer();
	int recognize(void *src,RUInt32 w,RUInt32 h,RUInt32 format );

private:
	int initFlag;//0 成功 -1未初始化 -2失败
	NetParam netparam;
	RUInt32 maxRecWidth;
	RUInt32 maxRecHeight;
//	double img[6400];
	double c1[GROUP_A][C1SIZE];
	double s1[GROUP_A][S1SIZE];

	
	double c2[GROUP_B][C2SIZE];
	double s2[GROUP_B][S2SIZE];
	double s2_temp[FFW];
//	double c3[FFB];
};
#endif