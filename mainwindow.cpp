#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include"QImage"
#include"QPixmap"
#include"QMessageBox"
//#include"imagemode.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->imageMode=new ImageMode;
    this->imageAlg=new ImageAlg;
    pOriChart=new QCustomPlot(ui->Orihistogram);
    pOriChart->resize(ui->Orihistogram->width(),ui->Orihistogram->height());
    pOriChart->xAxis->setLabel(tr("Grayscale"));
    pOriChart->yAxis->setLabel(tr("Number"));
    pOriChart->addGraph();
    pOriChart->addGraph();
    pOriChart->addGraph();
    pOriChart->addGraph();
    QCPTextElement *titleOri = new QCPTextElement(pOriChart,tr("Grayscale histogram"));
    pOriChart->plotLayout()->insertRow(0);
    pOriChart->plotLayout()->addElement(0,0,titleOri);
    pOriChart->xAxis->setRange(0,255);
    pOriChart->graph(0)->setLineStyle(QCPGraph::lsImpulse);
    pEftChart=new QCustomPlot(ui->Efthistogram);
    pEftChart->resize(ui->Efthistogram->width(),ui->Efthistogram->height());
    pEftChart->xAxis->setLabel(tr("Grayscale"));
    pEftChart->yAxis->setLabel(tr("Number"));
    pEftChart->addGraph();
    pEftChart->addGraph();
    pEftChart->addGraph();
    pEftChart->addGraph();
    QCPTextElement *titleEft = new QCPTextElement(pEftChart,tr("Grayscale histogram"));
    pEftChart->plotLayout()->insertRow(0);
    pEftChart->plotLayout()->addElement(0,0,titleEft);
    pEftChart->xAxis->setRange(0,255);
    pEftChart->graph(0)->setLineStyle(QCPGraph::lsImpulse);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_actionOpen_Image_triggered()
{
    //读入图片，设置路径，然后指向当前图片，显示原图片
    //读入图片和显示图片最好用Qpixmap，中间过程用QImage和Qpixmap互转即可
    openfilename=QFileDialog::getOpenFileName(this,tr("Open File"),"G:/QTCode/Image",tr("Images (*.png *.xpm *.jpg *.jpeg *.bmp)"));
    qDebug()<<openfilename<<endl;
    if(openfilename=="")return;
    if(!pImage.load(openfilename))
    {
        QMessageBox::information(this,"error","open faild");
    }

    imageMode->oriImage=pImage.toImage();
    imageMode->eftImage=pImage.toImage();
    //    imageMode->oriImage.load(openfilename);
    //    imageMode->eftImage.load(openfilename);
    //    imageMode->oriImage=imageMode->eftImage;
    //    imageMode->OpenImage();
    //    int pwidth=ui->LBoriImage->width();
    //    int pheight=ui->LBoriImage->height();
    //    qDebug()<<pwidth<<" "<<pheight;
    ui->LBoriImage->setPixmap(pImage);
    ui->LBoriImage->setScaledContents(true);
    ui->LBeftImage->setPixmap(pImage);
    ui->LBeftImage->setScaledContents(true);
    SetHistogram(this->pOriChart,imageMode->oriImage);
    //    QPixmap pShowImage= pImage->scaled(pwidth,pheight,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
}

void MainWindow::SetHistogram(QCustomPlot *n_Chart, QImage Image)
{
    QImage GaryImage=Image.convertToFormat(QImage::Format_Grayscale8);
    int nWidth = n_Chart->width();
    int nHeight = n_Chart->height();
    //X为横坐标
    QVector<double> vecX;
    QVector<double> GrayVecY(256,0);   //init Y data with 0;
    QVector<double> RedVecY(256,0);   //init Y data with 0;
    QVector<double> GreenVecY(256,0);   //init Y data with 0;
    QVector<double> BlueVecY(256,0);   //init Y data with 0;
    bool AllGray=true;
    int i = 0;
    while (256 != i) {
        vecX.append(i);
        ++i;
    }
    int GrayVal=0;
    int RedVal=0;
    int GreenVal=0;
    int BlueVal=0;
    QColor tColor;
    //统计最高坐标
    double yMax = 3000;
    n_Chart->yAxis->setRange(0,yMax);
    //处理RGB和Gray
    //取到图片每一个像素点的值 累加到vecY中
    for(int j = 0; j < nHeight; j++)
    {
        for(int k = 0; k < nWidth; k ++)
        {
            GrayVal = int(GaryImage.bits()[j*nHeight + k]);
            tColor=QColor(Image.pixel(k,j));
            RedVal=tColor.red();
            GreenVal=tColor.green();
            BlueVal=tColor.blue();
            GrayVecY[GrayVal] +=  1;
            RedVecY[RedVal] +=  1;
            GreenVecY[GreenVal] +=  1;
            BlueVecY[BlueVal] +=  1;
        }
    }



    for(int i=0;i<256;i++)
    {
        if(RedVecY[i]!=GreenVecY[i]||GreenVecY[i]!=BlueVecY[i]||RedVecY[i]!=BlueVecY[i])
        {
            AllGray=false;
            break;
        }
    }
    if(AllGray)
    {
        qDebug()<<"AllGray";
        n_Chart->graph(0)->setData(vecX,GrayVecY);
        n_Chart->graph(0)->setPen(QPen(Qt::gray));
        n_Chart->graph(1)->data().data()->clear();
        n_Chart->graph(2)->data().data()->clear();
        n_Chart->graph(3)->data().data()->clear();
//         RedVecY.clear();
//         GreenVecY.clear();
//         BlueVecY.clear();
    }
    else
    {
        n_Chart->graph(0)->setData(vecX,GrayVecY);
        n_Chart->graph(0)->setPen(QPen(Qt::gray));
        n_Chart->graph(1)->setData(vecX,RedVecY);
        n_Chart->graph(1)->setPen(QPen(Qt::red));
        n_Chart->graph(2)->setData(vecX,GreenVecY);
        n_Chart->graph(2)->setPen(QPen(Qt::green));
        n_Chart->graph(3)->setData(vecX,BlueVecY);
        n_Chart->graph(3)->setPen(QPen(Qt::blue));
    }

    n_Chart->replot();

}
void MainWindow::on_actionSave_Image_triggered()
{
    //保存效果图片,如果还没有操作提示并且不保存
    //    if()
    //    {
    //        return;
    //    }
    savefilename=QFileDialog::getSaveFileName(this,tr("Save File"),"G:/QTCode/Image",tr("Images (*.png *.xpm *.jpg *.jpeg *.bmp)"));
    if(savefilename=="")return;

    //    QScreen *screen = QGuiApplication::primaryScreen();
    //    screen->grabWindow(ui->LBeftImage->winId()).save(savefilename);
    if(!imageMode->eftImage.save(savefilename,"JPG",100))
    {
        QMessageBox::information(this,"error","save faild");
    }
}

void MainWindow::on_actionGray_triggered()
{
    imageMode->retImage();
    imageAlg->Image2Gray(imageMode->tImage);
    imageMode->seteftImage();
    pImage=QPixmap::fromImage(imageMode->eftImage);
    ui->LBeftImage->setPixmap(pImage);
    SetHistogram(this->pEftChart,imageMode->eftImage);
}

void MainWindow::on_actionEqual_triggered()
{
    imageMode->retImage();
    imageAlg->ImageEqual(imageMode->tImage);
    imageMode->seteftImage();
    pImage=QPixmap::fromImage(imageMode->eftImage);
    ui->LBeftImage->setPixmap(pImage);
    //    SetHistogram(this->pEftChart,imageMode->eftImage);
    SetHistogram(this->pEftChart,imageMode->eftImage);
}

void MainWindow::on_actionSmooth_triggered()
{
    imageMode->retImage();
    imageAlg->ImageSmooth(imageMode->tImage);
    imageMode->seteftImage();
    pImage=QPixmap::fromImage(imageMode->eftImage);
    ui->LBeftImage->setPixmap(pImage);
    SetHistogram(this->pEftChart,imageMode->eftImage);
}

void MainWindow::on_actionSharpen_triggered()
{
    imageMode->retImage();
    imageAlg->ImageSharpen(imageMode->tImage);
    imageMode->seteftImage();
    pImage=QPixmap::fromImage(imageMode->eftImage);
    ui->LBeftImage->setPixmap(pImage);
    SetHistogram(this->pEftChart,imageMode->eftImage);
}

void MainWindow::on_actionEdge_Detection_triggered()
{
    imageMode->retImage();
    imageAlg->ImageEdgeDetection(imageMode->tImage);
    imageMode->seteftImage();
    pImage=QPixmap::fromImage(imageMode->eftImage);
    ui->LBeftImage->setPixmap(pImage);
    SetHistogram(this->pEftChart,imageMode->eftImage);
}

void MainWindow::on_actionGaussian_Blur_triggered()
{
    imageMode->retImage();
    imageAlg->ImageGaussianBlur(imageMode->tImage);
    imageMode->seteftImage();
    pImage=QPixmap::fromImage(imageMode->eftImage);
    ui->LBeftImage->setPixmap(pImage);
    SetHistogram(this->pEftChart,imageMode->eftImage);
}

void MainWindow::on_actionmedian_filter_triggered()
{
    imageMode->retImage();
    imageAlg->ImageMedianFilter(imageMode->tImage);
    imageMode->seteftImage();
    pImage=QPixmap::fromImage(imageMode->eftImage);
    ui->LBeftImage->setPixmap(pImage);
    SetHistogram(this->pEftChart,imageMode->eftImage);
}

void MainWindow::on_actionSalt_triggered()
{
    imageMode->retImage();
    imageAlg->ImageSalt(imageMode->tImage);
    imageMode->seteftImage();
    pImage=QPixmap::fromImage(imageMode->eftImage);
    ui->LBeftImage->setPixmap(pImage);
    SetHistogram(this->pEftChart,imageMode->eftImage);
}

void MainWindow::on_actionChange_Image_triggered()
{
    //交换图片 重新绘图
    imageMode->changeImage();
    pImage=QPixmap::fromImage(imageMode->eftImage);
    QPixmap tImage=QPixmap::fromImage(imageMode->oriImage);
    ui->LBoriImage->setPixmap(tImage);
    ui->LBeftImage->setPixmap(pImage);
    SetHistogram(this->pOriChart,imageMode->oriImage);
    SetHistogram(this->pEftChart,imageMode->eftImage);
}
