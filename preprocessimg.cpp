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
    if(angle>=srcImgAngleRange||angle<=-srcImgAngleRange)
        return ANGLE_UNCERTAIN;

    /////////rotate the img and mask////////////////////////
    cv::Mat dstRotate;
    cv::Mat dstMask=cv::Mat(roiMat.rows,roiMat.cols,CV_8UC1,cv::Scalar::all(255));
    showTool.rotateMat(dstMask,dstMask,cvRound(angle));
    showTool.rotateMat(roiMat,dstRotate,cvRound(angle));

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

    //////////morphology process the threshold mul mask mat//////
    contoursMaskToExtractCodeRegion();

    /////////use the detected rotatedrect's angle to rotate valid code region mat///////
    //rotateValidRect();

    /////////refine the valid code region and threshod it in OTSU method////////////////
    refineValidRect();

    ///////save the valid code region////////////
    saveValidCodeMat();

    ///////return the num of valid code region/////////
    return validCodeRegionMat.size();
}

void preProcessImg::processDataIni()
{
    filterContours.clear();
    validCodeRegionMat.clear();
    validCodeRegionContours.clear();
    validCodeRegionHierarchy.clear();
    validCodeRect.clear();
    validCodeRotatedRect.clear();

    pcbAngle=361;
}

void preProcessImg::thresholdImg()
{
    uint proFlag=0;
    switch (proFlag){
    case 0:
    {
        uint adaThrBlockSize=25;
        cv::adaptiveThreshold(roiMat,thresholdMat,255,cv::ADAPTIVE_THRESH_MEAN_C ,cv::THRESH_BINARY_INV,adaThrBlockSize,1);
       // cv::imshow("adaptiveThreshold",thresholdMat);
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
        float ratio=_rectWidth/_rectHeight;
        if(ratio<1.5&&_rectHeight>20&&_rectWidth<200&&_rectHeight<100)
        {
            filterContours.push_back(*iter);
        }
    }
    ///////////////draw source contours ////////////////
//        for(int i=0;i<filterContours.size();++i)
//        {
//            cv::drawContours(contoursMat,filterContours,i,cv::Scalar(255,0,0),2);
//        }
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
 //   cv::imshow("dilateContourMat",dilateContourMat);
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
      //  cv::imshow("_thresholdMat",_thresholdMat);
        rectifyMat(_thresholdMat);
      //  cv::imshow("rectify_thresholdMat",_thresholdMat);
        cv::threshold(_thresholdMat,_thresholdMat,255,255,cv::THRESH_OTSU);
        uint stretchHeight=20;
        cv::Mat _result=cv::Mat(_thresholdMat.rows+stretchHeight,_thresholdMat.cols,CV_8UC1,cv::Scalar::all(0));
        _thresholdMat.copyTo(_result(cv::Rect(0,stretchHeight/2,_thresholdMat.cols,_thresholdMat.rows)));
        validCodeRegionMat[i]=_result.clone();
        cv::imshow(QString::number(i,10).toStdString(),validCodeRegionMat[i]);
    }
}

float preProcessImg::detectLineAngle(cv::Mat src)
{
    //return 361;
    cv::Mat thrMat;
    if(src.channels()!=1)
        cv::cvtColor(src,thrMat,cv::COLOR_RGB2GRAY);
    else
        thrMat=src.clone();

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
    //cv::line(contoursMat,cv::Point(resultLine[0], resultLine[1]), cv::Point(resultLine[2], resultLine[3]), cv::Scalar(0,0,255),2);
   // qDebug()<<"slope:"<<resultSlope;
    pcbAngle=atan(resultSlope);
    pcbAngle=pcbAngle/3.14*180;
  //  qDebug()<<"angle:"<<pcbAngle;
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
 //   qDebug()<<"validPointNum:"<<validPointNum;
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
    uint minLineLength=80;
    uint maxLinePointGap=10;
    std::vector<cv::Vec4i> lines;
    cv::Vec4i resultLine;
//    cv::HoughLinesP(cannyMat(cv::Rect(0,cannyMat.rows/2,cannyMat.cols,cannyMat.rows/2)),
//                    lines,1, CV_PI/180, linePointThreshold, minLineLength, maxLinePointGap);
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
    float angle=atan(resultSlope);
    angle=angle*180/3.14;
  //  qDebug()<<"slope:"<<resultSlope;
  //  qDebug()<<"roi angle:"<<angle;
    if(angle>roiImgAngleRange||angle<-roiImgAngleRange)
        return ROI_UNCERTAIN;
   // qDebug()<<"slope:"<<resultSlope;
    resultLine[0]=maxLine[0];
    resultLine[1]=maxLine[1];
    resultLine[2]=thrMat.cols;
    resultLine[3]=cvRound(resultSlope*(resultLine[2]-resultLine[0]))+resultLine[1];
    //cv::cvtColor(cannyMat,cannyMat,cv::COLOR_GRAY2BGR);
    //cv::line(cannyMat,cv::Point(resultLine[0], resultLine[1]), cv::Point(resultLine[2], resultLine[3]), cv::Scalar(0,0,255),2);

    for(int i=0;i<4;++i)
    {
        qDebug()<<resultLine[i];
    }
    int startY=(resultLine[1]>resultLine[3])?resultLine[3]:resultLine[1];
    int roiY=0;
    int roiHeight=50;
    uchar* matPtr=thrMat.data;
    uchar* maskPtr=dstmask.data;
//return ROI_UNCERTAIN;
  //  cv::imshow("src",src);
   // cv::imshow("mask",dstmask);
 //   return ROI_UNCERTAIN;
    while((startY-roiY)>0)
    {
        int blackPtNum=0;
        int index=thrMat.cols*(startY-roiY);
        for(uint i=0;i<thrMat.cols;++i){
            if(matPtr[index+i]==0&&maskPtr[index+i]==255)
                blackPtNum++;
        }
        ++roiY;
        if((blackPtNum/(float)thrMat.cols)<0.1)
            break;
    }
    //return ROI_UNCERTAIN;
    startY=((startY-roiY-roiHeight)>0)?(startY-roiY-roiHeight):0;
    if((startY+roiHeight)>src.rows){
        return ROI_UNCERTAIN;
    }
    if(startY+roiHeight>src.rows)
    {
        roiHeight=src.rows-startY;
       // qDebug()<<"startY+roiHeight>src.rows";
    }

  //return ROI_UNCERTAIN;
//    qDebug()<<"src.cols:"<<src.cols;
//    qDebug()<<"src.rows:"<<src.rows;
//    qDebug()<<"startY:"<<startY;
//    qDebug()<<"roiHeight:"<<roiHeight;
    dst=src(cv::Rect(0,startY,src.cols,roiHeight)).clone();
   // cv::imshow("dstss",dst);
    return RESULT_NORMAL;
}

void preProcessImg::rectifyMat(cv::Mat &mat)
{
    uchar* dataPtr=mat.data;
    uint num=mat.cols*mat.rows;
    uint thresholdValue=100;
    uint rectifyValue=170;
    for(uint i=0;i<num;++i)
    {
        if(dataPtr[i]<thresholdValue)
            dataPtr[i]=rectifyValue;
    }
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

void preProcessImg::morphToFilter()
{
    cv::Mat erodeKernelMat=cv::getStructuringElement(morPreProShape,cv::Size(morPreProSize,morPreProSize));
    cv::Mat dilateKernelMat=cv::getStructuringElement(morPreProShape,cv::Size(9,9));
    cv::dilate(thresholdMat,morphFilterMat,dilateKernelMat);
    cv::erode(morphFilterMat,morphFilterMat,erodeKernelMat);
    //cv::imshow("morphFilterMat",morphFilterMat);
}
