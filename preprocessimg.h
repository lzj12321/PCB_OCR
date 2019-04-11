#ifndef PREPROCESSIMG_H
#define PREPROCESSIMG_H
#include<QObject>
#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include<cvandui.h>

#define INVALID_INPUT_IMG -1
#define ANGLE_UNCERTAIN -2
#define ROI_UNCERTAIN -3
#define RESULT_NORMAL -4

class preProcessImg:public QObject
{
    Q_OBJECT
public:
    cv::Mat sourceMat;
    cv::Mat roiMat;
    cv::Mat thresholdMat;
    cv::Mat morphFilterMat;
    cv::Mat morphThresholdMat;
    cv::Mat contoursMat;
    cv::Mat maskMat;
    cv::Mat maskMulSrcMat;
    cv::Mat dilateContourMat;
    cv::Mat showMat;
public:
    std::vector<std::vector<cv::Point>> filterContours;
    std::vector<cv::Rect> validCodeRect;
    std::vector<cv::RotatedRect> validCodeRotatedRect;
    std::vector<cv::Mat> validCodeRegionMat;
    std::vector<std::vector<cv::Point>> validCodeRegionContours;
    std::vector<cv::Vec4i> validCodeRegionHierarchy;
    cv::Vec4i resultLine;
    float pcbAngle=361;
private:
    uint roiX=0;
    uint roiY=0;
    uint roiWidth=0;
    uint roiHeight=0;

    uint thresholdValue=170;
    uint thresholdFlag=cv::THRESH_BINARY;

    uint morPreProShape=cv::MORPH_ELLIPSE;
    uint morPreProSize=5;

    uint minContoursSize;
    uint maxContoursSize;

    uint morphExtractCodeRegionShape=cv::MORPH_RECT;
    uint morphRectWidth=21;
    uint morphRectHeight=1;

    uint minValidRegionWidth=200;
    uint maxValidRegionWidth;

    uint minValidRegionHeight;
    uint maxValidRegionHeight;
private:
    cvAndUi showTool;
public:
    preProcessImg();
    void saveValidCodeMat();
public:
    cv::Rect createRoiRect(const std::vector<std::vector<cv::Point>>&contours,const cv::Size&matSize);
    void setRoiParam(uint x,uint y,uint w,uint h);
    void createRoiMat();
    void createShowMat();
    cv::Mat createContourMask(const std::vector<std::vector<cv::Point>>&contours,cv::Size);
    void contourFilter(std::vector<std::vector<cv::Point>>&contours);
    cv::Mat resizeMat(const cv::Mat& src, float i);

    /////////ini data//////////
    void processDataIni();

    ///////////entrance///////////////
    int extractValidCodeRegion(const cv::Mat&);

    ///////////get the roi mat///////////
    void setRoiMat();

    ///////////threshold the roimat//////
    void thresholdImg();

    ///////////morphology the roiMat to eliminate the small contours
    void morphToFilter();

    //////////find and filter contours///
    void findAndFilterContours();

    //////////create the contours mask to get the valid code///
    void contoursMaskToExtractCodeRegion();

    ////////rotated the valid rect///////////////////
    void rotateValidRect();

    ///////refine valid rect////////////////////////
    void refineValidRect();

    float detectLineAngle(cv::Mat src);

    bool detectCharacterColor(cv::Mat src);

    int detectLineAndSetRoiRegion(cv::Mat src,const cv::Mat&mask,cv::Mat& dst);

public slots:
    void slotSetPreProcessParam(uint _thresholdValue, uint _thresholdFlag,
                                uint _morPreProShape, uint _morPreProSize,
                                uint _minContoursSize, uint _maxContoursSize,
                                uint _morphExtractCodeRegionShape, uint _morphRectWidth, uint _morphRectHeight);
};

#endif // PREPROCESSIMG_H
