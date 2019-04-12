#ifndef CVANDUI_H
#define CVANDUI_H
#include<QLabel>
#include<opencv2/opencv.hpp>
#include<opencv2/core.hpp>
#include<opencv2/highgui.hpp>

#define PI 3.14159

class cvAndUi
{
public:
    cvAndUi();
    double adjustMatSizeToShow(cv::Mat&mat,cv::Mat&dst, QLabel *label);
    cv::Mat resizeMat(cv::Mat& src, float i);
    void showMatOnDlg(cv::Mat &mat, QLabel*label,double* ratioPtr=nullptr);
    void drawRotatedRect(cv::Mat&src,cv::RotatedRect&r_rec,cv::Scalar color=cv::Scalar(255,200,50),uint thickness=2);
    void rotateMat(cv::Mat&temp,cv::Mat&dst, int rotateAngle);
    void rotate3CMat(cv::Mat&temp,cv::Mat&dst, int rotateAngle);
    void drawROI(cv::Mat&mat,cv::Rect roi,cv::Scalar color=cv::Scalar(255,0,0),int thickness=3);
    void drawROI(cv::Mat&mat,uint roix,uint roiy,uint w,uint h,cv::Scalar color=cv::Scalar(255,0,0),int thickness=3);
    cv::Point getPointAffinedPos(const cv::Point &src, const cv::Point &center,const cv::Point&center2, double angle);
};

#endif // CVANDUI_H
