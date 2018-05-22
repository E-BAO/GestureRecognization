#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include "renderarea.h"
#include "pickpoint.h"
#include "training.h"

#include <QWidget>

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
    void readFile(QString filename, vector<deque<Point> > &fingerlines, vector<vector<int> > &fingerJointsIdx, Point &center, int &thumbIdx,int &adist);
    void writeFile();
    void fitSkeleton(vector<deque<Point> > &fingerlines, vector<vector<int> > &fingerJointsIdx, Point &center, int &thumbIdx,int &adist);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};

#endif // RENDERWIDGET_H
