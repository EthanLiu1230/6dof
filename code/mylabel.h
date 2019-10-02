#ifndef MYLABEL_H
#define MYLABEL_H
#include <QMouseEvent>
#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <qpixmap.h>
#include <QString>
#include <QDebug>

class myLabel : public QLabel
{
    Q_OBJECT
public:
    myLabel(QWidget *parent);
    bool isPressed;
    int x, y;               // position
    int xpress, ypress, xrelease, yrelease;
protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void mouseMoveSignal(); // only need prototype statement , don't need to define in mylabel.cpp
};

#endif // MYLABEL_H
