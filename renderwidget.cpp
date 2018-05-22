#include "renderwidget.h"
#include <QGridLayout>
#include <QPainter>
#include <QFile>
#include <QMessageBox>
#include <QString>
#include <QList>
#include <QDebug>
#include <QApplication>

using namespace std;

typedef struct color Color;

struct color{
    float b,g,r;
    color(float blue,float green, float red):b(blue),g(green),r(red){}
};

//BGR
Color c_black(0, 0, 0);
Color c_red(0,0,1.0);
Color c_yellow(0,1.0,1.0);
Color c_green(0,1.0,0);
Color c_cyan(1.0,1.0,0);
Color c_blue(1,0,0);

Color paintColor[6] = {
    c_black,c_red,c_yellow,c_green,c_cyan,c_blue
};

/***************/


RenderWidget::RenderWidget(QWidget *parent) : QWidget(parent)
{
    edgeThresh = 1;
    lowThreshold = 0;
    ratio = 3;
    kernel_size = 3;

    step = 0;
    for(int i = 0;i < JOINTS_NUMBER;i++){
        pickPointArray[i] = new PickPoint(this);
//        connect(pickPointArray[i],SIGNAL(moved()) ,
//                this, SLOT(update()));
        pickPointArray[i]->setVisible(false);
    }
    handCtPoint = new PickPoint(this);
    handCenter = new Point(174,238);
    handCtPoint->setGeometry(handCenter->x - handCtPoint->width()/2,handCenter->y-handCtPoint->height()/2,handCtPoint->width(),handCtPoint->height());
}

QSize RenderWidget::minimumSizeHint() const
{
    return QSize(400, 400);
}

QSize RenderWidget::sizeHint() const
{
    return QSize(400, 400);
}

void RenderWidget::mousePressEvent(QMouseEvent *e){
    if(step >= JOINTS_NUMBER)return;
    PickPoint *point = handCtPoint;//pickPointArray[step];
    point->setVisible(true);
    point->setGeometry(e->x() - point->width()/2,e->y()-point->height()/2,point->width(),point->height());
    step++;
}

void RenderWidget::mouseMoveEvent(QMouseEvent *ev) {
    qDebug()<<"mouse move at"<<ev->x()<<ev->y();
}

void RenderWidget::keyPressEvent(QKeyEvent *e){
    qDebug()<<"key = ";
    QWidget::keyPressEvent(e);
}



//QStringList arguments = QCoreApplication::arguments();
////QString str = arguments.at(0);

// + to_string(indexofImg) + "/";

void RenderWidget::paintEvent(QPaintEvent * /* event */)
{
    int indexofImg = 2;

    string folder_path = "/Users/ebao/study/lab/Gesture/images/gesture" + to_string(indexofImg) + "/";

    QString filepath = QString::fromStdString(folder_path) + "parameters.txt";

    vector<deque<Point>> fingerlines;
    vector<vector<int>> fingerJointsIdx;

    Point center;
    int thumbIdx;
    int adist;
    readFile(filepath, fingerlines, fingerJointsIdx,center,thumbIdx,adist);
//    qDebug()<<"center = "<<center.x<<center.y<<"thumb ="<<thumbIdx;

    fitSkeleton(fingerlines, fingerJointsIdx,center,thumbIdx,adist);

    Mat src = imread(folder_path+"gesture.jpg");
    Mat src_gray;
    cvtColor(src, src_gray,CV_RGB2GRAY);

    QImage img = cvMat2QImage(src_gray);

    QPainter painter(this);
    painter.drawImage(QRect(0, 0, width(), height()), img);
    painter.end();

//    emit changed();
}

void RenderWidget::setCannyPara(int v){
    lowThreshold = v;
    update();
}

void RenderWidget::fitSkeleton(vector<deque<Point> > &fingerlines, vector<vector<int> > &fingerJointsIdx, Point &center, int &thumbIdx,int &adist){
    PickPoint *p0 = pickPointArray[0];
    Point fakePoint(-1,-1);
    int fakePointIdx = -1;
    p0->setGeometry(center.x,center.y,p0->width(),p0->height());
    p0->setVisible(true);

    for(int i = 0;i < fingerJointsIdx.size();i++){
        while(fingerJointsIdx[i].size() < 4){
            fingerJointsIdx[i].push_back(fakePointIdx);
        }
    }

    deque<Point> fingerThumb = fingerlines[thumbIdx];
    vector<int> fingerThumbIdx = fingerJointsIdx[thumbIdx];

    for(int j = 4; j > 0;j --){
//        qDebug()<<"line = "<<4 - j<<"idx = "<<fingerThumbIdx[4 - j];
        PickPoint *p = pickPointArray[j];
        Point pp;
        if(fingerThumbIdx[4 - j] == fakePointIdx){
            pp = fakePoint;
        }else{
            pp = fingerThumb[fingerThumbIdx[4 - j]];
        }
        p->setGeometry(pp.x,pp.y,p->width(),p->height());
        p->setVisible(true);
    }

    int count = 0;
    for(int i = 0; i < 5; i ++){
        if(i == thumbIdx)
            continue;

        deque<Point> fingerLine = fingerlines[i];
        vector<int> fingerIdx = fingerJointsIdx[i];
        for(int j = 0;j < 4; j ++){

//            qDebug()<<"line = "<<i<<"finger = "<<3 - j<<"idx = "<<fingerIdx[3 - j];

            Point pp;
            PickPoint *p = pickPointArray[5 + count * 4 + j];
            if(fingerIdx[3 - j] == fakePointIdx)
                pp = fakePoint;
            else
                pp = fingerLine[fingerIdx[3 - j]];

            p->setGeometry(pp.x,pp.y,p->width(),p->height());
            p->setVisible(true);
        }
        count++;
    }

}


void RenderWidget::readFile(QString filename,vector<deque<Point>> &fingerlines, vector<vector<int>> &fingerJointsIdx, Point &center, int &thumbIdx, int &adist)
{
    QFile f(filename);
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Open failed.";
        return;
    }

    QTextStream txtInput(&f);
    QString lineStr;
    lineStr = txtInput.readLine();
    QList<QString> lst;
    lst=lineStr.split('\t');
    int n = lst[0].toInt();

    lineStr = txtInput.readLine();
    lst=lineStr.split('\t');
    center.x = lst[0].toInt();
    center.y = lst[1].toInt();

    thumbIdx = lst[2].toInt();
    //    PickPoint *p0 = pickPointArray[0];
//    p0->setGeometry(x,y,p0->width(),p0->height());
//    p0->setVisible(true);

    int countii = 0;
    while(countii < n)
    {
        lineStr = txtInput.readLine();
        lst=lineStr.split('\t');
        deque<Point> fingerline;
        for(int j = 0;j < (lst.size() - 1) / 2;j ++){
//            PickPoint *p = pickPointArray[1+i * 4 + j];
            int x = lst[j * 2].toInt();
            int y = lst[j * 2 + 1].toInt();
            fingerline.push_back(Point(x,y));
//            p->setGeometry(x,y,p->width(),p->height());
//            p->setVisible(true);
        }
        fingerlines.push_back(fingerline);
//        qDebug()<<"fingerline"<<countii<<" size = "<<fingerline.size();
        countii++;
    }
//    qDebug()<<"fingerline total"<<fingerlines.size();


    countii = 0;
    while(countii < n)
    {
        lineStr = txtInput.readLine();
        lst=lineStr.split('\t');
        vector<int> fingerIdx;
        for(int j = 0;j < lst.size() - 1;j ++){
//            PickPoint *p = pickPointArray[1+i * 4 + j];
            int idx = lst[j].toInt();
            fingerIdx.push_back(idx);

//            p->setGeometry(x,y,p->width(),p->height());
//            p->setVisible(true);
        }
        fingerJointsIdx.push_back(fingerIdx);
//        qDebug()<<"fingerIdx"<<countii<<" size = "<<fingerIdx.size();

        countii++;
    }

//    qDebug()<<"fingerJointsIdx total"<<fingerJointsIdx.size();

//    step = JOINTS_NUMBER;

    f.close();
}

void RenderWidget::writeFile()
{
    if(step >= JOINTS_NUMBER){
        QFile f("test.txt");
        if(!f.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Open failed.";
            return;
        }
        QTextStream txtOutput(&f);

        PickPoint *p0 = pickPointArray[0];
        float x = p0->x();
        float y = p0->y();
        txtOutput << x << "\t" << y << "\t\n";

        for(int i = 0;i < 5;i ++){
            for(int j = 0;j < 4;j ++){
                PickPoint *p = pickPointArray[1+i * 4 + j];
                int x = p->x();
                int y = p->y();
                txtOutput << x << "\t" << y << "\t";
            }
            txtOutput <<"\n";
        }
        f.close();
    }else{
        QString string = "Joints not enough:" + QString::number(step);
        QMessageBox::about(NULL, "Write Error", string);
    }
}
