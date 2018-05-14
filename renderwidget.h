#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include "renderarea.h"
#include "pickpoint.h"

#include <QWidget>
#include <vector>
#include <deque>

#define JOINTS_NUMBER 21

using namespace std;

class finger
{
public:

};

class RenderWidget : public QWidget
{
    Q_OBJECT
private:
    int edgeThresh;
    int lowThreshold;
    int const max_lowThreshold = 100;
    int ratio;
    int kernel_size;

public:
    explicit RenderWidget(QWidget *parent = 0);
    RenderArea *renderArea;
    PickPoint *pickPointArray[JOINTS_NUMBER];
    PickPoint *handCtPoint;
    Point *handCenter;
    int step;

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;

signals:
    void changed();

public slots:
    void setCannyPara(int);
    void readFile();
    void writeFile();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};

void thinningIteration(cv::Mat& img, int iter);

void thinning(const cv::Mat& src, cv::Mat& dst);

void Erosion( int, void* );

bool findNextPoint(vector<Point> &_neighbor_points, Mat &_image, Point _inpoint, int flag, Point& _outpoint, int &_outflag);

bool findFirstPoint(Mat &_inputimg, Point &_outputpoint);

void findLines(Mat &inputimg, vector<deque<Point>> &_outputlines);

Scalar random_color(RNG& _rng);

void Refine(Mat& image);

void TJunctionDetect(Mat &img, vector<Point2f> &corners, vector<Point> &Tjunc, vector<Point2f> &norm, int r);

bool findTjunction(Mat &img, Point cenPt, vector<Point> &neighbor_points);

void findEnd(Mat &img, Point centerPoint, Point2f &vecDir);

void InitMat(Mat& m,float* num);

void RemoveSmallRegion(Mat& Src, Mat& Dst, int AreaLimit, int CheckMode, int NeihborMode);

bool findXPoint(Mat &img, Point point0,Point2f vecDir,Point &crossPt);

int findFarPoint(vector<Point> &_neighbor_points, Mat &_image, Point _inpoint, Point& _outpoint, int &count);
#endif // RENDERWIDGET_H
