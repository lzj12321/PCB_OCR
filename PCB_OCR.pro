#-------------------------------------------------
#
# Project created by QtCreator 2019-03-19T19:47:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PCB_OCR
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH +=D:\opencv4.0.1\build\include
INCLUDEPATH +=D:\opencv4.0.1\build\include\opencv2

INCLUDEPATH +=D:\MindVision\Demo\QT5\Include
#INCLUDEPATH +=D:\MindVision/Demo\vc\CameraApi.h
#INCLUDEPATH +=D:\MindVision/Demo\vc\CameraApiLoad.h
#INCLUDEPATH +=D:\MindVision/Demo\vc\CameraDefine.h
#INCLUDEPATH +=D:\MindVision/Demo\vc\CameraGrabber.h
#INCLUDEPATH +=D:\MindVision/Demo\vc\CameraImage.h
#INCLUDEPATH +=D:\MindVision\Demo\vc\CameraStatus.h

#INCLUDEPATH +=D:\MindVision\Demo\vc\include
#INCLUDEPATH +=D:\MindVision/Demo\vc\CameraApi.h
#INCLUDEPATH +=D:\MindVision/Demo\vc\CameraApiLoad.h
#INCLUDEPATH +=D:\MindVision/Demo\vc\CameraDefine.h
#INCLUDEPATH +=D:\MindVision/Demo\vc\CameraGrabber.h
#INCLUDEPATH +=D:\MindVision/Demo\vc\CameraImage.h
#INCLUDEPATH +=D:\MindVision\Demo\vc\CameraStatus.h

LIBS+=D:\opencv4.0.1\build\x64\vc15\lib\opencv_world400.lib
LIBS+=D:\opencv4.0.1\build\x64\vc15\lib\opencv_world400d.lib

LIBS+=D:\MindVision\SDK\X64\MVCAMSDK_X64.lib

#CONFIG(debug,debug|release){
#LIBS+=D:\opencv4.0.1\build\x64\vc15\lib\opencv_world400.lib
#}else{
#LIBS+=D:\opencv4.0.1\build\x64\vc15\lib\opencv_world400d.lib
#}

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    cvandui.cpp \
    settingsdlg.cpp \
    preprocessimg.cpp \
    mvcamera.cpp

HEADERS += \
        mainwindow.h \
    cvandui.h \
    settingsdlg.h \
    preprocessimg.h \
    mvcamera.h

FORMS += \
        mainwindow.ui \
    settingsdlg.ui
