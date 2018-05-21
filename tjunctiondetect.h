#ifndef TJUNCTIONDETECT_H
#define TJUNCTIONDETECT_H

#include <deque>
#include <vector>
#include "framework.h"
#include "imgprocessing.h"

using namespace std;

extern string folder_path;

void TJunctionDetect(Mat &img, vector<Point2f> &corners, vector<Point> &Tjunc, vector<Point2f> &norm1,vector<Point2f> &norm2, int r);

bool findTjunction(Mat &img, Point cenPt, vector<Point> &neighbor_points);

bool findXPoint(Mat &img, Point point0,Point2f vecDir,Point &crossPt);

#endif // TJUNCTIONDETECT_H
