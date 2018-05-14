#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <QWidget>
#include <QPixmap>
#include <QtMath>
#include <QDebug>
#include <QVector3D>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

QImage cvMat2QImage(const Mat& mat);
Mat QImage2cvMat(QImage image);
void cvFitPlane(const CvMat* points, float* plane);
void calPlane(QVector3D v1, QVector3D v2, QVector3D point,float* plane);
void pathDetector(QVector3D p0, Mat mat);

#endif // FRAMEWORK_H
