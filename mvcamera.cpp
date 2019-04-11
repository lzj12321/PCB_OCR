#include "mvcamera.h"
#include<QByteArray>
#include<QDebug>

MVCamera::MVCamera()
{

}

MVCamera::~MVCamera()
{
    closeCamera();
}

bool MVCamera::iniCamera()
{
    CameraSdkStatus status;
    // 调用CameraEnumerateDevice前，先设置CameraNums = 16，表示最多只读取16个设备。
    // 如果需要枚举更多的设备，请更改CameraList数组的大小和CameraNums的值
    // Before calling CameraEnumerateDevice, set CameraNums = 16 to read only 16 devices at most.
    // If you need to enumerate more devices, change the size of the CameraList array and CameraNums

    int CameraNums = 1;

    // 枚举设备，获得设备列表
    // Enumerate devices to get a list of devices
    status = CameraEnumerateDevice(CameraList, &CameraNums);
    if (status != CAMERA_STATUS_SUCCESS)
    {
        // printf("No camera was found!");
        qDebug()<<"No camera was found!";
        return false;
    }
    else
    {
        qDebug()<<"A camera was found!";


        // 该示例中，我们只初始化第一个相机。
        // (-1,-1)表示加载上次退出前保存的参数，如果是第一次使用该相机，则加载默认参数。
        // In this example, we only initialize the first camera.
        // (-1,-1) means to load the parameters saved before the last exit. If it is the first time to use the camera, then load the default parameters.
        status = CameraInit(&CameraList[0], -1, -1, &cameraHandler);
        if (status != CAMERA_STATUS_SUCCESS)
        {
            printf("Failed to init the camera! Error code is %d", status);
            return false;
        }


        // 获得该相机的特性描述
        // Get the camera's feature description
        CameraGetCapability(cameraHandler, &CameraInfo);


        return true;
    }
}

cv::Mat MVCamera::grabImg()
{
    cv::Mat grabImg;
    CameraSdkStatus status;
    // 让SDK内部取图线程开始工作
    // Let the SDK internal grab thread start working
    CameraSetRotate(cameraHandler,2);
    CameraSetMirror(cameraHandler,0,TRUE);

    CameraPlay(cameraHandler);

    // 判断是黑白相机还是彩色相机
    // Judging whether it is a mono camera or a color camera
    BOOL bMonoCamera = CameraInfo.sIspCapacity.bMonoSensor;

    // 黑白相机让ISP直接输出MONO数据，而不是扩展成R=G=B的24位灰度
    // Mono cameras allow the ISP to directly output MONO data instead of the 24-bit grayscale expanded to R=G=B
    if (bMonoCamera)
    {
        CameraSetIspOutFormat(cameraHandler, CAMERA_MEDIA_TYPE_MONO8);
    }

    // 计算RGB buffer所需的大小，这里直接按照相机的最大分辨率来分配
    UINT FrameBufferSize = CameraInfo.sResolutionRange.iWidthMax * CameraInfo.sResolutionRange.iHeightMax * (bMonoCamera ? 1 : 3);

    // 分配RGB buffer，用来存放ISP输出的图像
    // 备注：从相机传输到PC端的是RAW数据，在PC端通过软件ISP转为RGB数据（如果是黑白相机就不需要转换格式，但是ISP还有其它处理，所以也需要分配这个buffer）
    // allocate RGB buffer to store the image output by ISP
    // Note: RAW data is transferred from the camera to the PC and converted to RGB data via the software ISP on the PC (if it is a Mono camera, there is no need to convert the format, but the ISP has other processing, so we also need to allocate this buffer)
    BYTE* pFrameBuffer = (BYTE *)CameraAlignMalloc(FrameBufferSize, 16);
    qDebug()<<"FrameBufferSize:"<<FrameBufferSize;
    // 从相机取一帧图片
    // Take a frame from the camera
    tSdkFrameHead FrameHead;
    BYTE* pRawData;

    // Since the camera is currently in soft trigger mode, software is required to send a command to inform the camera to take pictures (to avoid accidentally fetching old pictures in the camera cache, the cache is cleared before the trigger command)
    CameraClearBuffer(cameraHandler);
    CameraSoftTrigger(cameraHandler);

    qDebug()<<"img width:"<<(int)CameraInfo.sResolutionRange.iWidthMax;
    qDebug()<<"img height:"<<(int)CameraInfo.sResolutionRange.iHeightMax;

    status = CameraGetImageBuffer(cameraHandler, &FrameHead, &pRawData, 2000);
    if (status == CAMERA_STATUS_SUCCESS)
    {
        grabImg=cv::Mat(CameraInfo.sResolutionRange.iHeightMax,CameraInfo.sResolutionRange.iWidthMax,CV_8UC1,cv::Scalar::all(123));
        //uint pixNum=CameraInfo.sResolutionRange.iWidthMax*CameraInfo.sResolutionRange.iHeightMax;


        CameraImageProcess(cameraHandler, pRawData, pFrameBuffer, &FrameHead);
        CameraReleaseImageBuffer(cameraHandler, pRawData);

        uchar* matPtr=grabImg.data;
        uchar* dataPtr=(uchar*)pFrameBuffer;
        for(uint i=0;i<FrameBufferSize;++i)
        {
            matPtr[i]=dataPtr[i];
//            matPtr[i]=123;
        }

        // At this point, the picture has been stored in pFrameBuffer. For pFrameBuffer=RGB data in color camera, pFrameBuffer=8bit gray data in mono camera.
        // In this example we just saved the image to a hard disk file
        QString img="C:/Users/lzj/Desktop/test1231.bmp";
        status = CameraSaveImage(cameraHandler,const_cast<char*>(img.toStdString().c_str()), pFrameBuffer, &FrameHead, bMonoCamera ? FILE_BMP_8BIT : FILE_BMP, 100);
        if (status == CAMERA_STATUS_SUCCESS)
            qDebug()<<"Save image successfully.";

        else
            qDebug()<<"Save image failed.";
    }
    else
    {
        qDebug()<<"camera not connect!";
    }
    CameraAlignFree(pFrameBuffer);

    return grabImg;
}

bool MVCamera::openCamera()
{
    if(iniCamera()){
        //grabImg();
        return true;
    }
    else
        return false;
}

bool MVCamera::setExpose(uint expTime)
{
    // 手动曝光，曝光时间30ms
    // Manual exposure, exposure time 30ms
    CameraSetAeState(cameraHandler, FALSE);
    CameraSetExposureTime(cameraHandler, expTime * 1000);
    return true;
}

bool MVCamera::setTriggerMode(uint mode)
{
    // 设置相机为软触发模式，并且把一次触发的帧数固定为1
    // Set the camera to soft trigger mode and fix the number of frames triggered at one time to 1
    CameraSetTriggerMode(cameraHandler, mode);
    CameraSetTriggerCount(cameraHandler, 1);
    return true;
}

void MVCamera::closeCamera()
{
    // 关闭相机
    // close camera
    CameraUnInit(cameraHandler);

    // release RGB buffer
   // BYTE* pFrameBuffer = (BYTE *)CameraAlignMalloc(FrameBufferSize, 16);

  //  CameraAlignFree(pFrameBuffer);

}
