#ifndef IMAGEALG_H
#define IMAGEALG_H
#include "QImage"

class ImageAlg
{
public:
    ImageAlg();
    void Image2Gray(QImage& Image);
    void ImageSmooth(QImage& Image);
    void ImageSharpen(QImage& Image);
    void ImageEqual(QImage& Image);
    void ImageEdgeDetection(QImage& Image);
    void ImageGaussianBlur(QImage& Image);
    void ImageMedianFilter(QImage& Image);
    int judgeEdge(int x,int i,int width);
//    void Image
};

#endif // IMAGEALG_H
