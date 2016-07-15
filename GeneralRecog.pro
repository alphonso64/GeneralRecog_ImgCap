#-------------------------------------------------
#
# Project created by QtCreator 2016-07-12T15:02:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GeneralRecog
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tcamera.cpp \
    imgpro.cpp \
    ftpthread.cpp \
    messageform.cpp

HEADERS  += mainwindow.h \
    tcamera.h \
    sdf.h \
    imgpro.h \
    Rbmp.h \
    Rcomdef.h \
    Recognizer.h \
    ftpthread.h \
    messageform.h

FORMS    += mainwindow.ui \
    messageform.ui
