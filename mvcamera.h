#ifndef MVCAMERA_H
#define MVCAMERA_H
#include<stdio.h>
#include<Windows.h>
#include "CameraApi.h"
#include<opencv2/opencv.hpp>

#define SOFTWARE_MODE 0
#define HARDWARE_MODE 1
#define CONTINUOUS_MODE 2

class MVCamera
{
public:
    MVCamera();
    virtual ~MVCamera();
    bool iniCamera();
    cv::Mat grabImg();
    bool openCamera();
    bool setExpose(uint expTime);
    bool setTriggerMode(uint mode);
    void closeCamera();
private:
    tSdkCameraDevInfo CameraList[1];
    int cameraHandler = 0;
    tSdkCameraCapbility CameraInfo;

};

#endif // MVCAMERA_H
