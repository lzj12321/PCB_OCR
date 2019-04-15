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
#include<QDateTime>
#include<QTime>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setDlg=new settingsDlg;
    iconPreProcess();
    ui->lineEdit->setReadOnly(true);
    openCamera();
    iniTesseract();
    connect(setDlg,SIGNAL(cameraParamChanged(uint,uint)),this,SLOT(slotCameraParamChanged(uint,uint)));
    showFullScreen();
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

void MainWindow::on_pushButton_2_clicked()
{
    extractCodeFromImg();
    showResultMat();
}

cv::Mat MainWindow::getRoiMat(uint& roiX,uint& roiY,uint& roiWidth,uint& roiHeight)
{
    setDlg->getRoiParam(roiX,roiY,roiWidth,roiHeight);
    if(roiX>sourceMat.cols||roiY>sourceMat.rows)
        return sourceMat.clone();
    if((roiX+roiWidth)>sourceMat.cols||(roiY+roiHeight)>sourceMat.rows)
        return(sourceMat(cv::Rect(roiX,roiY,sourceMat.cols-roiX,sourceMat.rows-roiY)).clone());
    return sourceMat(cv::Rect(roiX,roiY,roiWidth,roiHeight)).clone();
}

void MainWindow::recognizeCode()
{
    for(uint i=0;i<validCodeMats.size();++i)
    {
        QString str="";
        int matchScore=-1;
        if(extractTool.extractStr(validCodeMats[i],str,matchScore)){
            codes.push_back(str);
            matchScores.push_back(matchScore);
        }
    }
}

QString MainWindow::readResultFromTxt(QString txtPath)
{
    QString result="";
    QFile file(txtPath);
    if(!file.open(QFile::ReadOnly|QIODevice::Text)){
        //QMessageBox::warning(NULL,"Warning!","failed to open result txt!");
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

void MainWindow::outputProcessResult()
{
    ui->label_4->setVisible(false);

    ui->textEdit->setTextColor(Qt::blue);
    QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->textEdit->append(time);

    ui->textEdit->setTextColor(Qt::red);
    switch(processResultFlag)
    {
    case INVALID_INPUT_IMG:{
        ui->textEdit->append("Result:INVALID_INPUT_IMG");
    }break;
    case ANGLE_UNCERTAIN:{
        ui->textEdit->append("Result:ANGLE_UNCERTAIN");
    }break;
    case ROI_UNCERTAIN:{
        ui->textEdit->append("Result:ROI_UNCERTAIN");
    }break;
    case NOMATCH_REGION_DETECTED:{
        ui->textEdit->append("Result:NOMATCH_REGION_DETECTED");
    }break;
    default:{
        ui->textEdit->setTextColor(Qt::green);
        showValidCodeMat();
        ui->textEdit->append("PCB ANGLE:"+QString::number(pcbAngle,10));
        ui->textEdit->append("Valid Code Region Num:"+QString::number(validCodeMats.size(),10));
        ui->textEdit->append("Used Time:"+QString::number(processTime,10)+"ms");
        for(uint i=0;i<codes.size();++i)
        {
            ui->textEdit->append("Recognize code:"+codes[i].remove("\n"));
            ui->textEdit->append("Match Score:"+QString::number(matchScores[i],10));
        }
        ui->label_4->setVisible(true);
    };
    }

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
    ui->textEdit->append("\n");
}

cv::Mat MainWindow::drawResultMat()
{
    cv::Mat roiMat=getRoiMat(roiX,roiY,roiWidth,roiHeight);
    cv::Mat resultMat=sourceMat.clone();
    if(resultMat.channels()==1)
        cv::cvtColor(resultMat,resultMat,cv::COLOR_GRAY2RGB);
    if(roiMat.channels()==1)
        cv::cvtColor(roiMat,roiMat,cv::COLOR_GRAY2RGB);

    if(processResultFlag!=ROI_UNCERTAIN)
        cv::line(roiMat,borderPoint1,borderPoint2, cv::Scalar(0,0,255),3);

    //return resultMat;
    showTool.drawROI(resultMat,cv::Rect(roiX,roiY,roiWidth,roiHeight));

    roiMat.copyTo(resultMat(cv::Rect(roiX,roiY,roiMat.cols,roiMat.rows)));
    return resultMat;
}

void MainWindow::showResultMat()
{
    if(sourceMat.empty())
        return;
    cv::Mat resultMat=drawResultMat();
    showTool.showMatOnDlg(resultMat,ui->label_3);
}

void MainWindow::showValidCodeMat()
{
    ui->comboBox->clear();
    for(uint i=0;i<validCodeMats.size();++i){
        ui->comboBox->insertItem(i,"result "+QString::number(i,10));
    }
    showTool.showMatOnDlg(validCodeMats[0],ui->label_4);
}

bool MainWindow::openCamera()
{
    bool cameraStatus=cameraTool.openCamera();
    if(cameraStatus)
    {
        ui->textEdit->setTextColor(Qt::green);
        ui->textEdit->append("open camera sucess!");
        if(cameraTool.setExpose(setDlg->getExposeTime()))
        {
            ui->textEdit->setTextColor(Qt::green);
            ui->textEdit->append("set camera expose time sucess!");
        }
        else{
            ui->textEdit->setTextColor(Qt::red);
            ui->textEdit->append("set camera expose fail!");
        }
        if(cameraTool.setAnalogGain(setDlg->getAnalogGain()))
        {
            ui->textEdit->setTextColor(Qt::green);
            ui->textEdit->append("set camera expose time sucess!");
        }
        else{
            ui->textEdit->setTextColor(Qt::red);
            ui->textEdit->append("set camera expose fail!");
        }
    }
    else
    {
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->append("open camera fail!");
        ui->label_3->setText("相机掉线!");
    }
    return cameraStatus;
}

bool MainWindow::iniTesseract()
{
    bool tesseractStatus=extractTool.iniTesseractOCR();
    if(tesseractStatus){
        ui->textEdit->setTextColor(Qt::green);
        ui->textEdit->append("initialize tesseract sucess!");
    }else
    {
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->append("initialize tesseract error!");
    }
    return tesseractStatus;
}

void MainWindow::processImg()
{
    cv::Mat roiMat=getRoiMat(roiX,roiY,roiWidth,roiHeight);
    processResultFlag=processTool.extractValidCodeRegion(roiMat,validCodeMats,pcbAngle,borderPoint1,borderPoint2);
}

void MainWindow::extractCodeFromImg()
{
    QTime timer;
    timer.start();
    codes.clear();
    matchScores.clear();
    ui->lineEdit->clear();

    if(sourceMat.empty())
    {
        QMessageBox::warning(NULL,"Warning","Empty Img!");
        return;
    }

    ///////////ini process data////////////////////////////////////
    processDataIni();

    ///////////extract the valid code region////////////////////////
    processImg();

    ///////////recognize the code in img////////////////////////////
    recognizeCode();

    processTime=timer.elapsed();

    ///////////output process result////////////////////////////////
    outputProcessResult();
}

void MainWindow::slotStartGrab(QLabel * label)
{

}

void MainWindow::slotStopGrab()
{

}

void MainWindow::slotCameraParamChanged(uint exposeTime, uint analogGain)
{
    cameraTool.setAnalogGain(analogGain);
    cameraTool.setExpose(exposeTime);
}

void MainWindow::on_pushButton_4_clicked()
{
    sourceMat=cameraTool.grabImg().clone();
    showTool.showMatOnDlg(sourceMat,ui->label_3);
    extractCodeFromImg();
    showResultMat();
}

void MainWindow::processDataIni()
{
    validCodeMats.clear();
    processResultFlag=0;
    pcbAngle=361;
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if(index!=-1)
        showTool.showMatOnDlg(validCodeMats[index],ui->label_4);
}

void MainWindow::on_pushButton_5_clicked()
{
    if(!sourceMat.empty())
        cv::imwrite(QString(QDateTime::currentDateTime().toString("hh.mm.ss")+".bmp").toStdString(),sourceMat);
}
