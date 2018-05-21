#ifndef IMGPROCESSING_H
#define IMGPROCESSING_H

#include <vector>
#include <deque>
#include "framework.h"

using namespace std;

bool findNextPoint(vector<Point> &_neighbor_points, Mat &_image, Point _inpoint, int flag, Point& _outpoint, int &_outflag);

bool findFirstPoint(Mat &_inputimg, Point &_outputpoint);

void findLines(Mat &inputimg, vector<deque<Point>> &_outputlines);

void findEnd(Mat &img, Point centerPoint, Point2f &vecDir);

void InitMat(Mat& m,float* num);

void RemoveSmallRegion(Mat& Src, Mat& Dst, int AreaLimit, int CheckMode, int NeihborMode);

int findFarPoint(vector<Point> &_neighbor_points, Mat &_image, Point _inpoint, Point& _outpoint, int &count);


#endif // IMGPROCESSING_H
