#ifndef THINNING_H
#define THINNING_H

#include "framework.h"

#include <string>

using namespace std;

void thinningIteration(cv::Mat& img, int iter);

void thinning(const cv::Mat& src, cv::Mat& dst);

#endif // THINNING_H
