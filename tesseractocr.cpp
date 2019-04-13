#include "tesseractocr.h"
#include<QDebug>
TesseractOCR::TesseractOCR()
{

}

QString TesseractOCR::extractStr(const cv::Mat &mat)
{
    const unsigned char* dataPtr=mat.data;
    tesseractApi->SetImage(dataPtr,mat.cols,mat.rows,mat.channels(),mat.cols*mat.channels());
    char* str=tesseractApi->GetUTF8Text();
    QString result=QString(str);
    return result;
}

bool TesseractOCR::iniTesseractOCR()
{
    tesseractApi = new tesseract::TessBaseAPI();
    char* trainDataPath="/usr/share/tesseract-ocr";
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (tesseractApi->Init(trainDataPath, "num2")){
        qDebug()<<"Could not initialize tesseract.";
        return false;
    }else
        return true;
}

bool TesseractOCR::unIniTesseractOCR()
{
    return true;
}
