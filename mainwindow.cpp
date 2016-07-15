#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imgpro.h"
#include <time.h>
#include <stdlib.h>
#include <QImageWriter>
#include <QDebug>
int FRAME_WIDTH = 640;
int FRAME_HEIGHT = 480;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initCam();
    m_timer_stream = new QTimer ( this );
    m_timer_stream->start(33);
    connect ( m_timer_stream , SIGNAL (timeout ()), this , SLOT (imgStream()));
    m_timer_captrue = new QTimer ( this );
    m_timer_captrue->start(2000);
    connect ( m_timer_captrue , SIGNAL (timeout ()), this , SLOT (imgCapture()));

    char cmd[50];
    sprintf(cmd,"sudo rm -rf %s",IMAGESACEPATH_FULL);
    system(cmd);

    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->open_pushButton->setEnabled(true);

    m_camera->v4l2_init();
    if(m_camera->initFlag)
    {
        ui->open_pushButton->setText("关闭摄像头");
    }else
    {
        ui->open_pushButton->setText("打开摄像头");
    }
    cleanScreen();

    //     ui->pushButton_12->setVisible(false);

    ui->capture_num_label->setText("0");
    ui->capture_num_label_2->setText("0");

    msgForm = new MessageForm();
    msgForm->hide();
    msgForm->move(342,190);

    isCapMode = false;
    ftp.start();

    cam_img = new QImage(rgbImg,FRAME_WIDTH,FRAME_HEIGHT,QImage::Format_RGB888);
}

void MainWindow::cleanScreen()
{
    QImage img = QImage (( const uchar *) rgbImg , FRAME_WIDTH , FRAME_HEIGHT , QImage :: Format_RGB888 );
    cleanImg(&img,FRAME_WIDTH,FRAME_HEIGHT);
    ui -> cam_label -> setPixmap ( QPixmap :: fromImage ( img ));
    QImage img_a = QImage (( const uchar *) recImg , FRAME_WIDTH/2 , FRAME_HEIGHT/2 , QImage :: Format_RGB888 );
    cleanImg(&img_a,FRAME_WIDTH/2 ,FRAME_HEIGHT/2);
    ui -> capture_label -> setPixmap ( QPixmap :: fromImage ( img ));
}

void MainWindow::imgStream()
{
    if(m_camera->initFlag)
    {
        int length;
        m_camera->read_frame_from_camera( yuyvImg, &length);
        convert_YUYV_to_RGB24_(FRAME_WIDTH, FRAME_HEIGHT, yuyvImg, rgbImg);
        for (int y = 0; y < cam_img->height(); y++)
        {
            memcpy(cam_img->scanLine(y), rgbImg+y*FRAME_WIDTH*3, cam_img->bytesPerLine());
        }
        ui -> cam_label -> setPixmap ( QPixmap :: fromImage ( *cam_img ));
    }
}

void MainWindow::on_pushButton_12_clicked()
{
    ftp.stop();
    qApp->exit(0);
}

void MainWindow::imgCapture()
{
    if(m_camera->initFlag)
    {
        if(isCapMode)
        {
            char bb[20]={0};
            downRGBBy2x2(recImg,rgbImg,FRAME_WIDTH,FRAME_HEIGHT);
            sprintf(bb,IMAGESACEPATH,testmode_time,savenum);
//            saveYUYVImg(bb,yuyvImg,FRAME_WIDTH,FRAME_HEIGHT);
            ui->capture_num_label->setText(QString::number(savenum+1));
            ui->capture_num_label_2->setText(QString::number(ftp.currentNum+1));

            QImage img = QImage (( const uchar *) recImg , FRAME_WIDTH / 2, FRAME_HEIGHT /2 , QImage :: Format_RGB888 );
            ui ->capture_label -> setPixmap ( QPixmap :: fromImage ( img ));

            QImageWriter writer(bb);
            writer.setQuality(100);
            if(writer.write(*cam_img))
            {
                savenum++;
                ftp.setNum(savenum);
            }

        }
    }
}

MainWindow::~MainWindow()
{
    if(m_camera->initFlag)
    {
        m_camera->v4l2_close();
    }
    delete ui;
    delete m_camera;
    delete cam_img;
    uninitCam();
}

void MainWindow::initCam()
{

    yuyvImg = (uint8 *)malloc(FRAME_WIDTH * FRAME_HEIGHT*2);
    rgbImg = (uint8 *)malloc(FRAME_WIDTH * FRAME_HEIGHT * 3);
    recImg = (uint8 *)malloc(FRAME_WIDTH * FRAME_HEIGHT*3);

    savenum = 0;
    ftp.init(0);
    m_camera = new Tcamera(FRAME_WIDTH,FRAME_HEIGHT);
}

void MainWindow::uninitCam()
{
    free(rgbImg);
    free(yuyvImg);
    free(recImg);
}


void MainWindow::on_detect_pushButton_clicked()
{
    if(isCapMode)
    {
        isCapMode = false;
        ui->detect_pushButton->setText("开始采集");
    }else
    {
        isCapMode = true;
        savenum = 0;
        ftp.init(savenum);
        time(&testmode_time);
        ftp.testmode_time = testmode_time;
        ui->detect_pushButton->setText("停止采集");
    }
}

void MainWindow::on_swupdate_pushButton_clicked()
{
    FTP_OPT ftp_opt;
    ftp_opt.url = "ftp://115.29.193.236/param/rec_2/GeneralRecog";
    ftp_opt.file = "/home/program/temp/GeneralRecog";
    ftp_opt.user_key = "myftp:880414";
    if(FTP_DOWNLOAD_SUCCESS == ftp_download(ftp_opt))
    {
        msgForm->setMsg(QString("程序更新成功,请重启机器！"));
        msgForm->show();
        fileSync("/home/program/temp/GeneralRecog");
    }
    else
    {
        msgForm->setMsg(QString("程序更新失败！"));
        msgForm->show();
    }
}


void MainWindow::on_open_pushButton_clicked()
{
    if(m_camera->initFlag)
    {
        if(SUCCESS==m_camera->v4l2_close()){
            ui->open_pushButton->setText("打开摄像头");
            cleanScreen();
        }

    }else
    {
        if(SUCCESS==m_camera->v4l2_init()){
            ui->open_pushButton->setText("关闭摄像头");
        }
    }
}


