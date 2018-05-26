#ifndef THINNING_H
#define THINNING_H

#include "framework.h"

#include <string>

using namespace std;

void thinningIteration(cv::Mat& img, int iter);

void thinning(const cv::Mat& src, cv::Mat& dst);

void removeDoubleLine(cv::Mat &src_img,cv::Mat &out_img);
#endif // THINNING_H
