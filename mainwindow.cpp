#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFileDialog>
#include<QTextCodec>
#include<QDebug>
#include<QMessageBox>
#include<iostream>
#include<QProcess>
#include<QFile>
#include<QTextStream>
#include<QTime>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setDlg=new settingsDlg;
    iconPreProcess();
    ui->lineEdit->setReadOnly(true);
    showFullScreen();
    openCamera();
}

MainWindow::~MainWindow()
{
    delete setDlg;
    delete ui;
}

void MainWindow::iconPreProcess()
{
    QPixmap logo("PI_LOGO.jpg");
    int width=ui->label_10->width();
    int height=ui->label_10->height();
    QPixmap fitPixMap=logo.scaled(width,height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    ui->label_10->setPixmap(fitPixMap);
}

void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image File (*.jpg *.png *.bmp)"));
    QTextCodec *code = QTextCodec::codecForName("gb18030");
    std::string name = code->fromUnicode(filename).data();
    if(!name.empty())
    {
        cv::Mat src=cv::imread(name,1);
        if(!src.empty())
        {
            showTool.showMatOnDlg(src,ui->label_3);
            sourceMat=src.clone();
            ui->lineEdit->clear();
            ui->lineEdit->setStyleSheet("background-color: rgb(255,255,255);");
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Image Data Is Null");
            msgBox.exec();
        }
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    setDlg->show();
}

void MainWindow::roiChanged(uint _roiX,uint _roiY,uint _roiWidth,uint _roiHeight)
{
    processTool.setRoiParam(_roiX,_roiY,_roiWidth,_roiHeight);
}

void MainWindow::on_pushButton_2_clicked()
{
    processImg();

//    QTime timer;
//    timer.start();
//    codes.clear();
//    ui->lineEdit->clear();
//    //    cv::Mat _dst;
//    //    showTool.rotate3CMat(sourceMat,_dst,10);
//    //    cv::imwrite("dst.bmp",_dst);
//    if(sourceMat.empty())
//    {
//        QMessageBox::warning(NULL,"Warning","Empty Img!");
//        return;
//    }
//    int validCodeRegionNum=processTool.extractValidCodeRegion(getRoiMat(roiX,roiY,roiWidth,roiHeight));
//    recognizeCode(validCodeRegionNum);
//    outputProcessResult(validCodeRegionNum);
//    QString codeStr="";
//    for(int i=0;i<codes.size();++i)
//    {
//        codeStr+="   ";
//        codeStr+=codes[i];
//    }
//    if(codeStr.remove(" ")!=""&&codeStr.remove(" ")!="\f")
//    {
//        ui->lineEdit->setStyleSheet("background-color: rgb(0,255,0);");
//        ui->lineEdit->setText(codeStr);
//    }
//    else
//    {
//        ui->lineEdit->setStyleSheet("background-color: rgb(255,0,0);");
//        ui->lineEdit->setText("未识别到有效字符串！");
//    }
//    qDebug()<<"code:"<<codeStr;
//    processTime=timer.elapsed();
}

cv::Mat MainWindow::getRoiMat(uint& roiX,uint& roiY,uint& roiWidth,uint& roiHeight)
{
    setDlg->getRoiParam(roiX,roiY,roiWidth,roiHeight);
    if(roiX>sourceMat.cols||roiY>sourceMat.rows)
        return sourceMat;
    if((roiX+roiWidth)>sourceMat.cols||(roiY+roiHeight)>sourceMat.rows)
        return(sourceMat(cv::Rect(roiX,roiY,sourceMat.cols-roiX,sourceMat.rows-roiY)));
    return sourceMat(cv::Rect(roiX,roiY,roiWidth,roiHeight));
    // processTool.setRoiParam(roiX,roiY,roiWidth,roiHeight);
}

void MainWindow::recognizeCode(int validCodeRegionNum)
{
    for(uint i=0;i<validCodeRegionNum;++i)
    {
        QProcess process;
        QString imgPath=QString::number(i+1,10)+".tif";
        QString cmd ="tesseract "+imgPath+" recognizeResult -l num2";
        process.start(cmd);
        if(!process.waitForStarted())
        {
            QMessageBox::warning(NULL,"Warning","Command execute error!");
        }
        if(!process.waitForFinished())
        {
            QMessageBox::warning(NULL,"Warning","Command execute timeout!");
        }
        QString tempCode=readResultFromTxt("recognizeResult.txt");
        if(tempCode.remove(" ")!="")
            codes.push_back(tempCode);
    }
}

QString MainWindow::readResultFromTxt(QString txtPath)
{
    QString result="";
    QFile file(txtPath);
    if(!file.open(QFile::ReadOnly|QIODevice::Text)){
        QMessageBox::warning(NULL,"Warning!","failed to open result txt!");
        return result;
    }

    QTextStream dataStream(&file);
    while(!dataStream.atEnd()){
        result=dataStream.readLine();
        qDebug()<<"result:"<<result;
        if(confirmResultIsValid(result))
            break;
    }
    file.close();
    return result;
}

bool MainWindow::confirmResultIsValid(QString resultCode)
{
    return true;
    if(resultCode.contains("B55D"))
        return true;
    else
        return false;
}

void MainWindow::outputProcessResult(int validCodeRegionNum)
{
    ui->textEdit->clear();

    QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->textEdit->append(time);

    switch(validCodeRegionNum){
    case INVALID_INPUT_IMG:{
        ui->textEdit->append("Result:INVALID_INPUT_IMG");
    }break;
    case ANGLE_UNCERTAIN:{
        ui->textEdit->append("Result:ANGLE_UNCERTAIN");
    }break;
    case ROI_UNCERTAIN:{
        ui->textEdit->append("Result:ROI_UNCERTAIN");
    }break;
    default:{
        ui->textEdit->append("PCB ANGLE:"+QString::number((int)processTool.pcbAngle,10));
        ui->textEdit->append("Valid Code Region Num:"+QString::number(processTool.validCodeRegionMat.size(),10));
        ui->textEdit->append("Used Time:"+QString::number(processTime,10)+"ms");
        for(uint i=0;i<codes.size();++i)
        {
            qDebug()<<"codes:"<<codes[i];
            ui->textEdit->append("Recognize code:"+codes[i]);
        }
    };
    }
    showTool.showMatOnDlg(drawResultMat(),ui->label_3);
}

cv::Mat MainWindow::drawResultMat()
{
    cv::Mat roiMat=getRoiMat(roiX,roiY,roiWidth,roiHeight);
    cv::Mat resultMat=sourceMat.clone();
    if(resultMat.channels()==1)
        cv::cvtColor(resultMat,resultMat,cv::COLOR_GRAY2RGB);
    if(roiMat.channels()==1)
        cv::cvtColor(roiMat,roiMat,cv::COLOR_GRAY2RGB);

    uint drawY=10;
    for(uint i=0;i<processTool.validCodeRotatedRect.size();++i){
        //showTool.drawRotatedRect(roiMat,processTool.validCodeRotatedRect[i],cv::Scalar(0,255,0),3);
        cv::Mat mat=processTool.validCodeRegionMat[i];
        if(mat.channels()==1)
            cv::cvtColor(mat,mat,cv::COLOR_GRAY2RGB);
        mat.copyTo(resultMat(cv::Rect(0,drawY,mat.cols,mat.rows)));
        drawY+=mat.rows+3;
    }

    cv::line(roiMat,cv::Point(processTool.resultLine[0], processTool.resultLine[1]),
            cv::Point(processTool.resultLine[2], processTool.resultLine[3]), cv::Scalar(0,0,255),3);

    showTool.drawROI(resultMat,cv::Rect(roiX,roiY,roiWidth,roiHeight));

    roiMat.copyTo(resultMat(cv::Rect(roiX,roiY,roiWidth,roiHeight)));
    return resultMat;
}

bool MainWindow::openCamera()
{
    bool cameraStatus=cameraTool.openCamera();
    if(cameraStatus)
    {
        cameraTool.setExpose(setDlg->getExposeTime());
        qDebug()<<setDlg->getExposeTime();
    }
    else
        ui->label_3->setText("相机掉线!");
    return cameraStatus;
}

void MainWindow::processImg()
{
    QTime timer;
    timer.start();
    codes.clear();
    ui->lineEdit->clear();
    if(sourceMat.empty())
    {
        QMessageBox::warning(NULL,"Warning","Empty Img!");
        return;
    }
    int validCodeRegionNum=processTool.extractValidCodeRegion(getRoiMat(roiX,roiY,roiWidth,roiHeight));
    recognizeCode(validCodeRegionNum);
    outputProcessResult(validCodeRegionNum);
    QString codeStr="";
    for(int i=0;i<codes.size();++i)
    {
        codeStr+="   ";
        codeStr+=codes[i];
    }
    if(codeStr.remove(" ")!=""&&codeStr.remove(" ")!="\f")
    {
        ui->lineEdit->setStyleSheet("background-color: rgb(0,255,0);");
        ui->lineEdit->setText(codeStr);
    }
    else
    {
        ui->lineEdit->setStyleSheet("background-color: rgb(255,0,0);");
        ui->lineEdit->setText("未识别到有效字符串！");
    }
    processTime=timer.elapsed();
}

void MainWindow::slotStartGrab(QLabel * label)
{

}

void MainWindow::slotStopGrab()
{

}

void MainWindow::on_pushButton_4_clicked()
{
    //cv::Mat mat=cameraTool.grabImg();
    sourceMat=cameraTool.grabImg().clone();
    processImg();
   // showTool.showMatOnDlg(mat,ui->label_3);
   // cv::imshow("grabImg",showTool.resizeMat(mat,0.5));
}
