#include "framework.h"


QImage cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        //image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

cv::Mat QImage2cvMat(QImage image)
{
    cv::Mat mat;
    qDebug() << image.format();
    switch(image.format())
    {
        case QImage::Format_ARGB32:
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32_Premultiplied:
            mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
            break;
        case QImage::Format_RGB888:
            mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
            cv::cvtColor(mat, mat, CV_BGR2RGB);
            break;
        case QImage::Format_Indexed8:
            mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
            break;
    }
    return mat;
}

void cvFitPlane(const CvMat* points, float* plane){
   // Estimate geometric centroid.
   int nrows = points->rows;
   int ncols = points->cols;
   int type = points->type;
   CvMat* centroid = cvCreateMat(1, ncols, type);
   cvSet(centroid, cvScalar(0));
   for (int c = 0; c<ncols; c++){
       for (int r = 0; r < nrows; r++)
       {
           centroid->data.fl[c] += points->data.fl[ncols*r + c];
       }
       centroid->data.fl[c] /= nrows;
   }
   // Subtract geometric centroid from each point.
   CvMat* points2 = cvCreateMat(nrows, ncols, type);
   for (int r = 0; r<nrows; r++)
       for (int c = 0; c<ncols; c++)
           points2->data.fl[ncols*r + c] = points->data.fl[ncols*r + c] - centroid->data.fl[c];
   // Evaluate SVD of covariance matrix.
   CvMat* A = cvCreateMat(ncols, ncols, type);
   CvMat* W = cvCreateMat(ncols, ncols, type);
   CvMat* V = cvCreateMat(ncols, ncols, type);
   cvGEMM(points2, points, 1, NULL, 0, A, CV_GEMM_A_T);
   cvSVD(A, W, NULL, V, CV_SVD_V_T);
   // Assign plane coefficients by singular vector corresponding to smallest singular value.
   plane[ncols] = 0;
   for (int c = 0; c<ncols; c++){
       plane[c] = V->data.fl[ncols*(ncols - 1) + c];
       plane[ncols] += plane[c] * centroid->data.fl[c];
   }
   // Release allocated resources.
   cvReleaseMat(&centroid);
   cvReleaseMat(&points2);
   cvReleaseMat(&A);
   cvReleaseMat(&W);
   cvReleaseMat(&V);
}

void calPlane(QVector3D v1, QVector3D v2, QVector3D point,float* plane){
    QVector3D n = QVector3D::crossProduct(v1,v2);
    plane[0] = n.x();
    plane[1] = n.y();
    plane[2] = n.z();
    plane[3] = QVector3D::dotProduct(n,point);
}

void calInterLine(QVector3D v1, QVector3D v2,QVector3D &v3){
    v3 = QVector3D::crossProduct(v1,v2);
}

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

int comparef (const void * a, const void * b)
{
    return *(float *)a > *(float *)b ? 1 : -1;
}

int compared (const void * a, const void * b)
{
    return *(double *)a > *(double *)b ? 1 : -1;
}


//三次贝塞尔曲线
float bezier3funcX(float uu,Point *controlP){
   float part0 = controlP[0].x * uu * uu * uu;
   float part1 = 3 * controlP[1].x * uu * uu * (1 - uu);
   float part2 = 3 * controlP[2].x * uu * (1 - uu) * (1 - uu);
   float part3 = controlP[3].x * (1 - uu) * (1 - uu) * (1 - uu);
   return part0 + part1 + part2 + part3;
}
float bezier3funcY(float uu,Point *controlP){
   float part0 = controlP[0].y * uu * uu * uu;
   float part1 = 3 * controlP[1].y * uu * uu * (1 - uu);
   float part2 = 3 * controlP[2].y * uu * (1 - uu) * (1 - uu);
   float part3 = controlP[3].y * (1 - uu) * (1 - uu) * (1 - uu);
   return part0 + part1 + part2 + part3;
}

void createCurve(vector<Point> &originPoint,vector<Point> &curvePoint){
    //控制点收缩系数 ，经调试0.6较好，Point是opencv的，可自行定义结构体(x,y)
    float scale = 0.2;

    int originCount = originPoint.size();
    Point midpoints[originCount];
    //生成中点
    for(int i = 0 ;i < originCount ; i++){
        int nexti = (i + 1) % originCount;
        midpoints[i].x = (originPoint[i].x + originPoint[nexti].x)/2.0;
        midpoints[i].y = (originPoint[i].y + originPoint[nexti].y)/2.0;
    }

    //平移中点
    Point extrapoints[2 * originCount];
    for(int i = 0 ;i < originCount ; i++){
         int nexti = (i + 1) % originCount;
         int backi = (i + originCount - 1) % originCount;
         Point midinmid;
         midinmid.x = (midpoints[i].x + midpoints[backi].x)/2.0;
         midinmid.y = (midpoints[i].y + midpoints[backi].y)/2.0;
         int offsetx = originPoint[i].x - midinmid.x;
         int offsety = originPoint[i].y - midinmid.y;
         int extraindex = 2 * i;
         extrapoints[extraindex].x = midpoints[backi].x + offsetx;
         extrapoints[extraindex].y = midpoints[backi].y + offsety;
         //朝 originPoint[i]方向收缩
         int addx = (extrapoints[extraindex].x - originPoint[i].x) * scale;
         int addy = (extrapoints[extraindex].y - originPoint[i].y) * scale;
         extrapoints[extraindex].x = originPoint[i].x + addx;
         extrapoints[extraindex].y = originPoint[i].y + addy;

         int extranexti = (extraindex + 1)%(2 * originCount);
         extrapoints[extranexti].x = midpoints[i].x + offsetx;
         extrapoints[extranexti].y = midpoints[i].y + offsety;
         //朝 originPoint[i]方向收缩
         addx = (extrapoints[extranexti].x - originPoint[i].x) * scale;
         addy = (extrapoints[extranexti].y - originPoint[i].y) * scale;
         extrapoints[extranexti].x = originPoint[i].x + addx;
         extrapoints[extranexti].y = originPoint[i].y + addy;

    }

    Point controlPoint[4];
    //生成4控制点，产生贝塞尔曲线
    for(int i = 0 ;i < originCount ; i++){
           controlPoint[0] = originPoint[i];
           int extraindex = 2 * i;
           controlPoint[1] = extrapoints[extraindex + 1];
           int extranexti = (extraindex + 2) % (2 * originCount);
           controlPoint[2] = extrapoints[extranexti];
           int nexti = (i + 1) % originCount;
           controlPoint[3] = originPoint[nexti];
           float u = 1;
           while(u >= 0){
               int px = bezier3funcX(u,controlPoint);
               int py = bezier3funcY(u,controlPoint);
               //u的步长决定曲线的疏密
               u -= 0.005;
               Point tempP = Point(px,py);
               //存入曲线点
               curvePoint.push_back(tempP);
           }
    }
}

bool polynomial_curve_fit(std::vector<cv::Point>& key_point, int n, cv::Mat& A)
{
    //Number of key points
    int N = key_point.size();

    //构造矩阵X
    cv::Mat X = cv::Mat::zeros(n + 1, n + 1, CV_64FC1);
    for (int i = 0; i < n + 1; i++)
    {
        for (int j = 0; j < n + 1; j++)
        {
            for (int k = 0; k < N; k++)
            {
                X.at<double>(i, j) = X.at<double>(i, j) +
                    std::pow(key_point[k].x, i + j);
            }
        }
    }

    //构造矩阵Y
    cv::Mat Y = cv::Mat::zeros(n + 1, 1, CV_64FC1);
    for (int i = 0; i < n + 1; i++)
    {
        for (int k = 0; k < N; k++)
        {
            Y.at<double>(i, 0) = Y.at<double>(i, 0) +
                std::pow(key_point[k].x, i) * key_point[k].y;
        }
    }

    A = cv::Mat::zeros(n + 1, 1, CV_64FC1);
    //求解矩阵A
    cv::solve(X, Y, A, cv::DECOMP_LU);
    return true;
}

