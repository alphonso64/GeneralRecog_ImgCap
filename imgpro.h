#ifndef IMGPRO_H_INCLUDED
#define IMGPRO_H_INCLUDED

#include "sdf.h"
#include <curl/curl.h>
#include <QImage>
#include <QPainter>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
typedef enum FTP_STATE
{
    FTP_UPLOAD_SUCCESS,
    FTP_UPLOAD_FAILED,
    FTP_DOWNLOAD_SUCCESS,
    FTP_DOWNLOAD_FAILED
}FTP_STATE;

/*FTP OPERATIONS OPTIONS*/
typedef struct FTP_OPT
{
    char *url;      /*url of ftp*/
    char *user_key;     /*username:password*/
    char *file;     /*filepath*/
}FTP_OPT;


void fileSync(const char *file);

void getGrayImageFromYUYV(uint8 *src,int w,int h,uint8 *dst);

void convert_YUYV_to_RGB24_(int width, int height, const uint8* yuyv_image, uint8 *rgb_image);

void convert_YUYV_to_RGB24_Y(int width, int height, const uint8* yuyv_image, uint8 *rgb_image,uint8 *graybmp);

void drawRectangle(uint8* src,int  width,int height,int x,int y,int recWidth,int recHeight);

void drawRectangle(QImage *img,int x,int y,int recWidth,int recHeight);

void cleanImg(QImage *img,int recWidth,int recHeight);

void getBMPReczone(uint8* src,int  width,int height,int x,int y,int recWidth,int recHeight,uint8 *dst);

void getGRAYReczone(uint8* src,int  width,int height,int x,int y,int recWidth,int recHeight,uint8 *dst);

void getdownSample(unsigned char *output,int *w,int *h,unsigned char *src,int srcW,int srcH,int recWidth,int recHeight);

void ResizeLinear(unsigned char *output,int dstW,int dstH,unsigned char *src,int srcW,int srcH);

int detectThreshold(int nMaxIter , int &nDiffRet,unsigned char *graySrc,int w,int h);

int saveBMPFromGrayImg(const char *path,unsigned char *data,int w,int h);

int saveBMPFromRGB888Img(const char *path,unsigned char *data,int w,int h);

int saveYUYVImg(const char *path,unsigned char *data,int w,int h);

int getIntereZone(unsigned char *data,int w,int h,int threshold,int *x,int *y,int *zoneWidthg,int *zonHeight);

int downRGBBy2x2(unsigned char * recImg, unsigned char * rgbImg,int width,int height);

FTP_STATE ftp_upload(const FTP_OPT ftp_option);

FTP_STATE ftp_download(const FTP_OPT ftp_option);
#endif
