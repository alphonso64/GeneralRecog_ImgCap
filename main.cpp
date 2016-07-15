#include "mainwindow.h"
#include <QApplication>
#include <QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOverrideCursor(Qt::BlankCursor);
    QFile file("/home/program/asserts/stylesheet");
    bool bOpened = file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    a.setStyleSheet (styleSheet);
    MainWindow w;
    w.show();
    return a.exec();
}
