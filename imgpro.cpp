#include "imgpro.h"
#include "stdio.h"
#include <stdlib.h>
#include<math.h>
#include "Rbmp.h"
static const int K1 = int(1.402f * (1 << 16));
static const int K2 = int(0.714f * (1 << 16));
static const int K3 = int(0.334f * (1 << 16));
static const int K4 = int(1.772f * (1 << 16));


void getGrayImageFromYUYV(uint8 *src,int w,int h,uint8 *dst){
    int cnt=0;
    int offset_w = w * 2;
    int i,j;
    for(i=0;i<h;i++){
        for(j=0;j<offset_w;j+=2){
            int temp = src[i*offset_w+j];
            dst[cnt++] = temp;
            dst[cnt++] = temp;
            dst[cnt++] = temp;
        }
    }
}

void fileSync(const char *file)
{
    int fd =  open( file, O_RDWR);
    if(fd != -1)
    {
        syncfs(fd);
        close(fd);
    }
}

void convert_YUYV_to_RGB24_(int width, int height, const uint8* yuyv_image, uint8 *rgb_image){
    int y;
    int cr;
    int cb;

    int r;
    int g;
    int b;

    for (int i = 0,j = 0; i < width * height * 3; i+=6, j+=4) {
        //first pixel
        y = yuyv_image[j];
        cb = yuyv_image[j+1]-128;
        cr = yuyv_image[j+3]-128;

        r = y + (K1*cr >> 16);
        g = y - (K2*cr >> 16) - (K3*cb >> 16);
        b = y + (K4*cb >> 16);

        if (r < 0) r = 0;
        else if (r > 255) r = 255;
        if (g < 0) g = 0;
        else if (g > 255) g = 255;
        if (b < 0) b = 0;
        else if (b > 255) b = 255;

        rgb_image[i] = (unsigned char)r;
        rgb_image[i+1] = (unsigned char)g;
        rgb_image[i+2] = (unsigned char)b;

        //second pixel
        y = yuyv_image[j+2];

        r = y + (K1*cr >> 16);
        g = y - (K2*cr >> 16) - (K3*cb >> 16);
        b = y + (K4*cb >> 16);

        if (r < 0) r = 0;
        else if (r > 255) r = 255;
        if (g < 0) g = 0;
        else if (g > 255) g = 255;
        if (b < 0) b = 0;
        else if (b > 255) b = 255;

        rgb_image[i+3] = (unsigned char)r;
        rgb_image[i+4] = (unsigned char)g;
        rgb_image[i+5] = (unsigned char)b;
    }
}

void convert_YUYV_to_RGB24_Y(int width, int height, const uint8* yuyv_image, uint8 *rgb_image,uint8 *graybmp){
    int y;
    int cr;
    int cb;

    int r;
    int g;
    int b;
    int cnt =0;
    for (int i = 0,j = 0; i < width * height * 3; i+=6, j+=4) {
        //first pixel
        y = yuyv_image[j];
        cb = yuyv_image[j+1]-128;
        cr = yuyv_image[j+3]-128;
        graybmp[cnt++] = y;
        r = y + (K1*cr >> 16);
        g = y - (K2*cr >> 16) - (K3*cb >> 16);
        b = y + (K4*cb >> 16);

        if (r < 0) r = 0;
        else if (r > 255) r = 255;
        if (g < 0) g = 0;
        else if (g > 255) g = 255;
        if (b < 0) b = 0;
        else if (b > 255) b = 255;

        rgb_image[i] = (unsigned char)r;
        rgb_image[i+1] = (unsigned char)g;
        rgb_image[i+2] = (unsigned char)b;

        //second pixel
        y = yuyv_image[j+2];
        graybmp[cnt++] = y;

        r = y + (K1*cr >> 16);
        g = y - (K2*cr >> 16) - (K3*cb >> 16);
        b = y + (K4*cb >> 16);

        if (r < 0) r = 0;
        else if (r > 255) r = 255;
        if (g < 0) g = 0;
        else if (g > 255) g = 255;
        if (b < 0) b = 0;
        else if (b > 255) b = 255;

        rgb_image[i+3] = (unsigned char)r;
        rgb_image[i+4] = (unsigned char)g;
        rgb_image[i+5] = (unsigned char)b;
    }
}

void drawRectangle(uint8* src,int  width,int height,int x,int y,int recWidth,int recHeight){
    if(x + recWidth>width || y+recHeight > height){
        return;
    }

// 	printf("hahb\n");
    int offset = (y*width+x)*3;
    int offset_h= ((y+recHeight)*width+x)*3;
    int offset_w= (y*width+x+recWidth-1)*3;
    for(int i=0;i<recWidth;i++){
        src[offset+i*3] = 255;
        src[offset+i*3+1] = 0;
        src[offset+i*3+2] = 0;
        src[offset_h+i*3] = 255;
        src[offset_h+i*3+1] = 0;
        src[offset_h+i*3+2] = 0;
    }
    for(int i=1;i<recHeight;i++){
        offset += width*3;
        offset_w += width*3;
        src[offset] = 255;
        src[offset+1] = 0;
        src[offset+2] = 0;
        src[offset_w] = 255;
        src[offset_w+1] = 0;
        src[offset_w+2] = 0;
    }

}

void drawRectangle(QImage *img,int x,int y,int recWidth,int recHeight){

    if(x + recWidth>img->width() || y+recHeight > img->height()){
        return;
    }

    int value = qRgb(255,0,0);
    for(int i=0;i<recWidth;i++){
        img->setPixel(x+i,y,value);
        img->setPixel(x+i,y+recHeight,value);
    }
    for(int i=0;i<recHeight;i++){
        img->setPixel(x,y+i,value);
        img->setPixel(x+recWidth,y+i,value);
    }
}


void cleanImg(QImage *img,int width,int height)
{
    int value = qRgb(0,0,0);
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            img->setPixel(j,i,value);
        }
    }
}
void getBMPReczone(uint8* src,int  width,int height,int x,int y,int recWidth,int recHeight,uint8 *dst){
    //printf("getBMPReczone %d %d %d %d %d %d\n",width,height,x,y,recWidth,recHeight);
    int offset = (y*width + x)*3;
    for(int i=0;i<recHeight;i++){
        for(int j=0;j<recWidth;j++){
            dst[(i*recWidth+j)*3] = src[offset+(i*width+j)*3];
            dst[(i*recWidth+j)*3+1] = src[offset+(i*width+j)*3+1];
            dst[(i*recWidth+j)*3+2] = src[offset+(i*width+j)*3+2];
        }
    }
}

void getGRAYReczone(uint8* src,int  width,int height,int x,int y,int recWidth,int recHeight,uint8 *dst){
    int offset = (y*width + x);
    for(int i=0;i<recHeight;i++){
        for(int j=0;j<recWidth;j++){
            dst[(i*recWidth+j)] = src[offset+(i*width+j)];
        }
    }
}

void getdownSample(unsigned char *output,int *w,int *h,unsigned char *src,int srcW,int srcH,int recWidth,int recHeight){
    if(srcW%recWidth!=0 || srcH%recHeight !=0)
        return ;
    int tw = 0;
    int th = 0;
    *w = srcW/recWidth;
    *h = srcH/recHeight;
    for(int i=0;i<srcH;i+=recHeight){
        tw = 0;
        for(int j=0;j<srcW;j+=recWidth){
            int pos_1 = th*(*w)+tw;
            int pos_2 = i*(srcW)+j;
            int val =0;
            for(int k=0;k< recHeight;k++){
                for(int l=0;l<recWidth;l++){
                    int pos_3 =pos_2 + k*srcW+l;
                    val += src[pos_3];
                }
            }
            output[pos_1]  = val/(recWidth * recHeight);
            tw++;
        }
        th++;
    }
    return ;
}

void ResizeLinear(unsigned char *output,int dstW,int dstH,unsigned char *src,int srcW,int srcH)
{
    // int w0 = src.GetWidth();
    // int h0 = src.GetHeight();
    // int pitch0 = src.GetPitch();

    // int w1 = dst.GetWidth();
    // int h1 = dst.GetHeight();
    // int pitch1 = dst.GetPitch();

    float fw = float(srcW) / dstW;
    float fh = float(srcH) / dstH;

    int y1,y2, x1,x2, x0,y0;
    float fx1,fx2, fy1, fy2;

    for(int y=0; y<dstH; y++)
    {
        y0 = y*fh;
        y1 = int(y0);
        if(y1 == srcH-1)    y2 = y1;
        else y2 = y1 + 1;

        fy1 = y1-y0;
        fy2 = 1.0f - fy1;
        for(int x=0; x<dstW; x++)
        {
            x0 = x*fw;
            x1 = int(x0);
            if(x1 == srcW-1)    x2 = x1;
            else x2 = x1+1;

            fx1 = y1-y0;
            fx2 = 1.0f - fx1;

            float s1 = fx1*fy1;
            float s2 = fx2*fy1;
            float s3 = fx2*fy2;
            float s4 = fx1*fy2;

            float c1,c2,c3,c4;
            float val;
            c1 = src[y1*srcW+x1];
            c2 = src[y1*srcW+x2];
            c3 = src[y2*srcW+x1];
            c4 = src[y2*srcW+x2];
            val = c1*s3+c2*s4+c3*s2+c4*s1;
            output[y*dstW+x] = val;
        }
    }

}

int detectThreshold(int nMaxIter, int &nDiffRet,unsigned char *graySrc,int w,int h)
{
    int nThreshold;

    nDiffRet = 0;

    // 直方图数组
    int nHistogram[256] = { 0 };
    int i, j;

    unsigned char bt;

    int nMin = 255;
    int nMax = 0;

    // 扫描图像,计算出最大、最小灰度和直方图
    for(j = 0; j < h; j ++)
    {
        for(i=0; i<w; i++)
        {
            bt = graySrc[j*w+i];

            if(bt < nMin)
                nMin = bt;
            if(bt > nMax)
                nMax = bt;

            nHistogram[bt] ++;

        }
    }

    int nTotalGray = 0; //灰度值的和
    int nTotalPixel = 0; //像素数的和
    int nNewThreshold = (nMax + nMin)/2; //初始阈值

    nDiffRet = nMax - nMin;

    if (nMax == nMin)
        nThreshold = nNewThreshold;

    else
    {
        nThreshold = 0;

        // 迭代开始,直到迭代次数达到100或新阈值与上一轮得到的阈值相等，迭代结束
        for(int nIterationTimes = 0; nThreshold != nNewThreshold && nIterationTimes < nMaxIter; nIterationTimes ++)
        {
            nThreshold = nNewThreshold;
            nTotalGray = 0;
            nTotalPixel = 0;

            //计算图像中小于当前阈值部分的平均灰度
            for(i=nMin; i<nThreshold; i++)
            {
                nTotalGray += nHistogram[i]*i;
                nTotalPixel += nHistogram[i];
            }
            int nMean1GrayValue = nTotalGray/nTotalPixel;


            nTotalGray = 0;
            nTotalPixel = 0;

            //计算图像中大于当前阈值部分的平均灰度
            for(i=nThreshold + 1; i<=nMax; i++)
            {
                nTotalGray += nHistogram[i]*i;
                nTotalPixel += nHistogram[i];
            }
            int nMean2GrayValue = nTotalGray/nTotalPixel;

            nNewThreshold = (nMean1GrayValue + nMean2GrayValue)/2; //计算出新的阈值
            nDiffRet = abs(nMean1GrayValue - nMean2GrayValue);
        }
    }

    return nThreshold;
}


int saveBMPFromGrayImg(const char *path,unsigned char *data,int w,int h){
    FILE *fp = fopen(path,"wb+");
    if(fp== NULL){
        return -1;
    }
    int dibLen = 256*4;
    int offset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+dibLen;
    int totalLen = w*h+offset;
    unsigned char *temp = (unsigned char*)malloc(totalLen);
    BITMAPFILEHEADER *header = (BITMAPFILEHEADER *)temp;
    BITMAPINFOHEADER *info = (BITMAPINFOHEADER *)(temp+sizeof(BITMAPFILEHEADER));
    unsigned char *dstData = (unsigned char *)(temp + offset);

    header->bfOffBits = offset;
    header->bfReserved1 =0;
    header->bfReserved2 = 0;
    header->bfType = 0x4D42;
    header->bfSize = totalLen;

    info->biSize = sizeof(BITMAPINFOHEADER);
    info->biWidth = w;
    info->biHeight = h;
    info->biPlanes = 1;
    info->biBitCount = 8;
    info->biCompression = 0;
    info->biSizeImage = w*h;
    info->biXPelsPerMeter = 0;
    info->biYPelsPerMeter = 0;
    info->biClrUsed = 256;
    info->biClrImportant = 0;
    rgb_pixel_t *rgba = (rgb_pixel_t *)(temp+sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
    for(int i=0;i<256;i++){
        rgba->alpha = 0;
        rgba->blue = i;
        rgba->green = i;
        rgba->red = i;
        rgba++;
    }
    //memcpy(dstData,data,w*h);
    for(int i=0;i<h;i++){
        for(int j=0;j<w;j++){
            dstData[i*w+j] = data[(h-1-i)*w+j];
        }
    }
    fwrite(temp,1,totalLen,fp);
    fclose(fp);
    free(temp);
    return 0;
}

int saveYUYVImg(const char *path,unsigned char *data,int w,int h)
{
    FILE *fp = fopen(path,"wb+");
    if(fp== NULL){
        return -1;
    }
    int totalLen = w*h*2;
    unsigned char *temp = (unsigned char*)malloc(totalLen);
    memcpy(temp,data,totalLen);
    fwrite(temp,1,totalLen,fp);
    fclose(fp);
    free(temp);
    return 0;
}

int saveBMPFromRGB888Img(const char *path,unsigned char *data,int w,int h)
{
    FILE *fp = fopen(path,"wb+");
    if(fp== NULL){
        return -1;
    }
    int dibLen = 256*4;
    int offset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+dibLen;
    int totalLen = w*h*3+offset;
    unsigned char *temp = (unsigned char*)malloc(totalLen);
    BITMAPFILEHEADER *header = (BITMAPFILEHEADER *)temp;
    BITMAPINFOHEADER *info = (BITMAPINFOHEADER *)(temp+sizeof(BITMAPFILEHEADER));
    unsigned char *dstData = (unsigned char *)(temp + offset);

    header->bfOffBits = offset;
    header->bfReserved1 =0;
    header->bfReserved2 = 0;
    header->bfType = 0x4D42;
    header->bfSize = totalLen;

    info->biSize = sizeof(BITMAPINFOHEADER);
    info->biWidth = w;
    info->biHeight = -h;
    info->biPlanes = 1;
    info->biBitCount = 24;
    info->biCompression = 0;
    info->biSizeImage = w*h*3;
    info->biXPelsPerMeter = 0;
    info->biYPelsPerMeter = 0;
    info->biClrUsed = 256;
    info->biClrImportant = 0;
    rgb_pixel_t *rgba = (rgb_pixel_t *)(temp+sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
    for(int i=0;i<256;i++){
        rgba->alpha = 0;
        rgba->blue = i;
        rgba->green = i;
        rgba->red = i;
        rgba++;
    }
    memcpy(dstData,data,w*h*3);
//    for(int i=0;i<h;i++){
//        for(int j=0;j<w;j++){
//            dstData[i*w+j] = data[(h-1-i)*w+j];
//        }
//    }
    fwrite(temp,1,totalLen,fp);
    fclose(fp);
    free(temp);
    return 0;
}

int getIntereZone(unsigned char *data,int w,int h,int threshold,int *x,int *y,int *zoneWidthg,int *zoneHeight){
    int minX = w+1;
    int minY = w+1;
    int maxX = 0;
    int maxY = 0;
    for(int i=0;i<h;i++){
        for(int j=0;j<w;j++){
            if(data[i*w+j]>=threshold){

                if(minX>j)
                    minX = j;
                if(maxX<j)
                    maxX = j;

                if(minY>i)
                    minY = i;
                if(maxY<i)
                    maxY = i;

            }
        }
    }
    if(minX%2 !=0){
        minX -= 1;
    }
    if(minY%2 !=0){
        minY -= 1;
    }
    if(maxX%2 !=0){
        maxX += 1;
    }
    if(maxY%2 !=0){
        maxY += 1;
    }

    if(maxX>minX && maxY>minY){
        if((maxX-minX) %4!=0){
            if(minX == 0 ){
                maxX += 2;
            }else if(maxX == w-1){
                minX -= 2;
            }else{
                minX -= 1;
                maxX += 1;
            }
        }
        if((maxY-minY) %4!=0){
            if(minY == 0 ){
                maxY += 2;
            }else if(maxY == w-1){
                minY -= 2;
            }else{
                minY -= 1;
                maxY += 1;
            }
        }
        *x = minX;
        *y = minY;
        *zoneWidthg = (maxX-minX);
        *zoneHeight = (maxY-minY);
        return 0;
    }
    return -1;
}

/*****************util api******************/
int get_file_size(FILE *file)
{
    int size = 0;
    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return size;
}

/******************curl api****************/
CURL *curl_init()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();
    if(NULL == curl)
    {
        fprintf(stderr, "Init curl failed.\n");
        exit(1);
    }
    return curl;
}

void curl_set_upload_opt(CURL *curl, const char *url, const char *user_key, FILE *file)
{
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERPWD, user_key);
    curl_easy_setopt(curl, CURLOPT_READDATA, file);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, get_file_size(file));
    curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1);
//  curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
}

void curl_set_download_opt(CURL *curl, const char *url, const char *user_key, FILE *file)
{
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERPWD, user_key);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
//  curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
}

void curl_exit(CURL *curl)
{
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

CURLcode curl_perform(CURL *curl)
{
    CURLcode ret = curl_easy_perform(curl);
    return ret;
}

/****************ftp upload & download api******************/
FTP_STATE ftp_upload(const FTP_OPT ftp_option)
{
    FTP_STATE state;
    CURL *curl;;
    FILE *fp = fopen(ftp_option.file, "r");
    if(NULL == fp)
    {
        fprintf(stderr, "Open file failed at %s:%d\n", __FILE__, __LINE__);
        return FTP_UPLOAD_FAILED;
    }

    curl = curl_init();
    curl_set_upload_opt(curl, ftp_option.url, ftp_option.user_key, fp);
    if(CURLE_OK == curl_perform(curl))
        state = FTP_UPLOAD_SUCCESS;
    else
        state = FTP_UPLOAD_FAILED;

    curl_exit(curl);
    fclose(fp);
    return state;
}

FTP_STATE ftp_download(const FTP_OPT ftp_option)
{
    FTP_STATE state;
    CURL *curl;
    FILE *fp = fopen(ftp_option.file, "w");
    if(NULL == fp)
    {
        fprintf(stderr, "Open file failed at %s:%d\n", __FILE__, __LINE__);
        return FTP_UPLOAD_FAILED;
    }

    curl = curl_init();
    curl_set_download_opt(curl, ftp_option.url, ftp_option.user_key, fp);
    if(CURLE_OK == curl_perform(curl))
        state = FTP_DOWNLOAD_SUCCESS;
    else
        state = FTP_DOWNLOAD_FAILED;

    curl_exit(curl);
    fclose(fp);
    return state;
}

int downRGBBy2x2(unsigned char * recImg, unsigned char * rgbImg,int width,int height)
{
    int stepWidth = width/2;
    int stepHeight = height/2;
    for(int i=0;i<stepHeight;i++)
    {
        for(int j=0;j<stepWidth;j++)
        {
            recImg[i*stepWidth*3 +j*3] = rgbImg[i*width*6+j*6];
            recImg[i*stepWidth*3 +j*3+1] = rgbImg[i*width*6+j*6+1];
            recImg[i*stepWidth*3 +j*3+2] = rgbImg[i*width*6+j*6+2];
        }
    }
}
