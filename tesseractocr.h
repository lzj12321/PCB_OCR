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
    QString extractStr(const cv::Mat&mat);
    bool iniTesseractOCR();
    bool unIniTesseractOCR();
private:
    tesseract::TessBaseAPI *tesseractApi;
};

#endif // TESSERACTOCR_H
