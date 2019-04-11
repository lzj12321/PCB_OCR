#include "settingsdlg.h"
#include "ui_settingsdlg.h"
#include<QDebug>
#include<QMessageBox>
#include<QFileDialog>
#include<QSettings>
#include<QTextCodec>

settingsDlg::settingsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsDlg)
{
    ui->setupUi(this);
    sourceMat=cv::imread("123.BMP",1);
    paramLoad();
    settingsDlgIni();
}

settingsDlg::~settingsDlg()
{
    delete ui;
}

void settingsDlg::getRoiParam(uint &_roiX, uint &_roiY, uint &_roiWidth, uint &_roiHeight)
{
    _roiX=roiX;
    _roiY=roiY;
    _roiWidth=roiWidth;
    _roiHeight=roiHeight;
}

void settingsDlg::settingsDlgIni()
{
    cv::Mat tempMat=sourceMat.clone();
    cv::rectangle(tempMat,cv::Rect(roiX,roiY,roiWidth,roiHeight),roiColour,3);
    showTool.showMatOnDlg(tempMat,ui->label,&ratio);
    ui->label_3->setText("x:"+QString::number(0,10)+" y:"+QString::number(0,10));

    offsetX=(ui->label->width()-(sourceMat.cols/ratio))/2;
    offsetY=(ui->label->height()-(sourceMat.rows/ratio))/2;

    pt1X=roiX;
    pt1Y=roiY;
    pt2X=roiX+roiWidth;
    pt2Y=roiY+roiHeight;
}

void settingsDlg::restorePaintingStatus()
{
    pRoiStatus=false;
}

void settingsDlg::drawRoiOnMat(cv::Mat &src)
{
    int x,y,w,h;
    x=roiX;
    y=roiY;
    w=roiWidth;
    h=roiHeight;

    if(roiX+roiWidth<=0||roiY+roiHeight<=0){
        return;
    }
    if((roiX+roiWidth)>src.cols){
        x=0;
        w=src.cols;
    }
    if((roiY+roiHeight)>src.rows){
        y=0;
        h=src.rows;
    }
    cv::rectangle(src,cv::Rect(x,y,w,h),roiColour,4);
}

void settingsDlg::mousePressEvent(QMouseEvent *event)
{
    if(event->x()>=ui->label->x()
            &&event->x()<=(ui->label->x()+ui->label->width())
            &&event->y()>=ui->label->y()
            &&event->y()<=(ui->label->y()+ui->label->height())){
        int w=cvRound((event->x()-ui->label->x()-offsetX)*ratio);
        int h=cvRound((event->y()-ui->label->y()-offsetY)*ratio);
        ui->label_3->setText("x:"+QString::number(w>0?w:0,10)+" y:"+QString::number(h>0?h:0,10));
        if(pRoiStatus){
            pt1X=w;
            pt1Y=h;
        }
    }
}

void settingsDlg::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->x()>=ui->label->x()&&
            event->x()<=(ui->label->x()+ui->label->width())&&
            event->y()>=ui->label->y()&&
            event->y()<=(ui->label->y()+ui->label->height())){
        int w=cvRound((event->x()-ui->label->x()-offsetX)*ratio);
        int h=cvRound((event->y()-ui->label->y()-offsetY)*ratio);
        ui->label_3->setText("x:"+QString::number(w>0?w:0,10)+" y:"+QString::number(h>0?h:0,10));
        if(pRoiStatus){
            pt2X=w;
            pt2Y=h;
            cv::Mat t=sourceMat.clone();
            if(t.channels()==1)
                cv::cvtColor(t,t,cv::COLOR_GRAY2RGB);
            cv::rectangle(t,cv::Point(pt1X,pt1Y),cv::Point(pt2X,pt2Y),roiColour,3);
            showTool.showMatOnDlg(t,ui->label);
            pRoiStatus=false;
        }
    }
}

void settingsDlg::mouseMoveEvent(QMouseEvent *event)
{
    if(event->x()>=ui->label->x()&&
            event->x()<=(ui->label->x()+ui->label->width())&&
            event->y()>=ui->label->y()&&
            event->y()<=(ui->label->y()+ui->label->height()))
    {
        int w=cvRound((event->x()-ui->label->x()-offsetX)*ratio);
        int h=cvRound((event->y()-ui->label->y()-offsetY)*ratio);
        ui->label_3->setText("x:"+QString::number((w>0&&w<sourceMat.cols)?w:0,10)+" y:"+QString::number((h>0&&h<sourceMat.rows)?h:0,10));
        if(pRoiStatus)
        {
            int pt2X=w;
            int pt2Y=h;
            cv::Mat t=sourceMat.clone();
            if(t.channels()==1)
                cv::cvtColor(t,t,cv::COLOR_GRAY2RGB);
            cv::rectangle(t,cv::Point(pt1X,pt1Y),cv::Point(pt2X,pt2Y),roiColour,3);
            showTool.showMatOnDlg(t,ui->label);
            isRoiChanged=true;
        }
    }
}

void settingsDlg::closeEvent(QCloseEvent *event)
{
    settingsDlgIni();
}

void settingsDlg::on_pushButton_clicked()
{
    pRoiStatus=true;
    showTool.showMatOnDlg(sourceMat,ui->label);
}

void settingsDlg::on_pushButton_2_clicked()
{
    cv::Mat tempMat=sourceMat.clone();
    drawRoiOnMat(tempMat);
    showTool.showMatOnDlg(tempMat,ui->label);
    if(isRoiChanged)
    {
        roiX=pt1X;
        roiY=pt1Y;
        roiWidth=abs(pt1X-pt2X);
        roiHeight=abs(pt1Y-pt2Y);
    }
    settingsDlgIni();
    emit roiChanged(roiX,roiY,roiWidth,roiHeight);
    paramSave();
    isRoiChanged=false;
    close();
}

void settingsDlg::on_pushButton_3_clicked()
{
    isRoiChanged=false;
    settingsDlgIni();
    close();
}

void settingsDlg::on_pushButton_5_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image File (*.jpg *.png *.bmp)"));
    QTextCodec *code = QTextCodec::codecForName("gb18030");
    std::string name = code->fromUnicode(filename).data();
    if(!name.empty())
    {
        cv::Mat src=cv::imread(name,1);
        if(!src.empty())
        {
            showTool.showMatOnDlg(src,ui->label);
            sourceMat=src.clone();
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Image Data Is Null");
            msgBox.exec();
        }
    }
    settingsDlgIni();
}

void settingsDlg::paramSave()
{
    QSettings paramWrite("param.ini",QSettings::IniFormat);
    paramWrite.setValue("ROI/roiX",roiX);
    paramWrite.setValue("ROI/roiY",roiY);
    paramWrite.setValue("ROI/roiWidth",roiWidth);
    paramWrite.setValue("ROI/roiHeight",roiHeight);
}

void settingsDlg::paramLoad()
{
    QSettings paramRead("param.ini",QSettings::IniFormat);
    roiX=paramRead.value("ROI/roiX").toInt();
    roiY=paramRead.value("ROI/roiY").toInt();
    roiWidth=paramRead.value("ROI/roiWidth").toInt();
    roiHeight=paramRead.value("ROI/roiHeight").toInt();
}

void settingsDlg::on_pushButton_4_clicked()
{

}
