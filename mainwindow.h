#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "tcamera.h"
#include "Recognizer.h"
#include "ftpthread.h"
#include "messageform.h"
#include <QMutex>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void imgCapture();
    void imgStream();
    void on_pushButton_12_clicked();
    void on_open_pushButton_clicked();
    void on_detect_pushButton_clicked();
    void on_swupdate_pushButton_clicked();

private:
    void cleanScreen();

private:
    Ui::MainWindow *ui;
    Tcamera *m_camera;
    QTimer *m_timer_stream;
    QTimer *m_timer_captrue;
    FtpThread ftp;
    uint8 *rgbImg,*yuyvImg,*recImg;
    int savenum;
    bool isCapMode;
    void initCam();
    void uninitCam();
    time_t testmode_time;
    MessageForm *msgForm;
    QMutex mutex;
    QImage *cam_img;
};

#endif // MAINWINDOW_H
