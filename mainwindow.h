#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<settingsdlg.h>
#include<cvandui.h>
#include<preprocessimg.h>
#include<mvcamera.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void iconPreProcess();
private slots:
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void roiChanged(uint _roiX,uint _roiY,uint _roiWidth,uint _roiHeight);
    void on_pushButton_2_clicked();   
    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    cvAndUi showTool;
    preProcessImg processTool;
    MVCamera cameraTool;
    cv::Mat sourceMat;
    settingsDlg* setDlg;
    uint roiX,roiY,roiWidth,roiHeight;

    std::vector<QString> codes;
    uint processTime=0;
private:
    cv::Mat getRoiMat(uint& roiX,uint& roiY,uint& roiWidth,uint& roiHeight);
    void recognizeCode(int);
    QString readResultFromTxt(QString txtPath);
    bool confirmResultIsValid(QString resultCode);
    void outputProcessResult(int);
    cv::Mat drawResultMat();
    void showResultMat();
    bool openCamera();
    void processImg();

private slots:
    void slotStartGrab(QLabel*);
    void slotStopGrab();
    void slotCameraParamChanged(uint,uint);
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_5_clicked();
};

#endif // MAINWINDOW_H
