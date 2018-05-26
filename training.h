#ifndef TRAINING_H
#define TRAINING_H

#include "imgprocessing.h"
#include "tjunctiondetect.h"
#include "thinning.h"

using namespace std;

void training(string folder_path, vector<Point> &out_para, Mat &output_img);

Scalar random_color(RNG& _rng);

void lineCurve(string folder_path, deque<Point> &line, vector<double> &curvature, vector<int> &curvePointIdx, vector<int> &maxima_idx, const int size, const float threshold);

void findFingerEnds(string folder_path, Mat &maskimg, deque<Point> &line, vector<Point> &curve_vex, const int size, const float threshold, const int dirlen);

void localMaxima(vector<double> src,const int minPeakDistance,const double minHeightDiff,const double minPeakHeight,vector<int> &maxLoc);

void findFingerLine(string folder_path, Mat &img, vector<deque<Point>> &lines, vector<Point> &finger_ends, vector<Point> &finger_ends_ol);

int countMat(Mat& inputImg, int val);

#endif // TRAINING_H
