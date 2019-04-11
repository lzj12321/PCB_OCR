#include "preprocessimg.h"
#include<QString>
#include<QDebug>
#include<QTime>

preProcessImg::preProcessImg()
{

}

void preProcessImg::saveValidCodeMat()
{
    for(int i=0;i<validCodeRegionMat.size();++i)
    {
        QString imgName=QString::number(i+1,10)+".tif";
        cv::Mat tempMat;
        if(validCodeRegionMat[i].channels()==1)
            cv::cvtColor(validCodeRegionMat[i],tempMat,cv::COLOR_GRAY2RGB);
        else
            tempMat=validCodeRegionMat[i].clone();
        cv::imwrite(imgName.toStdString(),tempMat);
    }
}

cv::Rect preProcessImg::createRoiRect(const std::vector<std::vector<cv::Point> > &contours,const cv::Size&matSize)
{
    //std::vector<std::vector>
    cv::Rect test;
    return test;
}

int preProcessImg::extractValidCodeRegion(const cv::Mat &src)
{
    //////////initialize the data//////////////////
    processDataIni();

    //////////confirm the input img is valid//////////////
    if(src.empty())
        return INVALID_INPUT_IMG;
    if(src.channels()!=1)
        cv::cvtColor(src,roiMat,cv::COLOR_RGB2GRAY);
    else
        roiMat=src.clone();

    //////////detect line to calculate the img's angle////
    float angle=detectLineAngle(src.clone());
    if(angle==361)
        return ANGLE_UNCERTAIN;


    /////////rotate the img and mask////////////////////////
    cv::Mat dstRotate;
    cv::Mat dstMask=cv::Mat(roiMat.rows,roiMat.cols,CV_8UC1,cv::Scalar::all(255));
    showTool.rotateMat(dstMask,dstMask,cvRound(90-angle));
    showTool.rotateMat(roiMat,dstRotate,cvRound(90-angle));

    ////////detect the roi//////////////////////////////////
    if(detectLineAndSetRoiRegion(dstRotate,dstMask,roiMat)!=RESULT_NORMAL)
        return ROI_UNCERTAIN;

    if(roiMat.channels()!=1)
        cv::cvtColor(roiMat,roiMat,cv::COLOR_RGB2GRAY);

    //////////detect the font's color//////////////////
    //detectCharacterColor(roiMat);

    //////////threshold the img/////////////////////
    thresholdImg();

    //////////morphology process the img////////////
    //morphToFilter();

    //////////prescreening the contours//////////////
    findAndFilterContours();
    //return 0;
    //////////morphology process the threshold mul mask mat//////
    contoursMaskToExtractCodeRegion();

    /////////use the detected rotatedrect's angle to rotate valid code region mat///////
    //rotateValidRect();

    /////////refine the valid code region and threshod it in OTSU method////////////////
    refineValidRect();

    ///////save the valid code region////////////
    saveValidCodeMat();

    ///////create the result mat to show on dialog/////
    createShowMat();

    ///////return the num of valid code region/////////
    return validCodeRegionMat.size();
}

void preProcessImg::processDataIni()
{
    // srcContours.clear();
    //  srcHierarchy.clear();
    filterContours.clear();
    validCodeRegionMat.clear();
    validCodeRegionContours.clear();
    validCodeRegionHierarchy.clear();
    validCodeRect.clear();
    validCodeRotatedRect.clear();

    pcbAngle=361;
}

void preProcessImg::createShowMat()
{
    //    cv::Size _size=cv::Size(contoursMat.cols,contoursMat.rows*6);
    //    showMat=cv::Mat(_size,CV_8UC3,cv::Scalar::all(0));
    //    contoursMat.copyTo(showMat(cv::Rect(0,0,contoursMat.cols,contoursMat.rows)));
    //    cv::Mat _thresholdMat,_morphFilterMat,_maskMulSrcMat,_maskMat,_dilateContourMat;
    //    cv::cvtColor(thresholdMat,_thresholdMat,cv::COLOR_GRAY2RGB);
    //    cv::cvtColor(morphFilterMat,_morphFilterMat,cv::COLOR_GRAY2RGB);
    //    cv::cvtColor(maskMulSrcMat,_maskMulSrcMat,cv::COLOR_GRAY2RGB);
    //    cv::cvtColor(maskMat,_maskMat,cv::COLOR_GRAY2RGB);
    //    cv::cvtColor(dilateContourMat,_dilateContourMat,cv::COLOR_GRAY2RGB);

    //    _thresholdMat.copyTo(showMat(cv::Rect(0,contoursMat.rows,contoursMat.cols,contoursMat.rows)));
    //    _morphFilterMat.copyTo(showMat(cv::Rect(0,contoursMat.rows*2,contoursMat.cols,contoursMat.rows)));
    //    _maskMulSrcMat.copyTo(showMat(cv::Rect(0,contoursMat.rows*3,contoursMat.cols,contoursMat.rows)));
    //    _maskMat.copyTo(showMat(cv::Rect(0,contoursMat.rows*4,contoursMat.cols,contoursMat.rows)));
    //    _dilateContourMat.copyTo(showMat(cv::Rect(0,contoursMat.rows*5,contoursMat.cols,contoursMat.rows)));

    //    cv::Point linePt1=cv::Point(0,contoursMat.rows);
    //    cv::Point linePt2=cv::Point(contoursMat.cols,contoursMat.rows);
    //    cv::line(showMat,linePt1,linePt2,cv::Scalar(255,0,0),3);
    //    linePt1=cv::Point(0,contoursMat.rows*2);
    //    linePt2=cv::Point(contoursMat.cols,contoursMat.rows*2);
    //    cv::line(showMat,linePt1,linePt2,cv::Scalar(255,0,0),3);
    //    linePt1=cv::Point(0,contoursMat.rows*3);
    //    linePt2=cv::Point(contoursMat.cols,contoursMat.rows*3);
    //    cv::line(showMat,linePt1,linePt2,cv::Scalar(255,0,0),3);
    //    linePt1=cv::Point(0,contoursMat.rows*4);
    //    linePt2=cv::Point(contoursMat.cols,contoursMat.rows*4);
    //    cv::line(showMat,linePt1,linePt2,cv::Scalar(255,0,0),3);
    //    linePt1=cv::Point(0,contoursMat.rows*5);
    //    linePt2=cv::Point(contoursMat.cols,contoursMat.rows*5);
    //    cv::line(showMat,linePt1,linePt2,cv::Scalar(255,0,0),3);

    //    char positionText1[50]="Contours Mat";
    //    cv::Point textPosition1 = cv::Point(70, 120);
    //    cv::putText(showMat, positionText1, textPosition1, 2, 1.5, CV_RGB(255, 200, 100),2);
    //    char positionText2[50]="Threshold Mat";
    //    cv::Point textPosition2 = cv::Point(70, 120+contoursMat.rows);
    //    cv::putText(showMat, positionText2, textPosition2, 2, 1.5, CV_RGB(255, 200, 100),2);
    //    char positionText3[50]="MorphFilter Mat";
    //    cv::Point textPosition3 = cv::Point(70, 120+contoursMat.rows*2);
    //    cv::putText(showMat, positionText3, textPosition3, 2, 1.5, CV_RGB(255, 200, 100),2);
    //    char positionText4[50]="MaskMulSrc Mat";
    //    cv::Point textPosition4 = cv::Point(70, 120+contoursMat.rows*3);
    //    cv::putText(showMat, positionText4, textPosition4, 2, 1.5, CV_RGB(255, 200, 100),2);
    //    char positionText5[50]="Mask Mat";
    //    cv::Point textPosition5 = cv::Point(70, 120+contoursMat.rows*4);
    //    cv::putText(showMat, positionText5, textPosition5, 2, 1.5, CV_RGB(255, 200, 100),2);
    //    char positionText6[50]="Dilate Contour Mat";
    //    cv::Point textPosition6 = cv::Point(70, 120+contoursMat.rows*5);
    //    cv::putText(showMat, positionText6, textPosition6, 2, 1.5, CV_RGB(255, 200, 100),2);

    //    cv::imshow("test",resizeMat(showMat,2));
}

cv::Mat preProcessImg::createContourMask(const std::vector<std::vector<cv::Point> > &contours, cv::Size size)
{
    cv::Mat maskMat=cv::Mat(size,CV_8UC1,cv::Scalar::all(0));
    for(uint i=0;i<contours.size();++i)
    {
        cv::drawContours(maskMat,contours,i,cv::Scalar(255),-1);
    }
    cv::Mat erodeKernelMat=cv::getStructuringElement(cv::MORPH_RECT,cv::Size(2,2));
    // cv::erode(maskMat,maskMat,erodeKernelMat);
    return maskMat;
}

void preProcessImg::contourFilter(std::vector<std::vector<cv::Point>>&contours)
{
    auto iter=contours.begin();
    while(iter!=contours.end())
    {
        cv::RotatedRect tempRect=cv::minAreaRect(*iter);
        if(tempRect.size.height<10||tempRect.size.height>150||tempRect.size.width>150||tempRect.size.width<10)
            contours.erase(iter);
        else
            ++iter;
    }
}

void preProcessImg::thresholdImg()
{
    uint proFlag=0;
    switch (proFlag){
    case 0:
    {
        uint adaThrBlockSize=25;
        cv::adaptiveThreshold(roiMat,thresholdMat,255,cv::ADAPTIVE_THRESH_MEAN_C ,cv::THRESH_BINARY_INV,adaThrBlockSize,1);
        //cv::imshow("adaptiveThreshold",thresholdMat);
        //morphThresholdMat=thresholdMat.clone();
    }break;
    default:
        break;
    }
}

void preProcessImg::findAndFilterContours()
{
    ///////////////find and filter contours//////////////////
    std::vector<std::vector<cv::Point>> srcContours;
    std::vector<cv::Vec4i> srcHierarchy;
    cv::findContours(thresholdMat, srcContours, srcHierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    auto iter=srcContours.begin();

    for(;iter!=srcContours.end();++iter)
    {
        float _rectWidth=0;
        float _rectHeight=0;
        cv::Rect _rect=cv::boundingRect(*iter);
        _rectHeight=_rect.height;
        _rectWidth=_rect.width;
        if(_rectHeight<0.01)
            qDebug()<<"sadsadasdasd:"<<_rectHeight;
        float ratio=_rectWidth/_rectHeight;
        if(ratio<1.5&&_rectHeight>20&&_rectWidth<200&&_rectHeight<100)
        {
            filterContours.push_back(*iter);
        }
    }
    ///////////////draw source contours ////////////////
    //    for(int i=0;i<filterContours.size();++i)
    //    {
    //        cv::drawContours(contoursMat,filterContours,i,cv::Scalar(255,0,0),2);
    //    }
}

void preProcessImg::contoursMaskToExtractCodeRegion()
{
    maskMat=cv::Mat(cv::Size(roiMat.cols,roiMat.rows),CV_8UC1,cv::Scalar::all(0));
    for(int i=0;i<filterContours.size();++i)
    {
        cv::drawContours(maskMat,filterContours,i,cv::Scalar::all(255),-1);
    }
    maskMulSrcMat=thresholdMat.mul(maskMat);

    cv::Mat dilateKernelMat=cv::getStructuringElement(morphExtractCodeRegionShape,cv::Size(morphRectWidth,morphRectHeight));
    cv::dilate(maskMulSrcMat,dilateContourMat,dilateKernelMat);

    cv::findContours(dilateContourMat,validCodeRegionContours, validCodeRegionHierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    auto iter=validCodeRegionContours.begin();
    for(;iter!=validCodeRegionContours.end();++iter){
        cv::Rect _rect=cv::boundingRect(*iter);
        cv::Rect tempRect;
        tempRect.x=_rect.x;
        tempRect.y=_rect.y;
        tempRect.width=_rect.width;
        tempRect.height=_rect.height;

        if((uint)_rect.width>minValidRegionWidth){
            validCodeRect.push_back(_rect);
            cv::RotatedRect _rotatedRect=cv::minAreaRect(*iter);
            validCodeRotatedRect.push_back(_rotatedRect);
            validCodeRegionMat.push_back(roiMat(tempRect).clone());
        }
    }
}

void preProcessImg::rotateValidRect()
{
    for(int i=0;i<validCodeRotatedRect.size();++i)
    {
        int angle=(int)validCodeRotatedRect[i].angle;
        showTool.rotateMat(validCodeRegionMat[i],validCodeRegionMat[i],angle);
        //cv::imshow("validCodeRegionMat",validCodeRegionMat[i]);
    }
}

void preProcessImg::refineValidRect()
{
    for(int i=0;i<validCodeRegionMat.size();++i){
        cv::Mat tempMat=validCodeRegionMat[i].clone();
        cv::threshold(validCodeRegionMat[i],validCodeRegionMat[i],100,255,cv::THRESH_OTSU);
        cv::Mat erodeKernelMat=cv::getStructuringElement(cv::MORPH_RECT,cv::Size(2,2));
        cv::Mat dilateKernelMat=cv::getStructuringElement(cv::MORPH_RECT,cv::Size(23,2));
        cv::erode(validCodeRegionMat[i],validCodeRegionMat[i],erodeKernelMat);
        cv::dilate(validCodeRegionMat[i],validCodeRegionMat[i],dilateKernelMat);
        std::vector<std::vector<cv::Point>>contours;
        cv::findContours(validCodeRegionMat[i],contours,cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
        auto iter=contours.begin();
        cv::Rect maxRect;
        maxRect.width=0;
        while(iter!=contours.end())
        {
            cv::Rect tempRect=cv::boundingRect(*iter);
            if(tempRect.width>maxRect.width)
                maxRect=tempRect;
            ++iter;
        }
        if(maxRect.width==0){
            qDebug()<<"didnt detect any valid code region!";
            return;
        }
        cv::Mat _thresholdMat=tempMat(cv::Rect(maxRect));
        cv::threshold(_thresholdMat,_thresholdMat,255,255,cv::THRESH_OTSU);
        uint stretchHeight=20;
        cv::Mat _result=cv::Mat(_thresholdMat.rows+stretchHeight,_thresholdMat.cols,CV_8UC1,cv::Scalar::all(0));
        _thresholdMat.copyTo(_result(cv::Rect(0,stretchHeight/2,_thresholdMat.cols,_thresholdMat.rows)));
        validCodeRegionMat[i]=_result.clone();
    }
}

float preProcessImg::detectLineAngle(cv::Mat src)
{
    cv::Mat thrMat;
    cv::cvtColor(src,thrMat,cv::COLOR_RGB2GRAY);

    cv::threshold(thrMat,thrMat,100,255,cv::THRESH_BINARY);
    cv::medianBlur(thrMat,thrMat,5);
    cv::Mat cannyMat;
    cv::Canny(thrMat,cannyMat,50,200);
    //cv::imshow("cannymat",cannyMat);

    uint linePointThreshold=50;
    uint minLineLength=100;
    uint maxLinePointGap=10;
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(cannyMat,lines,1, CV_PI/180, linePointThreshold, minLineLength, maxLinePointGap);

    if(lines.size()==0){
        qDebug()<<"no line detected!";
        return 361;
    }
    cv::Vec4i maxLine;
    float resultSlope=0;
    maxLine[1]=0;
    for(size_t i = 0; i < lines.size();i++)
    {
        cv::Vec4i tempLine = lines[i];
        if(tempLine[1]>maxLine[1])
            maxLine=tempLine;
    }
    resultSlope=(maxLine[3]-maxLine[1])/(float)(maxLine[2]-maxLine[0]);
    resultLine[0]=0;
    resultLine[1]=maxLine[1];
    resultLine[2]=thrMat.cols;
    resultLine[3]=cvRound(resultSlope*(resultLine[2]-resultLine[0])+resultLine[1]);
    cv::line(contoursMat,cv::Point(resultLine[0], resultLine[1]), cv::Point(resultLine[2], resultLine[3]), cv::Scalar(0,0,255),2);

    pcbAngle=acos(resultSlope);
    pcbAngle=pcbAngle/3.14*180;
    qDebug()<<"pcbAngle:"<<pcbAngle;
    return pcbAngle;
}

bool preProcessImg::detectCharacterColor(cv::Mat src)
{
    uint thresholdValue=20;
    uint pointThreshold=200;
    uchar* matPtr=src.data;
    uint pixNum=src.cols*src.rows;
    uint validPointNum=0;
    for(uint i=0;i<pixNum;++i){
        if(matPtr[i]<thresholdValue)
            validPointNum++;
    }
    qDebug()<<"validPointNum:"<<validPointNum;
    if(validPointNum>=pointThreshold)
        return true;
}

int preProcessImg::detectLineAndSetRoiRegion(cv::Mat src,const cv::Mat&dstmask,cv::Mat&dst)
{
    if(src.empty()||dstmask.empty()||src.cols!=dstmask.cols||src.rows!=dstmask.rows)
        return ROI_UNCERTAIN;

    cv::Mat thrMat;
    if(src.channels()!=1)
        cv::cvtColor(src,thrMat,cv::COLOR_RGB2GRAY);
    else
        thrMat=src.clone();

    cv::threshold(thrMat,thrMat,100,255,cv::THRESH_BINARY);
    cv::medianBlur(thrMat,thrMat,5);
    cv::Mat cannyMat;
    cv::Canny(thrMat,cannyMat,50,200);

    uint linePointThreshold=50;
    uint minLineLength=100;
    uint maxLinePointGap=10;
    std::vector<cv::Vec4i> lines;
    cv::Vec4i resultLine;
    cv::HoughLinesP(cannyMat,lines,1, CV_PI/180, linePointThreshold, minLineLength, maxLinePointGap);
    if(lines.size()==0){
        return ROI_UNCERTAIN;
    }

    cv::Vec4i maxLine;
    maxLine[1]=0;
    for(size_t i = 0; i < lines.size();i++)
    {
        cv::Vec4i tempLine = lines[i];
        if(tempLine[1]>maxLine[1])
            maxLine=tempLine;
    }
    float resultSlope=(maxLine[3]-maxLine[1])/(float)(maxLine[2]-maxLine[0]);
    resultLine[0]=0;
    resultLine[1]=maxLine[1];
    resultLine[2]=thrMat.cols;
    resultLine[3]=cvRound(resultSlope*(resultLine[2]-resultLine[0])+resultLine[1]);
    cv::cvtColor(cannyMat,cannyMat,cv::COLOR_GRAY2BGR);
    cv::line(cannyMat,cv::Point(resultLine[0], resultLine[1]), cv::Point(resultLine[2], resultLine[3]), cv::Scalar(0,0,255),2);

    uint startY=resultLine[1]>resultLine[3]?resultLine[3]:resultLine[1];
    uint roiY=0;
    uint roiHeight=60;
    uchar* matPtr=thrMat.data;
    uchar* maskPtr=dstmask.data;
    while((startY-roiY)>0)
    {
        uint blackPtNum=0;
        uint index=thrMat.cols*(startY-roiY);
        for(uint i=0;i<thrMat.cols;++i){
            if(matPtr[index+i]==0&&maskPtr[index+i]!=0)
                blackPtNum++;
        }
        ++roiY;
        if((blackPtNum/(float)thrMat.cols)<0.1)
            break;
    }
    startY=(startY-roiY-roiHeight)>0?(startY-roiY-roiHeight):0;
    if((startY+roiHeight)>src.rows){
        return ROI_UNCERTAIN;
    }
    dst=src(cv::Rect(0,startY,src.cols,roiHeight)).clone();
    return RESULT_NORMAL;
}

void preProcessImg::setRoiParam(uint x, uint y, uint w, uint h)
{
    roiX=x;
    roiY=y;
    roiWidth=w;
    roiHeight=h;
}

void preProcessImg::slotSetPreProcessParam(uint _thresholdValue, uint _thresholdFlag,
                                           uint _morPreProShape, uint _morPreProSize,
                                           uint _minContoursSize, uint _maxContoursSize,
                                           uint _morphExtractCodeRegionShape, uint _morphRectWidth, uint _morphRectHeight)
{
    thresholdValue=_thresholdValue;
    thresholdFlag=_thresholdFlag;

    morPreProShape=_morPreProShape;
    morPreProSize=_morPreProSize;

    minContoursSize=_minContoursSize;
    maxContoursSize=_maxContoursSize;

    morphExtractCodeRegionShape=_morphExtractCodeRegionShape;
    morphRectWidth=_morphRectWidth;
    morphRectHeight=_morphRectHeight;
}

void preProcessImg::setRoiMat()
{
    roiMat=sourceMat(cv::Rect(roiX,roiY,roiWidth,roiHeight));
    if(roiMat.channels()!=1)
        cv::cvtColor(roiMat,roiMat,cv::COLOR_RGB2GRAY);
    //  cv::imshow("roiMat",roiMat);
}

void preProcessImg::morphToFilter()
{
    cv::Mat erodeKernelMat=cv::getStructuringElement(morPreProShape,cv::Size(morPreProSize,morPreProSize));
    cv::Mat dilateKernelMat=cv::getStructuringElement(morPreProShape,cv::Size(9,9));
    cv::dilate(thresholdMat,morphFilterMat,dilateKernelMat);
    cv::erode(morphFilterMat,morphFilterMat,erodeKernelMat);
    //cv::imshow("morphFilterMat",morphFilterMat);
}

cv::Mat preProcessImg::resizeMat(const cv::Mat& src, float i)
{
    cv::Mat dst;
    cv::resize(src, dst, cv::Size(cvRound( src.cols * i),cvRound( src.rows * i)),cv::INTER_LINEAR);
    return dst;
}
