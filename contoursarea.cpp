#include "contoursarea.h"
#include <QPainter>

ContoursArea::ContoursArea()
{
    setFixedSize(400,400);
    Mat Contours(400,400,CV_8UC1,Scalar(255));
//    imwrite("/Users/ebao/study/lab/Gesture/images/Contours.png",Contours);
}

void ContoursArea::paintEvent(QPaintEvent * /* event */)
{
    Mat M = imread("/Users/ebao/study/lab/Gesture/images/Contours.png");

    QImage img = cvMat2QImage(M);

    QPainter painter(this);
    painter.drawImage(QRect(0, 0, width(), height()), img);
    painter.end();
}
