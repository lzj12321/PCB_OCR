#include "cvandui.h"
#include<QImage>
#include<QMessageBox>
#include <QFileDialog>
#include <QTextCodec>
#include<QDebug>

cvAndUi::cvAndUi()
{

}

double cvAndUi::adjustMatSizeToShow(cv::Mat&mat,cv::Mat&dst, QLabel *label)
{
    double ratio=0;
    double colRatio=mat.cols/(double)label->width();
    double rowRatio=mat.rows/(double)label->height();
    ratio=colRatio>rowRatio?colRatio:rowRatio;
    dst=resizeMat(mat,1/ratio);
    return ratio;
}

cv::Mat cvAndUi::resizeMat(cv::Mat &src, float i)
{
    cv::Mat dst;
    cv::resize(src, dst, cv::Size(cvRound( src.cols * i),cvRound( src.rows * i)));
    return dst;
}

void cvAndUi::showMatOnDlg(cv::Mat &mat, QLabel *label,double* ratioPtr)
{
    if(mat.empty())
    {
        qDebug()<<"mat.empty()";
        return;
    }
    QImage imageShow;
    cv::Mat resizeMat;
    double r=0;
    r=adjustMatSizeToShow(mat,resizeMat,label);
    if(ratioPtr!=nullptr)
        *ratioPtr=r;
    if(resizeMat.type() == CV_8UC1)
    {
        QImage image(resizeMat.cols, resizeMat.rows, QImage::Format_Indexed8);
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        uchar *pSrc = resizeMat.data;
        for(int row = 0; row < resizeMat.rows; row++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, resizeMat.cols);
            pSrc += resizeMat.step;
        }
        imageShow=image;
    }
    else if(resizeMat.type() == CV_8UC3)
    {
        const uchar *pSrc = (const uchar*)resizeMat.data;
        QImage image(pSrc, resizeMat.cols, resizeMat.rows, resizeMat.step, QImage::Format_RGB888);
        imageShow=image.rgbSwapped();
    }
    else if(resizeMat.type() == CV_8UC4)
    {
        const uchar *pSrc = (const uchar*)resizeMat.data;
        QImage image(pSrc, resizeMat.cols, resizeMat.rows, resizeMat.step, QImage::Format_ARGB32);
        imageShow=image;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText( "ERROR: Mat could not be converted to QImage.");
        msgBox.exec();
    }
    label->setPixmap(QPixmap::fromImage(imageShow));
    label->show();
}

void cvAndUi::drawRotatedRect(cv::Mat &src, cv::RotatedRect &r_rec, cv::Scalar color,uint thickness)
{
    cv::Point2f vertices[4];
    r_rec.points(vertices);
    for (int j = 0; j < 4; ++j){
        cv::line(src, vertices[j], vertices[(j + 1) % 4],color,thickness);
    }
}

void cvAndUi::rotateMat(cv::Mat&temp,cv::Mat&dst, int rotateAngle)
{
    double width=sqrt(pow(temp.rows,2)+pow(temp.cols,2));
    width=cvRound(width);
    cv::Size2f rotateResultImgSize=cv::Size2f(width,width);
    cv::Mat rotateTemp;
    rotateTemp=cv::Mat(rotateResultImgSize,CV_8UC1,cv::Scalar::all(0));

    ushort offsetX=cvRound(width/2.0-temp.cols/2.0);
    ushort offsetY=cvRound(width/2.0-temp.rows/2.0);
    cv::Mat temporyMat=rotateTemp(cv::Rect(offsetX,offsetY,temp.cols,temp.rows));
    temp.copyTo(temporyMat);

    cv::Point rotateTempCenter=cv::Point(cvRound(rotateTemp.cols/2.0),cvRound(rotateTemp.rows/2.0));
    cv::Mat rotateTempMat = cv::getRotationMatrix2D(rotateTempCenter,rotateAngle, 1.0);
    cv::warpAffine(rotateTemp, rotateTemp, rotateTempMat,rotateResultImgSize,1,0,0);

    double angle=rotateAngle*PI/180.0;
    double sinx=fabs(sin(angle));
    double cosx=fabs(cos(angle));
    ushort h=cvRound(temp.cols*sinx+temp.rows*cosx);
    ushort w=cvRound(temp.cols*cosx+temp.rows*sinx);
    cv::Size2i sizex=cv::Size2i(w,h);
    dst=cv::Mat(sizex,CV_8UC1,cv::Scalar::all(0));

    cv::Point startPoint=cv::Point(cvRound(rotateTempCenter.x-w/2.0),cvRound(rotateTempCenter.y-h/2.0));
    rotateTemp(cv::Rect(startPoint.x,startPoint.y,w,h)).copyTo(dst);
}

void cvAndUi::rotate3CMat(cv::Mat&temp,cv::Mat&dst, int rotateAngle)
{
    double width=sqrt(pow(temp.rows,2)+pow(temp.cols,2));
    width=cvRound(width);
    cv::Size2f rotateResultImgSize=cv::Size2f(width,width);
    cv::Mat rotateTemp;
    rotateTemp=cv::Mat(rotateResultImgSize,CV_8UC3,cv::Scalar::all(0));

    ushort offsetX=cvRound(width/2.0-temp.cols/2.0);
    ushort offsetY=cvRound(width/2.0-temp.rows/2.0);
    cv::Mat temporyMat=rotateTemp(cv::Rect(offsetX,offsetY,temp.cols,temp.rows));
    temp.copyTo(temporyMat);

    cv::Point rotateTempCenter=cv::Point(cvRound(rotateTemp.cols/2.0),cvRound(rotateTemp.rows/2.0));
    cv::Mat rotateTempMat = cv::getRotationMatrix2D(rotateTempCenter,rotateAngle, 1.0);
    cv::warpAffine(rotateTemp, rotateTemp, rotateTempMat,rotateResultImgSize,1,0,0);

    double angle=rotateAngle*PI/180.0;
    double sinx=fabs(sin(angle));
    double cosx=fabs(cos(angle));
    ushort h=cvRound(temp.cols*sinx+temp.rows*cosx);
    ushort w=cvRound(temp.cols*cosx+temp.rows*sinx);
    cv::Size2i sizex=cv::Size2i(w,h);
    dst=cv::Mat(sizex,CV_8UC3,cv::Scalar::all(0));

    cv::Point startPoint=cv::Point(cvRound(rotateTempCenter.x-w/2.0),cvRound(rotateTempCenter.y-h/2.0));
    rotateTemp(cv::Rect(startPoint.x,startPoint.y,w,h)).copyTo(dst);
}

void cvAndUi::drawROI(cv::Mat &mat, cv::Rect roi,cv::Scalar color,int thickness)
{
    cv::rectangle(mat,roi,color,thickness);
}

void cvAndUi::drawROI(cv::Mat &mat, uint roix, uint roiy, uint w, uint h,cv::Scalar color,int thickness)
{
    cv::Rect rect=cv::Rect(roix,roiy,w,h);
    cv::rectangle(mat,rect,color,thickness);
}

cv::Point cvAndUi::getPointAffinedPos(const cv::Point &src, const cv::Point &center,const cv::Point&center2, double angle)
{
    cv::Point dst;
    int x = src.x - center.x;
    int y = src.y - center.y;
    dst.x = cvRound(x * cos(angle) + y * sin(angle) + center2.x);
    dst.y = cvRound(-x * sin(angle) + y * cos(angle) + center2.y);
    qDebug()<<"cos angle:"<<cos(angle);
    return dst;
}

float cvAndUi::pointTolineDistance(const cv::Point &pt, const cv::Vec4i &line)
{
    cv::Point lineP1,lineP2;
    lineP1=cv::Point(line[0],line[1]);
    lineP2=cv::Point(line[2],line[3]);

    cv::Point v1,v2;
    v1=cv::Point(pt.x-lineP1.x,pt.y-lineP1.y);
    v2=cv::Point(lineP2.x-lineP1.x,lineP2.y-lineP1.y);

    float m_v1v2=sqrt(pow(v1.x,2)+pow(v1.y,2))*sqrt(pow(v2.x,2)+pow(v2.y,2));
    float angle=acos((v1.x*v2.x+v1.y*v2.y)/(m_v1v2));

    float distance=sqrt(pow(v1.x,2)+pow(v1.y,2))*sin(angle);
    return distance;
}
