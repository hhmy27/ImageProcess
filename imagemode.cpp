#include "imagemode.h"
#include "QDebug"
#include "QImage"
#include "QString"
//#include "QFileDialog"
ImageMode::ImageMode()
{

}

void ImageMode::OpenImage()
{
//    openfilename=QFileDialog::getOpenFileName(this,QObject::tr("Open File"),"",QObject::tr("Images (*.png *.xpm *.jpg)"));
//    oriImage.load(openfilename);
//    eftImage.load(openfilename);
}
void ImageMode::SaveImage()
{

}
void ImageMode::retImage()
{
    tImage=oriImage;
}
void ImageMode::seteftImage()
{
    eftImage=tImage;
}

