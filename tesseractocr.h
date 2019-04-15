#ifndef TESSERACTOCR_H
#define TESSERACTOCR_H
#include<cvandui.h>
//#include<tesseractocr.h>
#include<tesseract/baseapi.h>
#include<leptonica/allheaders.h>

class TesseractOCR
{
public:
    TesseractOCR();
    bool extractStr(const cv::Mat&mat,QString&recognizeStr,int& matchScore);
    bool iniTesseractOCR();
    bool unIniTesseractOCR();
private:
    tesseract::TessBaseAPI *tesseractApi;
    bool tesseractStatus=false;
};

#endif // TESSERACTOCR_H
