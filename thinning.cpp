#include "thinning.h"

/**
 * Perform one thinning iteration.
 * Normally you wouldn't call this function directly from your code.
 *
 * Parameters:
 * 		im    Binary image with range = [0,1]
 * 		iter  0=even, 1=odd
 */

/*
bool ThiningDIBSkeleton (unsigned char* lpDIBBits, int lWidth, int lHeight)
{
    //循环变量
    long i;
    long j;
    long lLength;

    unsigned char deletemark[256] = {      // 这个即为前人据8领域总结的是否可以被删除的256种情况
        0,0,0,0,0,0,0,1,    0,0,1,1,0,0,1,1,
        0,0,0,0,0,0,0,0,    0,0,1,1,1,0,1,1,
        0,0,0,0,0,0,0,0,    1,0,0,0,1,0,1,1,
        0,0,0,0,0,0,0,0,    1,0,1,1,1,0,1,1,
        0,0,0,0,0,0,0,0,    0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,    0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,    1,0,0,0,1,0,1,1,
        1,0,0,0,0,0,0,0,    1,0,1,1,1,0,1,1,
        0,0,1,1,0,0,1,1,    0,0,0,1,0,0,1,1,
        0,0,0,0,0,0,0,0,    0,0,0,1,0,0,1,1,
        1,1,0,1,0,0,0,1,    0,0,0,0,0,0,0,0,
        1,1,0,1,0,0,0,1,    1,1,0,0,1,0,0,0,
        0,1,1,1,0,0,1,1,    0,0,0,1,0,0,1,1,
        0,0,0,0,0,0,0,0,    0,0,0,0,0,1,1,1,
        1,1,1,1,0,0,1,1,    1,1,0,0,1,1,0,0,
        1,1,1,1,0,0,1,1,    1,1,0,0,1,1,0,0
    };//索引表

    unsigned char p0, p1, p2, p3, p4, p5, p6, p7;
    unsigned char *pmid, *pmidtemp;    // pmid 用来指向二值图像  pmidtemp用来指向存放是否为边缘
    unsigned char sum;
    bool bStart = true;
    lLength = lWidth * lHeight;
    unsigned char *pTemp = new uchar[sizeof(unsigned char) * lWidth * lHeight]();  //动态创建数组 并且初始化

    //    P0 P1 P2
    //    P7    P3
    //    P6 P5 P4

    while(bStart)
    {
        bStart = false;

        //首先求边缘点
        pmid = (unsigned char *)lpDIBBits + lWidth + 1;
        memset(pTemp,  0, lLength);
        pmidtemp = (unsigned char *)pTemp + lWidth + 1; //  如果是边缘点 则将其设为1
        for(i = 1; i < lHeight -1; i++)
        {
            for(j = 1; j < lWidth - 1; j++)
            {
                if( *pmid == 0)                   //是0 不是我们需要考虑的点
                {
                    pmid++;
                    pmidtemp++;
                    continue;
                }
                p3 = *(pmid + 1);
                p2 = *(pmid + 1 - lWidth);
                p1 = *(pmid - lWidth);
                p0 = *(pmid - lWidth -1);
                p7 = *(pmid - 1);
                p6 = *(pmid + lWidth - 1);
                p5 = *(pmid + lWidth);
                p4 = *(pmid + lWidth + 1);
                sum = p0 & p1 & p2 & p3 & p4 & p5 & p6 & p7;
                if(sum == 0)
                {
                    *pmidtemp = 1;       // 这样周围8个都是1的时候  pmidtemp==1 表明是边缘
                }

                pmid++;
                pmidtemp++;
            }
            pmid++;
            pmid++;
            pmidtemp++;
            pmidtemp++;
        }

        //现在开始删除
        pmid = (unsigned char *)lpDIBBits + lWidth + 1;
        pmidtemp = (unsigned char *)pTemp + lWidth + 1;

        for(i = 1; i < lHeight -1; i++)   // 不考虑图像第一行 第一列 最后一行 最后一列
        {
            for(j = 1; j < lWidth - 1; j++)
            {
                if( *pmidtemp == 0)     //1表明是边缘 0--周围8个都是1 即为中间点暂不予考虑
                {
                    pmid++;
                    pmidtemp++;
                    continue;
                }

                p3 = *(pmid + 1);
                p2 = *(pmid + 1 - lWidth);
                p1 = *(pmid - lWidth);
                p0 = *(pmid - lWidth -1);
                p7 = *(pmid - 1);
                p6 = *(pmid + lWidth - 1);
                p5 = *(pmid + lWidth);
                p4 = *(pmid + lWidth + 1);

                p1 *= 2;
                p2 *= 4;
                p3 *= 8;
                p4 *= 16;
                p5 *= 32;
                p6 *= 64;
                p7 *= 128;

                sum = p0 | p1 | p2 | p3 | p4 | p5 | p6 | p7;
            //  sum = p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7;
                if(deletemark[sum] == 1)
                {
                    *pmid = 0;
                    bStart = true;      //  表明本次扫描进行了细化
                }
                pmid++;
                pmidtemp++;
            }

            pmid++;
            pmid++;
            pmidtemp++;
            pmidtemp++;
        }
    }
    delete []pTemp;
    return true;
}
*/

void thinningIteration(cv::Mat& img, int iter)
{
    CV_Assert(img.channels() == 1);
    CV_Assert(img.depth() != sizeof(uchar));
    CV_Assert(img.rows > 3 && img.cols > 3);

    cv::Mat marker = cv::Mat::zeros(img.size(), CV_8UC1);

    int nRows = img.rows;
    int nCols = img.cols;

    if (img.isContinuous()) {
        nCols *= nRows;
        nRows = 1;
    }

    int x, y;
    uchar *pAbove;
    uchar *pCurr;
    uchar *pBelow;
    uchar *nw, *no, *ne;    // north (pAbove)
    uchar *we, *me, *ea;
    uchar *sw, *so, *se;    // south (pBelow)

    uchar *pDst;

    // initialize row pointers
    pAbove = NULL;
    pCurr  = img.ptr<uchar>(0);
    pBelow = img.ptr<uchar>(1);

    for (y = 1; y < img.rows-1; ++y) {
        // shift the rows up by one
        pAbove = pCurr;
        pCurr  = pBelow;
        pBelow = img.ptr<uchar>(y+1);

        pDst = marker.ptr<uchar>(y);

        // initialize col pointers
        no = &(pAbove[0]);
        ne = &(pAbove[1]);
        me = &(pCurr[0]);
        ea = &(pCurr[1]);
        so = &(pBelow[0]);
        se = &(pBelow[1]);

        for (x = 1; x < img.cols-1; ++x) {
            // shift col pointers left by one (scan left to right)
            nw = no;
            no = ne;
            ne = &(pAbove[x+1]);
            we = me;
            me = ea;
            ea = &(pCurr[x+1]);
            sw = so;
            so = se;
            se = &(pBelow[x+1]);

            int A  = (*no == 0 && *ne == 1) + (*ne == 0 && *ea == 1) +
                     (*ea == 0 && *se == 1) + (*se == 0 && *so == 1) +
                     (*so == 0 && *sw == 1) + (*sw == 0 && *we == 1) +
                     (*we == 0 && *nw == 1) + (*nw == 0 && *no == 1);
            int B  = *no + *ne + *ea + *se + *so + *sw + *we + *nw;
            int m1 = iter == 0 ? (*no * *ea * *so) : (*no * *ea * *we);
            int m2 = iter == 0 ? (*ea * *so * *we) : (*no * *so * *we);

            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
                pDst[x] = 1;
        }
    }

    img &= ~marker;
}

/**
 * Function for thinning the given binary image
 *
 * Parameters:
 * 		src  The source image, binary with range = [0,255]
 * 		dst  The destination image
 */
void thinning(const cv::Mat& src, cv::Mat& dst)
{
    dst = src.clone();
    dst /= 255;         // convert to binary image

    cv::Mat prev = cv::Mat::zeros(dst.size(), CV_8UC1);
    cv::Mat diff;

    do {
        thinningIteration(dst, 0);
        thinningIteration(dst, 1);
        cv::absdiff(dst, prev, diff);
        dst.copyTo(prev);
    }
    while (cv::countNonZero(diff) > 0);

    dst *= 255;
}

