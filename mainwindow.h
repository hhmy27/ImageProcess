#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<imagemode.h>
#include"imagealg.h"
#include "qcustomplot.h"
#include<opencv2/opencv.hpp>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    ImageMode* imageMode;
    ImageAlg* imageAlg;
    //用于显示图片
    QPixmap pImage;
    QString openfilename;
    QString savefilename;
    QCustomPlot* pOriChart;
    QCustomPlot* pEftChart;
    void SetHistogram(QCustomPlot* n_Chart,QImage Image);
private slots:

    void on_actionOpen_Image_triggered();

    void on_actionSave_Image_triggered();

    void on_actionGray_triggered();

    void on_actionEqual_triggered();

    void on_actionSmooth_triggered();

    void on_actionSharpen_triggered();

    void on_actionEdge_Detection_triggered();

    void on_actionGaussian_Blur_triggered();

    void on_actionmedian_filter_triggered();

    void on_actionSalt_triggered();

    void on_actionChange_Image_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
