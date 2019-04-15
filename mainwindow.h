#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<settingsdlg.h>
#include<cvandui.h>
#include<preprocessimg.h>
#include<mvcamera.h>
#include<tesseractocr.h>

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
    void on_pushButton_2_clicked();
    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    cvAndUi showTool;
    preProcessImg processTool;
    MVCamera cameraTool;
    TesseractOCR extractTool;
    cv::Mat sourceMat;
    settingsDlg* setDlg;
    uint roiX,roiY,roiWidth,roiHeight;

    std::vector<cv::Mat> validCodeMats;
    std::vector<QString> codes;
    std::vector<int>matchScores;
    int pcbAngle;
    int processResultFlag;
    uint processTime=0;
    cv::Point borderPoint1,borderPoint2;
private:
    void processDataIni();
    cv::Mat getRoiMat(uint& roiX,uint& roiY,uint& roiWidth,uint& roiHeight);
    void recognizeCode();
    QString readResultFromTxt(QString txtPath);
    bool confirmResultIsValid(QString resultCode);
    void outputProcessResult();
    cv::Mat drawResultMat();
    void showResultMat();
    void showValidCodeMat();
    bool openCamera();
    bool iniTesseract();
    void processImg();
    void extractCodeFromImg();
private slots:
    void slotStartGrab(QLabel*);
    void slotStopGrab();
    void slotCameraParamChanged(uint,uint);
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_5_clicked();
};

#endif // MAINWINDOW_H
