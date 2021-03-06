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
#include <string>
#include <deque>
#include <vector>

using namespace cv;
using namespace std;

QImage cvMat2QImage(const Mat& mat);
Mat QImage2cvMat(QImage image);
void cvFitPlane(const CvMat* points, float* plane);
void calPlane(QVector3D v1, QVector3D v2, QVector3D point,float* plane);
void pathDetector(QVector3D p0, Mat mat);
void calInterLine(QVector3D v1, QVector3D v2,QVector3D &v3);

int compare (const void * a, const void * b);

int comparef (const void * a, const void * b);

int compared (const void * a, const void * b);

void createCurve(vector<Point> &originPoint,vector<Point> &curvePoint);

bool polynomial_curve_fit(std::vector<cv::Point>& key_point, int n, cv::Mat& A);

#endif // FRAMEWORK_H
