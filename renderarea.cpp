#include "renderarea.h"
#include <QPainter>
#include <QDebug>

RenderArea::RenderArea(QWidget *parent) : QWidget(parent)
{
    setFixedSize(400,400);
    pickPoint = new PickPoint();
//    pickPoint->show();
    pickPoint->setGeometry(50,50,pickPoint->width(),pickPoint->height());
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    Mat M = imread("/Users/ebao/study/lab/Gesture/images/gesture1.jpg");

    QImage img = cvMat2QImage(M);

    QPainter painter(this);
    painter.drawImage(QRect(0, 0, width(), height()), img);
    painter.end();
}

