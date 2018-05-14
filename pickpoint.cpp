#include "pickpoint.h"

#include <QDebug>
#include <QPainter>


PickPoint::PickPoint(QWidget *parent) : QLabel(parent)
{
    //    this->setText("hello");
    brushColor = new QColor(Qt::red);
    size = 8;
    setFixedSize(size,size);
}


void PickPoint::mousePressEvent(QMouseEvent *e)
{
    qDebug()<<"left"<<"button pressed";
    lX = e->x();
    lY = e->y();
    emit moved();
}

void PickPoint::mouseMoveEvent(QMouseEvent *e)
{
    int dX = e->x() - lX;
    int dY = e->y() - lY;
//    cX = this->x() + dX + this->width();
//    cY = this->y() + dY + this->height();
    qDebug()<<"move"<<e->x()<<e->y()<<"width"<<this->width()<<"height"<<this->height();
    this->setGeometry(this->x()+dX, this->y()+dY,this->width(),this->height());
    emit moved();
}

QSize PickPoint::minimumSizeHint() const
{
    return QSize(size, size);
}

QSize PickPoint::sizeHint() const
{
    return QSize(size, size);
}

void PickPoint::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(*brushColor);
    painter.drawEllipse(0,0,size,size);
    painter.end();
}
