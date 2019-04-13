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


############windows#########windows################

#INCLUDEPATH +=D:\opencv4.0.1\build\include
#INCLUDEPATH +=D:\opencv4.0.1\build\include\opencv2
#INCLUDEPATH +=D:\MindVision\Demo\QT5\Include
#LIBS+=D:\opencv4.0.1\build\x64\vc15\lib\opencv_world400.lib
#LIBS+=D:\opencv4.0.1\build\x64\vc15\lib\opencv_world400d.lib
#LIBS+=D:\MindVision\SDK\X64\MVCAMSDK_X64.lib

############windows#########windows################


############ubuntu#########ubuntu################

INCLUDEPATH+=/usr/local/include/opencv4 \
/usr/local/include/opencv4/opencv2 \
/usr/local/include/leptonica

LIBS += /usr/local/lib/libopencv_calib3d.so \
/usr/local/lib/libopencv_core.so \
/usr/local/lib/libopencv_features2d.so \
/usr/local/lib/libopencv_flann.so \
/usr/local/lib/libopencv_highgui.so \
/usr/local/lib/libopencv_imgproc.so \
/usr/local/lib/libopencv_ml.so \
/usr/local/lib/libopencv_objdetect.so \
/usr/local/lib/libopencv_photo.so \
/usr/local/lib/libopencv_stitching.so \
/usr/local/lib/libopencv_video.so \
/usr/local/lib/libopencv_imgcodecs.so \
/usr/lib/libtesseract.so \
/usr/lib/liblept.so.5 \
/usr/local/lib/libMVSDK.so

############ubuntu#########ubuntu################




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
    mvcamera.cpp \
    tesseractocr.cpp

HEADERS += \
        mainwindow.h \
    cvandui.h \
    settingsdlg.h \
    preprocessimg.h \
    mvcamera.h \
    tesseractocr.h

FORMS += \
        mainwindow.ui \
    settingsdlg.ui
