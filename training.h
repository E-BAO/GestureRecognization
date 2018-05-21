#ifndef TRAINING_H
#define TRAINING_H

#include "imgprocessing.h"
#include "tjunctiondetect.h"
#include "thinning.h"

using namespace std;

void training(string folder_path, vector<Point> &out_para, Mat &output_img);

Scalar random_color(RNG& _rng);

#endif // TRAINING_H
