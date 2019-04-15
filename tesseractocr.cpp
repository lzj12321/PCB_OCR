#include "tesseractocr.h"
#include<QDebug>
TesseractOCR::TesseractOCR()
{

}

bool TesseractOCR::extractStr(const cv::Mat &mat,QString&recognizeStr,int& matchScore)
{
    if(tesseractStatus){
        const unsigned char* dataPtr=mat.data;
        tesseractApi->SetImage(dataPtr,mat.cols,mat.rows,mat.channels(),mat.cols*mat.channels());
        char* str=tesseractApi->GetUTF8Text();
        int* confidences=tesseractApi->AllWordConfidences();
        matchScore=tesseractApi->MeanTextConf();
        for(int i=0;confidences[i]!=-1;++i){
            qDebug()<<"confidence:"<<confidences[i];
        }
        qDebug()<<"mean confidence:"<<matchScore;
        delete[] confidences;
        recognizeStr=QString(str);
        tesseractApi->Clear();
        return true;
    }else
        return false;
}

bool TesseractOCR::iniTesseractOCR()
{
    tesseractApi = new tesseract::TessBaseAPI();
    char* trainDataPath="/usr/share/tesseract-ocr";
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (tesseractApi->Init(trainDataPath, "num2")){
        qDebug()<<"Could not initialize tesseract.";
        tesseractStatus=false;
        return false;
    }else
    {
        tesseractStatus=true;
        return true;
    }
}

bool TesseractOCR::unIniTesseractOCR()
{
    return true;
}
