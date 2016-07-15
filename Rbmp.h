#ifndef __RRRbmp_H__
#define __RRRbmp_H__
#include "Rcomdef.h"
#pragma pack(2)  
using namespace std; 
//下面两个结构是位图的结构  
typedef struct BITMAPFILEHEADER  
{   
	RUInt16 bfType;   
	RUInt32 bfSize;   
	RUInt16 bfReserved1;   
	RUInt16 bfReserved2;   
	RUInt32 bfOffBits;   
}BITMAPFILEHEADER;   

typedef struct BITMAPINFOHEADER  
{   
	RUInt32 biSize;   
	RUInt32 biWidth;   
	RUInt32 biHeight;   
	RUInt16 biPlanes;   
	RUInt16 biBitCount;   
	RUInt32 biCompression;   
	RUInt32 biSizeImage;   
	RUInt32 biXPelsPerMeter;   
	RUInt32 biYPelsPerMeter;   
	RUInt32 biClrUsed;   
	RUInt32 biClrImportant;   
}BITMAPINFODEADER; 

typedef struct rgb_pixel_t{
	RUInt8 blue;
	RUInt8 green;
	RUInt8 red;
	RUInt8 alpha;
} rgb_pixel_t;

void showBmpHead(BITMAPFILEHEADER &pBmpHead);
void showBmpInforHead(BITMAPINFODEADER &pBmpInforHead);
void getGrayBMPInfo(const Rchar *path);
RInt32 loadImgFromGrayBMP(RUInt8 *dst,RUInt32 dstLen,RUInt32 *imgWidth,RUInt32 *imgHeight,const Rchar *path);
RInt32 saveGrayBMP(const Rchar *path,RUInt8 *data,RUInt32 w,RUInt32 h);
#endif

