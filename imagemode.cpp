#include "imagemode.h"
#include "QDebug"
#include "QImage"
#include "QString"
//#include "QFileDialog"
ImageMode::ImageMode()
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

void ImageMode::changeImage()
{
    tImage=eftImage;
    eftImage=oriImage;
    oriImage=tImage;
}

