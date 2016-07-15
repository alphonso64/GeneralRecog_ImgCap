#ifndef __NetParam_H__
#define __NetParam_H__
#include "Rcomdef.h"

#define  GROUP_A 6
#define  CONVN_GROUP_A 225
#define  GROUP_B 12
#define  CONVN_GROUP_B 25
#define  FFB 4
#define  FFW 432
#define  CONVNSTEP_A 15
#define  CONVNSTEP_B  5
#define  DOWNSTEP  3


using namespace std; 

class NetParam{
	public:
	double layer2_K[GROUP_A][CONVN_GROUP_A];
	double layer2_B[GROUP_A];
	
	double layer4_K[GROUP_A][GROUP_B][CONVN_GROUP_B];
	double layer4_B[GROUP_B];
	double p_ffb[FFB];
	double p_ffw[FFB][FFW];
	
	int initParam(const char *path);
	int saveParam(const char *path);
	int loadParam(const char *path);
	
	private:
	int initLayer2(const char *path);
	int initLayer4(const char *path);
	int initFF(const char *path);

	
};
#endif