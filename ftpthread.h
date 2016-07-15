#ifndef FTPTHREAD_H
#define FTPTHREAD_H

#include <QThread>

class FtpThread : public QThread
{
    Q_OBJECT
public:
    explicit FtpThread(QObject *parent = 0);
    void stop();
    void setNum(int num);
    void init(int num);
signals:
public slots:
protected:
    void run();
private:
    volatile bool stopped;
    volatile int saveNum;
public:
    volatile int currentNum;
public:
    time_t testmode_time;
};

#endif // FTPTHREAD_H
