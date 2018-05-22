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
