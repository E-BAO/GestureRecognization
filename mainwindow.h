#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "renderarea.h"
#include "pickpoint.h"
#include "renderwidget.h"
#include "floodfillarea.h"
#include "contoursarea.h"
#include "detectededgearea.h"
#include "glwidget.h"

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QMouseEvent>
#include <QSlider>
#include <QLabel>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void mousePressEvent(QMouseEvent *);
//    void mouseMoveEvent(QMouseEvent *e);    //--鼠标移动事件
//    void mouseReleaseEvent(QMouseEvent *e); //--鼠标释放（松开）事件
//    void mouseDoubleClickEvent(QMouseEvent *e); //--鼠标双击事件
    void keyPressEvent(QKeyEvent *e);
    QSlider *createSlider();

    QLabel *topHintLabel;
    QPushButton *generateBtn;
    QPushButton *passBtn;
    QPushButton *ReadFileBtn;
    QPushButton *WriteFileBtn;
    QSlider *cannySlider;
    QLabel *cannyLabel;
    RenderWidget *renderWidget;
    FloodFillArea *floodfillArea;
    ContoursArea *contoursArea;
    DetectedEdgeArea *detectedEdgeArea;
    int stepN;
    GLWidget *glWidget;
    QSlider *xSlider;
    QSlider *ySlider;
    QSlider *zSlider;

public slots:
    void updateHint();
    void cannyChanged(int);
    void generatePoints();
};

#endif // MAINWINDOW_H
