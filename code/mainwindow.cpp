#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // initialize camera
   // width = 2592;    height = 1944;
   width = 1024; height = 768;
    // width = 2048; height = 1536;
    qimage = new QImage(640, 480, QImage::Format_RGB888);
    cap = cv::VideoCapture(cv::CAP_V4L2);
    cap.set(3, width);  cap.set(4, height);
    QString res = "Resolotion: " + QString::number(cap.get(3)) + " x " + QString::number(cap.get(4));
    ui->lblResolution->setText(res);
    qDebug() << "Resolution: " << cap.get(3) << "*" << cap.get(4);

    // refresh
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(refresh()));
    timer->start(33);

    // calculate FPS
    cnt_fps = 0;
    timer_fps = new QTimer(this);
    connect(timer_fps, SIGNAL(timeout()), this, SLOT(calcu_fps()));
    timer_fps->start(1000);

    state = doNothing;

    ui->spinBox->setMinimum(0);
    ui->spinBox_2->setMinimum(0);
    ui->spinBox_3->setMinimum(0);
    ui->spinBox->setMaximum(9);
    ui->spinBox_2->setMaximum(9);
    ui->spinBox_3->setMaximum(9);
    a = 0;
    b = 3;
    c = 5;
    cnt = 30;
    ui->spinBox->setValue(a);
    ui->spinBox_2->setValue(b);
    ui->spinBox_3->setValue(c);
    setFocalLength();

    ui->spinBox_4->setValue(cnt/10);
    ui->spinBox_4->setMaximum(9);
    ui->spinBox_4->setMinimum(0);

    ui->spinBox_5->setValue(cnt%10);
    ui->spinBox_5->setMaximum(9);
    ui->spinBox_5->setMinimum(0);

//    ui->lblTitle->setPixmap(QPixmap("/home/title.png"));
    //ui->lblX->setPixmap(QPixmap(":/new/btnPic/home/X.png"));
//    ui->lblY->setPixmap(QPixmap("/home/Y.png"));
//    ui->lblZ->setPixmap(QPixmap("/home/Z.png"));
//    ui->lblAlpha->setPixmap(QPixmap("/home/alpha.png"));
//    ui->lblBeta->setPixmap(QPixmap("/home/beta.png"));
//    ui->lblGama->setPixmap(QPixmap("/home/gama.png"));
  //  ui->lblX->setPixmap(QPixmap(":/new/btnPic/X.png"));

     QIcon iconStart(":/new/btnPic/start.png");
     ui->btnSTART->setIcon(iconStart);
     ui->btnSTART->setIconSize(ui->btnSTART->size());

     QIcon iconEnd(":/new/btnPic/end.png");
     ui->btnSTOP->setIcon(iconEnd);
     ui->btnSTOP->setIconSize(ui->btnSTOP->size());

     QIcon iconReset(":/new/btnPic/reset.png");
     ui->btnRESET->setIcon(iconReset);
     ui->btnRESET->setIconSize(ui->btnSTOP->size());

     QIcon iconFrame(":/new/btnPic/frame.png");
     ui->btnCNTSET->setIcon(iconFrame);
     ui->btnCNTSET->setIconSize(ui->btnCNTSET->size());

     QIcon iconZero(":/new/btnPic/zero.png");
     ui->btnZERO->setIcon(iconZero);
     ui->btnZERO->setIconSize(ui->btnZERO->size());

}

MainWindow::~MainWindow()
{
    delete ui;

    timer->stop();
    timer_fps->stop();
    cap.release();
}

void MainWindow::setFocalLength()
{
    control.para.focal_length = a*100.0 + b*10.0 + c*1.0;
}

void MainWindow::showNothing()
{
    ui->showAX->setText("");
    ui->showAY->setText("");
    ui->showAZ->setText("");
    ui->showDX->setText("");
    ui->showDY->setText("");
    ui->showDZ->setText("");
}

void MainWindow::refresh()
{
    if(cap.isOpened())
    {
        // 1. get frame
        cap.read(src_img);


        // 2. process
        if(!src_img.empty())
        {
            // --- modify
            // cv::equalizeHist(src_img,src_img);

            switch (state) {

            case doNothing:
            {
                showNothing();
            }
                break;

            case shoot:
            {
                imwrite("shoot.jpg", src_img);
                state == doNothing;
            }
                break;


            case start:
            {
                roi = Rect(Point(ui->lblImg->xpress, ui->lblImg->ypress),
                           Point(ui->lblImg->xrelease, ui->lblImg->yrelease));

                if(control.start(src_img, roi))
                    state = tracking;
                else
                    state = doNothing;
            }
                break;


            case tracking:
            {
                //qDebug() << "tracking";
                if(control.update(src_img)) {
                    //qDebug()<<"updated";
                    ui->showDX->setText(QString::number(control.x()));
                    ui->showDY->setText(QString::number(control.y()));
                    ui->showDZ->setText(QString::number(control.z()));

                    // tmp
                    QString str = QString::number(control.theFit.center.x) + ","
                            +QString::number(control.theFit.center.y);
                    ui->centerCord->setText(str);

//                    QString str2 = "u:"+QString::number(control.calc_z(control.up_fit))+" "
//                            +"d:"+QString::number(control.calc_z(control.down_fit))+" "
//                            +"l:"+QString::number(control.calc_z(control.left_fit))+" "
//                            +"r:"+QString::number(control.calc_z(control.right_fit));
                    double hi = control.theFit.radius*2.0;
                    QString str2 = QString::number(hi);
                    ui->lbldepth->setText(str2);

                    // TODO:
                    bool lr = false;
                    bool ud = false;
                    control.updateOthers(src_img,lr,ud);

                    if(ud) {
                        ui->showAX->setText(QString::number(control.ax()));
                    }
                    if(lr) {
                        ui->showAY->setText(QString::number(control.ay()));
                        ui->showAZ->setText(QString::number(control.az()));
                    }
                }
            }
                break;


            case retrack:
            {
                roi = Rect(Point(ui->lblImg->xpress,
                                 ui->lblImg->ypress),
                           Point(ui->lblImg->xrelease,
                                 ui->lblImg->yrelease));

                if(control.retrack(src_img, roi))
                {
                    state = tracking;
                }
                else
                    state = doNothing;
            }
                break;


            case ending:
            {
                control.ending();
                state = doNothing;
            }
                break;


            case CNTSET_tracking:
            {
                control.setFrameCnt(cnt);
                state = tracking;
            }
                break;

            case CNTSET_doNothing:
            {
                control.setFrameCnt(cnt);
                state = doNothing;
            }
                break;

            case setOrigin:
            {
                control.setOrigin();
                state = tracking;
            }
                break;

            default:
                break;
            }

            dst_img = src_img;
        }

        // 3. show
        cv::resize(dst_img, dst_img, cv::Size(1024, 768));
        *qimage = cvMat2QImage(dst_img); // Mat->QImage
        ui->lblImg->setPixmap(QPixmap::fromImage(*qimage,Qt::AutoColor));

        cnt_fps += 1;
    }
}

void MainWindow::calcu_fps()
{
    ui->lblFPS->setText(QString::number(cnt_fps));
    cnt_fps = 0;
}


void MainWindow::on_spinBox_valueChanged(int arg1)
{
    if(state == doNothing)
    {
        a = arg1;
        setFocalLength();
    }
    ui->spinBox->setValue(a);
}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    if(state == doNothing)
    {
        b = arg1;
        setFocalLength();
    }
    ui->spinBox_2->setValue(b);
}

void MainWindow::on_spinBox_3_valueChanged(int arg1)
{
    if(state == doNothing)
    {
        c = arg1;
        setFocalLength();
    }
    ui->spinBox_3->setValue(c);
}

void MainWindow::on_btnRESET_clicked()
{
    if((state == tracking || state == doNothing))
    {
        state = retrack;
    }
}

void MainWindow::on_btnSTOP_clicked()
{
    state = ending;
}

void MainWindow::on_btnSTART_clicked()
{
    if(state == doNothing)
    {
        state = start;
    }

}

void MainWindow::on_btnCNTSET_clicked()
{
    if(state == tracking)
    {
        state = CNTSET_tracking;
    }
    if(state == doNothing)
    {
        state = CNTSET_doNothing;
    }
}

void MainWindow::on_btnZERO_clicked()
{
    if(state == tracking)
    {
        state = setOrigin;
    }
}

void MainWindow::on_spinBox_4_valueChanged(int arg1)
{
    int d0 = cnt % 10;
    cnt = arg1 * 10 + d0;
    if(cnt <= 0) {
        cnt = 1;
        ui->spinBox_4->setValue(0);
        ui->spinBox_5->setValue(1);
    }
}

void MainWindow::on_spinBox_5_valueChanged(int arg1)
{
    int d1 = cnt / 10;
    cnt = d1 * 10 + arg1;
    if(cnt <= 0) {
        cnt = 1;
        ui->spinBox_4->setValue(0);
        ui->spinBox_5->setValue(1);
    }
}



void MainWindow::on_shoot_clicked()
{
    if(state == doNothing) {
        state = shoot;
    }

}
