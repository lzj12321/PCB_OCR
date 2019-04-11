#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QDialog>
#include<cvandui.h>
#include<QMouseEvent>
#include<QEvent>

#define roiColour cv::Scalar(255,200,50)

namespace Ui {
class settingsDlg;
}

class settingsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit settingsDlg(QWidget *parent = 0);
    ~settingsDlg();
    void getRoiParam(uint& roiX,uint& roiY,uint& roiWidth,uint& roiHeight);
private:
    cv::Mat sourceMat;

    uint roiX=0;
    uint roiY=0;
    uint roiWidth=0;
    uint roiHeight=0;
    bool pRoiStatus=false;
    bool isRoiChanged=false;
    double ratio=0;

    int offsetX=0;
    int offsetY=0;
private:
    int pt1X=0;
    int pt1Y=0;
    int pt2X=0;
    int pt2Y=0;
private:
    Ui::settingsDlg *ui;
    cvAndUi showTool;
    void settingsDlgIni();
    void drawRoiOnMat(cv::Mat &src);
    void restorePaintingStatus();
    void paramSave();
    void paramLoad();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_4_clicked();

signals:
    void roiChanged(uint roiX,uint roiY,uint roiWidth,uint roiHeight);
};

#endif // SETTINGSDLG_H
