#include "imagealg.h"
#include"QDebug"
#include"QVector"
#include <QtAlgorithms>
ImageAlg::ImageAlg()
{

}
//x中心位置，i偏移量
//为了消除黑边 参考文章 https://blog.csdn.net/Mahabharata_/article/details/69066942
int ImageAlg::judgeEdge(int x, int i, int width)
{
    //nx现在矩阵中的位置
    int nx=x+i;
    //修正位置
    if(nx>=width||nx<0)nx-=i;
    return nx;
}
void ImageAlg::Image2Gray(QImage& Image)
{
    qDebug()<<"ok";
    for(int y=0;y<Image.height();y++)
    {
        QRgb *pixel=(QRgb*)Image.scanLine(y);
        for(int x=0;x<Image.width();x++)
        {
            //            qRed(pixel[x])*0.3+qGreen(pixel[x])*0.59+qBlue(pixel[x])*0.11
            int r=qRed(pixel[x]);
            int g=qGreen(pixel[x]);
            int b=qBlue(pixel[x]);
            int avg=(r+g+b)/3;
            //平均法取灰度
            Image.setPixel(x,y,qRgb(avg,avg,avg));
            //            加权法取灰度
            //            Image.setPixel(x,y,qRgb(qGray(r,g,b),qGray(r,g,b),qGray(r,g,b)));
        }
    }
}

void ImageAlg::ImageEqual(QImage &Image)
{
    qDebug()<<"ok\n";
    //无奈自己不会操作QBitmap取到其中的GrayData，只好对Image中的图都进行统计然后再转成灰度图
    int width,height;
    width=Image.width();
    height=Image.height();
    QRgb *pixel;

    //读入数据，并直接转换成概率形式
    float rlist[256],glist[256],blist[256];
    int rdict[256],gdict[256],bdict[256];//均衡化后对应像素的值
    memset(rlist,0,sizeof(rlist));
    memset(glist,0,sizeof(glist));
    memset(blist,0,sizeof(blist));

    //获取原图各个灰度的数量
    for(int y=0;y<height;y++)
    {
        //一次读取一行数据
        pixel=(QRgb*)Image.scanLine(y);
        for(int x=0;x<width;x++)
        {
            rlist[qRed(pixel[x])]++;
            glist[qGreen(pixel[x])]++;
            blist[qBlue(pixel[x])]++;
        }
    }

    rlist[0]/=width*height;
    glist[0]/=width*height;
    blist[0]/=width*height;
    //获取原图各个灰度级的概率
    for(int i=1;i<256;i++)
    {
        rlist[i]/=(width*height);
        rlist[i]+=rlist[i-1];
        glist[i]/=(width*height);
        glist[i]+=glist[i-1];
        blist[i]/=(width*height);
        blist[i]+=blist[i-1];
    }
    //生成直方图查找表
    for(int i=0;i<256;i++)
    {
        rdict[i]=int(rlist[i]*255+0.5);
        gdict[i]=int(glist[i]*255+0.5);
        bdict[i]=int(blist[i]*255+0.5);
    }
    int nr,ng,nb;
    //更新像素点
    for(int y=0;y<height;y++)
    {
        //一次读取一行数据
        pixel=(QRgb*)Image.scanLine(y);
        for(int x=0;x<width;x++)
        {
            //注意这里要取到pixel值时要加qRed获取到int值，不然会越界
            nr=rdict[qRed(pixel[x])];
            ng=gdict[qGreen(pixel[x])];
            nb=bdict[qBlue(pixel[x])];
            Image.setPixel(x,y,qRgb(nr,ng,nb));
        }
    }

}
void ImageAlg::ImageSmooth(QImage& Image)
{
    //---------------------------------------------------------------------------------
    //        //两种方法
    //        double kernel[3][3]={{0.1,0.1,0.1},{0.1,0.2,0.1},{0.1,0.1,0.1}};
    //        int kernelSize=3;
    //        int sumKernel=1;    //(-1)+(-1)+5+(-1)+(-1)
    //        int r,g,b;
    //        QColor color;
    //        QImage Image2=Image;
    //        for(int x=kernelSize/2;x<Image.width()-(kernelSize/2);x++){
    //            for(int y=kernelSize/2;y<Image.height()-(kernelSize/2);y++){
    //                r=0;
    //                g=0;
    //                b=0;
    //                for(int i=-kernelSize/2;i<=kernelSize/2;i++){
    //                    for(int j=-kernelSize/2;j<=kernelSize/2;j++){
    //                        color=QColor(Image.pixel(x+i,y+j));
    //                        r +=color.red()*kernel[kernelSize/2+i][kernelSize/2+j];
    //                        g += color.green()*kernel[kernelSize/2+i][kernelSize/2+j];
    //                        b += color.blue()*kernel[kernelSize/2+i][kernelSize/2+j];
    //                    }
    //                }
    //                //将值域限制到[0,255]
    //                r = qBound(0, r/sumKernel, 255);
    //                g = qBound(0, g/sumKernel, 255);
    //                b = qBound(0, b/sumKernel, 255);
    //                Image2.setPixel(x,y, qRgb(r,g,b));
    //            }
    //        }
    //        Image=Image2;
    //---------------------------------------------------------------------------------


    //---------------------------------------------------------------------------------
    //自己写的算法
    //读入某些图片会提示像素点越界
    double kernel[]={0.1,0.1,0.1,0.1,0.2,0.1,0.1,0.1,0.1};
    int size=1;
    int H=Image.height();
    int W=Image.width();
    //    Image.setPixel(W-1,H-1,qRgb(0,0,0));
    //        int sumKernel=1;
    //        for(int i=0;i<9;i++)sumKernel+=kernel[i];
    QImage tImage=Image;
    int tr,tg,tb;
    QColor tColor;
    int nx=0;
    int ny=0;
    //!!!这里虽然理论上上从0~H-1，0~W-1遍历能完整的处理图像，但实际上如果写for(int y=0;y<H;y++)的话
    //! 会弹出警告，QImage::pixel: coordinate (607,603) out of range，提示最后一行越界，但程序会正常运行
    //! 如果不处理最后一行 则不会弹出警告。
    for(int y=0;y<H-1;y++)
    {
        for(int x=0;x<W-1;x++)
        {
            tr=tg=tb=0;
            //进行卷积 i行 j列
            for(int i=-size;i<=size;i++)
            {
                for(int j=-size;j<=size;j++)
                {
                    nx=judgeEdge(x,i,W);
                    ny=judgeEdge(y,j,W);
                    tColor=QColor(Image.pixel(nx,ny));
                    tr+=tColor.red()*kernel[(i+1)*3+j+1];
                    tg+=tColor.green()*kernel[(i+1)*3+j+1];
                    tb+=tColor.blue()*kernel[(i+1)*3+j+1];
                }
            }
            tr = qBound(0, tr, 255);
            tg = qBound(0, tg, 255);
            tb = qBound(0, tb, 255);
            tImage.setPixel(x,y,qRgb(tr,tg,tb));
        }
    }
    Image=tImage;
    //---------------------------------------------------------------------------------
}

void ImageAlg::ImageSharpen(QImage &Image)
{
    //读入某些图片会提示像素点越界 ！ 不处理最后一行即可
    //---------------------------------------------------------------------------------
    int kernel[]={0,-1,0,-1,5,-1,0,-1,0};
    int size=1;
    int H=Image.height();
    int W=Image.width();
    QImage tImage=Image;
    int tr,tg,tb;
    QColor tColor;
    int nx=0;
    int ny=0;
    for(int y=0;y<H-1;y++)
    {
        for(int x=0;x<W-1;x++)
        {
            tr=tg=tb=0;
            //进行卷积 i行 j列
            for(int i=-size;i<=size;i++)
            {
                for(int j=-size;j<=size;j++)
                {
                    nx=judgeEdge(x,i,W);
                    ny=judgeEdge(y,j,W);
                    tColor=QColor(Image.pixel(nx,ny));
                    tr+=tColor.red()*kernel[(i+1)*3+j+1];
                    tg+=tColor.green()*kernel[(i+1)*3+j+1];
                    tb+=tColor.blue()*kernel[(i+1)*3+j+1];
                }
            }
            tr = qBound(0, tr, 255);
            tg = qBound(0, tg, 255);
            tb = qBound(0, tb, 255);
            tImage.setPixel(x,y,qRgb(tr,tg,tb));
        }
    }
    Image=tImage;
    //---------------------------------------------------------------------------------

    //---------------------------------------------------------------------------------
    //    //滤波器
    //    int kernel[3][3]={{0,-1,0},{-1,5,-1},{0,-1,0}};
    //    int kernelSize=3;
    //    int sumKernel=1;    //(-1)+(-1)+5+(-1)+(-1)
    //    int r,g,b;
    //    QColor color;
    //    QImage Image2=Image;
    //    for(int x=kernelSize/2;x<Image.width()-(kernelSize/2);x++){
    //        for(int y=kernelSize/2;y<Image.height()-(kernelSize/2);y++){
    //            r=0;
    //            g=0;
    //            b=0;
    //            for(int i=-kernelSize/2;i<=kernelSize/2;i++){
    //                for(int j=-kernelSize/2;j<=kernelSize/2;j++){
    //                    color=QColor(Image.pixel(x+i,y+j));
    //                    r +=color.red()*kernel[kernelSize/2+i][kernelSize/2+j];
    //                    g += color.green()*kernel[kernelSize/2+i][kernelSize/2+j];
    //                    b += color.blue()*kernel[kernelSize/2+i][kernelSize/2+j];
    //                }
    //            }
    //            //将值域限制到[0,255]
    //            r = qBound(0, r/sumKernel, 255);
    //            g = qBound(0, g/sumKernel, 255);
    //            b = qBound(0, b/sumKernel, 255);
    //            Image2.setPixel(x,y, qRgb(r,g,b));
    //        }
    //    }
    //    Image=Image2;
    //---------------------------------------------------------------------------------

}

void ImageAlg::ImageEdgeDetection(QImage &Image)
{
    //---------------------------------------------------------------------------------

    //    int kernelx[]={-1,-2,-1,0,0,0,1,2,1};
    //    int kernely[]={-1,0,1,-2,0,2,-1,0,1};
    //    int size=1;
    //    int H=Image.height();
    //    int W=Image.width();
    //    //        int sumKernel=1;
    //    //        for(int i=0;i<9;i++)sumKernel+=kernel[i];
    //    QImage tImage=Image;
    //    int tr,tg,tb;
    //    QColor tColor;
    //    int nx=0;
    //    int ny=0;
    //    for(int y=0;y<H;y++)
    //    {
    //        for(int x=0;x<W;x++)
    //        {
    //            tr=tg=tb=0;
    //            //进行卷积 i行 j列
    //            for(int i=-size;i<=size;i++)
    //            {
    //                for(int j=-size;j<=size;j++)
    //                {
    //                    nx=judgeEdge(x,i,W);
    //                    ny=judgeEdge(y,j,W);
    //                    tColor=QColor(Image.pixel(nx,ny));
    //                    tr+=tColor.red()*kernely[(i+1)*3+j+1];
    //                    tg+=tColor.green()*kernely[(i+1)*3+j+1];
    //                    tb+=tColor.blue()*kernely[(i+1)*3+j+1];
    //                }
    //            }
    //            tr = qBound(0, tr, 255);
    //            tg = qBound(0, tg, 255);
    //            tb = qBound(0, tb, 255);
    //            tImage.setPixel(x,y,qRgb(tr,tg,tb));
    //        }
    //    }
    //    for(int y=0;y<H;y++)
    //    {
    //        for(int x=0;x<W;x++)
    //        {
    //            tr=tg=tb=0;
    //            //进行卷积 i行 j列
    //            for(int i=-size;i<=size;i++)
    //            {
    //                for(int j=-size;j<=size;j++)
    //                {
    //                    nx=judgeEdge(x,i,W);
    //                    ny=judgeEdge(y,j,W);
    //                    tColor=QColor(Image.pixel(nx,ny));
    //                    tr+=tColor.red()*kernelx[(i+1)*3+j+1];
    //                    tg+=tColor.green()*kernelx[(i+1)*3+j+1];
    //                    tb+=tColor.blue()*kernelx[(i+1)*3+j+1];
    //                }
    //            }
    //            tr = qBound(0, tr, 255);
    //            tg = qBound(0, tg, 255);
    //            tb = qBound(0, tb, 255);
    //            tImage.setPixel(x,y,qRgb(tr,tg,tb));
    //        }
    //    }

    //    Image=tImage;
    //---------------------------------------------------------------------------------

    //---------------------------------------------------------------------------------
    int kernel[3][3]={{-1,0,-1},{-2,0,2},{-1,0,1}};
    int kernelx[3][3]={{-1,-2,-1},{0,0,0},{1,2,1}};
    int kernelSize=3;
    int sumKernel=1;    //(-1)+(-1)+5+(-1)+(-1)
    int r,g,b;
    QColor color;
    QImage Image2=Image;
    for(int x=kernelSize/2;x<Image.width()-(kernelSize/2);x++){
        for(int y=kernelSize/2;y<Image.height()-(kernelSize/2);y++){
            r=0;
            g=0;
            b=0;
            for(int i=-kernelSize/2;i<=kernelSize/2;i++){
                for(int j=-kernelSize/2;j<=kernelSize/2;j++){
                    color=QColor(Image.pixel(x+i,y+j));
                    r +=color.red()*kernel[kernelSize/2+i][kernelSize/2+j];
                    g += color.green()*kernel[kernelSize/2+i][kernelSize/2+j];
                    b += color.blue()*kernel[kernelSize/2+i][kernelSize/2+j];
                }
            }
            //将值域限制到[0,255]
            r = qBound(0, r/sumKernel, 255);
            g = qBound(0, g/sumKernel, 255);
            b = qBound(0, b/sumKernel, 255);
            Image2.setPixel(x,y, qRgb(r,g,b));
        }
    }
    for(int x=kernelSize/2;x<Image.width()-(kernelSize/2);x++){
        for(int y=kernelSize/2;y<Image.height()-(kernelSize/2);y++){
            r=0;
            g=0;
            b=0;
            for(int i=-kernelSize/2;i<=kernelSize/2;i++){
                for(int j=-kernelSize/2;j<=kernelSize/2;j++){
                    color=QColor(Image.pixel(x+i,y+j));
                    r +=color.red()*kernelx[kernelSize/2+i][kernelSize/2+j];
                    g += color.green()*kernelx[kernelSize/2+i][kernelSize/2+j];
                    b += color.blue()*kernelx[kernelSize/2+i][kernelSize/2+j];
                }
            }
            //将值域限制到[0,255]
            r = qBound(0, r/sumKernel, 255);
            g = qBound(0, g/sumKernel, 255);
            b = qBound(0, b/sumKernel, 255);
            Image2.setPixel(x,y, qRgb(r,g,b));
        }
    }
    Image=Image2;
    //---------------------------------------------------------------------------------

}
void ImageAlg::ImageGaussianBlur(QImage &Image)
{
    //    double kernel[3][3]={{0.1,0.1,0.1},{0.1,0.2,0.1},{0.1,0.1,0.1}};
    //    int kernelSize=3;
    //    int sumKernel=1;    //(-1)+(-1)+5+(-1)+(-1)
    double kernel[5][5]={
        {1,4,7,4,1},
        {4,16,26,16,4},
        {7,26,41,26,7},
        {4,16,26,16,4},
        {1,4,7,4,1}
    };
    int kernelSize=5;
    int sumKernel=273;    //(-1)+(-1)+5+(-1)+(-1)
    int r,g,b;
    QColor color;
    QImage Image2=Image;
    for(int x=kernelSize/2;x<Image.width()-(kernelSize/2);x++){
        for(int y=kernelSize/2;y<Image.height()-(kernelSize/2);y++){
            r=0;
            g=0;
            b=0;
            for(int i=-kernelSize/2;i<=kernelSize/2;i++){
                for(int j=-kernelSize/2;j<=kernelSize/2;j++){
                    color=QColor(Image.pixel(x+i,y+j));
                    r +=color.red()*kernel[kernelSize/2+i][kernelSize/2+j];
                    g += color.green()*kernel[kernelSize/2+i][kernelSize/2+j];
                    b += color.blue()*kernel[kernelSize/2+i][kernelSize/2+j];
                }
            }
            //将值域限制到[0,255]
            r = qBound(0, r/sumKernel, 255);
            g = qBound(0, g/sumKernel, 255);
            b = qBound(0, b/sumKernel, 255);
            Image2.setPixel(x,y, qRgb(r,g,b));
        }
    }
    Image=Image2;
}
void ImageAlg::ImageMedianFilter(QImage &Image)
{
    //3X3大小空间，排序所有的像素，然后取中间值
    QVector<int>RV;
    QVector<int>GV;
    QVector<int>BV;
    int size=1;
    int H=Image.height();
    int W=Image.width();
    QImage tImage=Image;
    QColor tColor;
    int nx=0;
    int ny=0;
    for(int y=0;y<H;y++)
    {
        for(int x=0;x<W;x++)
        {
            RV.clear();
            GV.clear();
            BV.clear();
            //取到3X3窗口内的像素值
            for(int i=-size;i<=size;i++)
            {
                for(int j=-size;j<=size;j++)
                {
                    nx=judgeEdge(x,i,W);
                    ny=judgeEdge(y,j,W);
                    tColor=QColor(Image.pixel(nx,ny));
                    RV.append(tColor.red());
                    GV.append(tColor.green());
                    BV.append(tColor.blue());
                }
            }
            qSort(RV.begin(),RV.end());
            qSort(GV.begin(),GV.end());
            qSort(BV.begin(),BV.end());
            tImage.setPixel(x,y,qRgb(RV[4],GV[4],BV[4]));
        }
    }
    Image=tImage;

}
