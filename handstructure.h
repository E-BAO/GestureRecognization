#ifndef HANDSTRUCTURE_H
#define HANDSTRUCTURE_H

#include "framework.h"
#include <QVector4D>
#include <QVector3D>
#include <QVector2D>
#include <qmath.h>
#include <deque>
#include <vector>

using namespace std;

#define JOINTS_NUMBER 21

struct Finger{
    float length[4] = {0,0,0,0};
    float rz0 = 0;
    //rx1,rz1,rx2,rx3;
    float Dof[4] = {0,0,0,0};
};

class HandStructure
{
private:
    QVector3D position;
    QVector3D rotation;

    const float angle[5] = {
        -45.0,
        -15.0,
        0.1,
        15.0,
        30.0
    };

//    const float length[5][4] = {
//        1.0f, 1.0f, 0.5f, 1.0f,
//        2.0f, 0.9f, 0.9f, 0.9f,
//        2.0f, 1.0f, 1.0f, 1.0f,
//        2.0f, 0.9f, 0.9f, 0.9f,
//        2.0f, 0.8f, 0.8f, 0.8f,
//    };

    const float length[5][4] = {
        0.5f, 0.5f, 0.5f, 1.0f,
        2.0f, 1.0f, 1.0f, 1.0f,
        2.0f, 1.0f, 1.0f, 1.0f,
        2.0f, 1.0f, 1.0f, 1.0f,
        2.0f, 1.0f, 1.0f, 1.0f,
    };

    Finger* fingers[5];
    QVector2D points2D[JOINTS_NUMBER];

    int adist;
    int thumbIdx;

public:
    HandStructure();
    QVector3D joints[5][5];
    void CalPoints();
    void CalGesture();
    void fitSkeleton(vector<deque<Point> > &fingerlines, vector<vector<int> > &fingerJointsIdx, Point &center, int &thumbIdx,int &adist);
    void readFile(QString filename,vector<deque<Point>> &fingerlines, vector<vector<int>> &fingerJointsIdx, Point &center, int &thumbIdx,int &adist);

signals:
    void changed();

};

#endif // HANDSTRUCTURE_H
