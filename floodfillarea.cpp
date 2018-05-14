#include "floodfillarea.h"
#include <QPainter>

FloodFillArea::FloodFillArea(QWidget *parent) : QWidget(parent)
{
    setFixedSize(400,400);
    Mat floodFill(400,400,CV_8UC1,Scalar(0));
//    imwrite("/Users/ebao/study/lab/Gesture/images/FloodFill.png",floodFill);
}

void FloodFillArea::paintEvent(QPaintEvent * /* event */)
{
    Mat M = imread("/Users/ebao/study/lab/Gesture/images/FloodFill.png");

    QImage img = cvMat2QImage(M);

    QPainter painter(this);
    painter.drawImage(QRect(0, 0, width(), height()), img);
    painter.end();
}
