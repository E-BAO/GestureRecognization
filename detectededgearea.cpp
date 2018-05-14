#include "detectededgearea.h"
#include <QPainter>

DetectedEdgeArea::DetectedEdgeArea(QWidget *parent) : QWidget(parent)
{
    setFixedSize(400,400);
    Mat edges(400,400,CV_8UC1,Scalar(0));
    edgeThresh = 1;
    lowThreshold = 0;
    ratio = 3;
    kernel_size = 3;

    /***************/
//    cv::imdecode()
    /***************/
//    imwrite("/Users/ebao/study/lab/Gesture/images/edges.png",edges);
}

void DetectedEdgeArea::paintEvent(QPaintEvent * /* event */)
{
////    Mat M = imread("/Users/ebao/study/lab/Gesture/images/edges.png");

//    Mat src = imread("/Users/ebao/study/lab/Gesture/images/gesture1.jpg");///Users/ebao/Pictures/hazard.png");//
//    Mat src_gray;
//    cvtColor(src, src_gray,CV_BGR2GRAY);

//    Mat dst, detected_edges;

//    /// Reduce noise with a kernel 3x3
//    blur( src_gray, detected_edges, Size(3,3) );

//    threshold(src_gray,src_gray,128,255,THRESH_BINARY);//转换成2值图像
////    imshow("src_gray",src_gray);
//    // Canny detector
//    Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

//    /// Using Canny's output as a mask, we display our result
//    dst = Scalar::all(0);

//    src.copyTo( dst, detected_edges);
////    threshold(dst,dst,128,255,THRESH_BINARY);//转换成2值图像
////    imwrite("/Users/ebao/study/lab/Gesture/images/edges.png",dst);

    Mat M = imread("/Users/ebao/study/lab/Gesture/images/Contours.png");


    QImage img = cvMat2QImage(M);

    QPainter painter(this);
    painter.drawImage(QRect(0, 0, width(), height()), img);
    painter.end();
}

void DetectedEdgeArea::setCannyPara(int v){
    lowThreshold = v;
    update();
}
