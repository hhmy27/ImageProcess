#ifndef IMAGEMODE_H
#define IMAGEMODE_H
#include "QImage"
#include "QFileDialog"
#include "QString"
class ImageMode
{
public:
    ImageMode();

    //oriImage原始照片，eftImage处理照片，tImage指向原始照片的数据，每次需要处理的时候先取到tImage的数据再赋值给eftImage
    QImage oriImage;
    QImage eftImage;
    QImage tImage;
    QString openfilename;
    QString savefilename;
    void retImage();
    void seteftImage();
    void changeImage();
private:

};

#endif // IMAGEMODE_H
