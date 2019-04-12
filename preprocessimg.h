#ifndef PREPROCESSIMG_H
#define PREPROCESSIMG_H
#include<QObject>
#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include<cvandui.h>

#define NOMATCH_REGION_DETECTED 0
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

    int srcImgAngleRange=15;
    int roiImgAngleRange=10;

private:
    cvAndUi showTool;
public:
    preProcessImg();
    /////////ini data//////////
    void processDataIni();

    ///////////entrance///////////////
    int extractValidCodeRegion(const cv::Mat&);

    //////////detect the src img angle//////////
    float detectLineAngle(cv::Mat src);

    //////////detect the font's color////////////
    bool detectCharacterColor(cv::Mat src);

    /////////detect the roi img's angle and set the roi mat/////////
    int detectLineAndSetRoiRegion(cv::Mat src,const cv::Mat&mask,cv::Mat& dst);

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

    ///////save the valid code region mat//////////
    void saveValidCodeMat();

    void rectifyMat(cv::Mat&mat);

public slots:
    void slotSetPreProcessParam(uint _thresholdValue, uint _thresholdFlag,
                                uint _morPreProShape, uint _morPreProSize,
                                uint _minContoursSize, uint _maxContoursSize,
                                uint _morphExtractCodeRegionShape, uint _morphRectWidth, uint _morphRectHeight);
};

#endif // PREPROCESSIMG_H
