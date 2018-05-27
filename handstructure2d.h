#ifndef HANDSTRUCTURE2D_H
#define HANDSTRUCTURE2D_H

#include "framework.h"
#include <QVector2D>
#include <string>

using namespace std;

struct Finger2D{
    float length[4] = {0,0,0,0};
    float Dof[4] = {0,0,0,0};
};

class HandStructure2D
{
private:
    const float angle[5] = {
//        -45.0,
//        -15.0,
//        0.1,
//        15.0,
//        30.0
        45.0,
        60.0,
        90.0,
        105.0,
        120.0
    };

//    float angle_rad[5];

    const float length[5][4] = {
        0.8f, 1.0f, 0.5f, 1.0f,
        2.0f, 1.0f, 1.0f, 1.0f,
        2.0f, 1.0f, 1.0f, 1.0f,
        2.0f, 1.0f, 1.0f, 1.0f,
        2.0f, 1.0f, 1.0f, 1.0f,
    };

    Finger2D* fingers[5];

    int adist;
    int thumbIdx;
    int scale;
    int valid_fingerN;
    float basisThumbAngle;
    string folder_path;

    vector<deque<Point>> fingerlines;
    vector<vector<int>> fingerJointsIdx;

    Point fakeend = Point(-1,-1);
    QVector2D fakeendvec = QVector2D(-1,-1);
    int fakePointIdx = -1;

public:
    HandStructure2D();
    QVector2D center;
    QVector2D joints[5][4];
    Point centerPt;
    Point jointPts[5][4];
    int joint_valid[5][4];

    int indexofImg;

    void readFile(QString filename,vector<deque<Point>> &fingerlines, vector<vector<int>> &fingerJointsIdx, Point &center, int &thumbIdx, int &adist);
    void writeFile(QString filename);

    void calPoints();
    void drawPoints();
    void syncPts();
    void syncVec();
    void fitSkeleton();
    void fitSkeleton_parb();
    void initPoints();
    void initPoints_parb();

    void sortFingers();
    void adjustSkeleton();
    float calVariance();
    void calGesture();
    void findRoot();

    void calBLine();


    //    void clear();

//    calFingers();
};

#endif // HANDSTRUCTURE2D_H
