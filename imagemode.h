#ifndef IMAGEMODE_H
#define IMAGEMODE_H
#include "QImage"
#include "QFileDialog"
#include "QString"
class ImageMode
{
public:
    ImageMode();
    void OpenImage();
    void SaveImage();
    //oriImage原始照片，eftImage处理照片，tImage指向原始照片的数据，每次需要处理的时候先取到tImage的数据再赋值给eftImage
    QImage oriImage;
    QImage eftImage;
    QImage tImage;
    QString openfilename;
    QString savefilename;
    void retImage();
    void seteftImage();
private:

};

#endif // IMAGEMODE_H
