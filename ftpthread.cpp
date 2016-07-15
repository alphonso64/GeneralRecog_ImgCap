#include "ftpthread.h"
#include <stdio.h>
#include "imgpro.h"
FtpThread::FtpThread(QObject *parent) :
    QThread(parent)
{
    stopped = false;
    saveNum = 0;
    currentNum = 0;
}

void FtpThread::run()
{
    while(!stopped)
    {
        if(currentNum<saveNum)
        {
            char dst[100] ;
            memset(dst,100,0);
            char src[100] ;
            memset(src,100,0);
            sprintf(src,IMAGESACEPATH,testmode_time,currentNum);
            FTP_OPT ftp_opt;
            sprintf(dst,"ftp://115.29.193.236/train/%d_%d.jpg",testmode_time,currentNum);
            ftp_opt.url = dst;
            ftp_opt.user_key = "myftp:880414";
            ftp_opt.file = src;
            if(FTP_UPLOAD_SUCCESS == ftp_upload(ftp_opt))
            {
                currentNum++;
                printf("Upload success %s.\n",src);
            }
            else
                printf("Upload failed.\n");
        }
    }
    stopped = false;
}

void FtpThread::stop()
{
    stopped = true;
}

void FtpThread::setNum(int num)
{
    saveNum = num;
}

void FtpThread::init(int num)
{
    saveNum = num;
    currentNum = num;
}
