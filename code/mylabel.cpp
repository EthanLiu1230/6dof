#include "mylabel.h"

myLabel::myLabel(QWidget *parent) :
QLabel(parent)
{
    setMouseTracking(true);
    this->isPressed = false;
}

void myLabel::paintEvent(QPaintEvent *event)        //overwrite
{
    QLabel::paintEvent(event);//call painEvent, show background

    QPainter painter(this);
    // set pen
    QPen pen;
    pen.setWidth(6);
    pen.setColor(Qt::red);
    painter.setPen(pen);
    // set font
    QFont font;
    font.setPixelSize(30);
    painter.setFont(font);

    if(isPressed)
    {
        painter.drawRect(QRect(QPoint(xpress, ypress), QPoint(x, y)));
        QString str = "";
        str.append("(").append(QString::number(xpress)).append(",").append(QString::number(ypress)).append(")");
        painter.drawText(xpress, ypress, str);
    }

    QString str = "";
    str.append("(").append(QString::number(x)).append(",").append(QString::number(y)).append(")");
    painter.drawText(x, y, str);

}
void myLabel::mouseMoveEvent(QMouseEvent *event) //overwrite
{
    emit mouseMoveSignal();

    x = event->x();
    y = event->y();
}
void myLabel::mousePressEvent(QMouseEvent *event)   //overwrite
{
    isPressed = true;
    xpress = event->x();
    ypress = event->y();
}
void myLabel::mouseReleaseEvent(QMouseEvent *event) //overwrite
{
    isPressed = false;
    xrelease = event->x();
    yrelease = event->y();
}



