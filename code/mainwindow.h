#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "mylabel.h"
#include "cvmatqimage.h"
#include "singlecircle.h"


namespace Ui {
class MainWindow;
}

enum STATE{
    doNothing, start, retrack, ending, CNTSET_tracking, CNTSET_doNothing, tracking, setOrigin, shoot
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // set focal length
    void setFocalLength();

    void showNothing();

private:
    Ui::MainWindow *ui;
    QImage *qimage;

    // timer
    QTimer *timer;

    // FPS
    QTimer *timer_fps;
    int cnt_fps;

    // camera
    cv::VideoCapture cap;
    int width;
    int height;
    cv::Mat src_img;
    cv::Mat dst_img;

    // circle-tracking algorithm
    SingleCircle control;
    cv::Rect roi;


    // Focal length set
    int a;
    int b;
    int c;

    int cnt;

    STATE state;

private slots:

    void refresh();

    void calcu_fps();

    // focalLength setting
    void on_spinBox_valueChanged(int arg1);
    void on_spinBox_2_valueChanged(int arg1);
    void on_spinBox_3_valueChanged(int arg1);

    void on_btnRESET_clicked();
    void on_btnSTOP_clicked();
    void on_btnSTART_clicked();
    void on_btnCNTSET_clicked();
    void on_btnZERO_clicked();
    void on_spinBox_4_valueChanged(int arg1);
    void on_spinBox_5_valueChanged(int arg1);

    void on_shoot_clicked();
};

#endif // MAINWINDOW_H
