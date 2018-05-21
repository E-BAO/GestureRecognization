#include "renderwidget.h"
#include <QGridLayout>
#include <QPainter>
#include <QFile>
#include <QMessageBox>
#include <QString>
#include <QList>
#include <QDebug>
#include <deque>
#include <QApplication>

using namespace std;

typedef struct color Color;

struct color{
    float b,g,r;
    color(float blue,float green, float red):b(blue),g(green),r(red){}
};

//BGR
Color c_black(0, 0, 0);
Color c_red(0,0,1.0);
Color c_yellow(0,1.0,1.0);
Color c_green(0,1.0,0);
Color c_cyan(1.0,1.0,0);
Color c_blue(1,0,0);

Color paintColor[6] = {
    c_black,c_red,c_yellow,c_green,c_cyan,c_blue
};

/***************/
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

RenderWidget::RenderWidget(QWidget *parent) : QWidget(parent)
{
    edgeThresh = 1;
    lowThreshold = 0;
    ratio = 3;
    kernel_size = 3;

    step = 0;
    for(int i = 0;i < JOINTS_NUMBER;i++){
        pickPointArray[i] = new PickPoint(this);
//        connect(pickPointArray[i],SIGNAL(moved()) ,
//                this, SLOT(update()));
        pickPointArray[i]->setVisible(false);
    }
    handCtPoint = new PickPoint(this);
    handCenter = new Point(174,238);
    handCtPoint->setGeometry(handCenter->x - handCtPoint->width()/2,handCenter->y-handCtPoint->height()/2,handCtPoint->width(),handCtPoint->height());
}

QSize RenderWidget::minimumSizeHint() const
{
    return QSize(400, 400);
}

QSize RenderWidget::sizeHint() const
{
    return QSize(400, 400);
}

void RenderWidget::mousePressEvent(QMouseEvent *e){
    if(step >= JOINTS_NUMBER)return;
    PickPoint *point = handCtPoint;//pickPointArray[step];
    point->setVisible(true);
    point->setGeometry(e->x() - point->width()/2,e->y()-point->height()/2,point->width(),point->height());
    step++;
}

void RenderWidget::mouseMoveEvent(QMouseEvent *ev) {
    qDebug()<<"mouse move at"<<ev->x()<<ev->y();
}

void RenderWidget::keyPressEvent(QKeyEvent *e){
    qDebug()<<"key = ";
    QWidget::keyPressEvent(e);
}

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

double compared (const void * a, const void * b)
{
  return ( *(double*)a - *(double*)b );
}

//QStringList arguments = QCoreApplication::arguments();
////QString str = arguments.at(0);
int indexofImg = 1;
string folder_path;

void RenderWidget::paintEvent(QPaintEvent * /* event */)
{
    qDebug()<<"image gesture"<<indexofImg;
    string folder_path = "/Users/ebao/study/lab/Gesture/images/gesture" + to_string(indexofImg) + "/";

    Scalar color;
    RNG rng(12345);

    vector<Point> neighbor_points = { Point(-1,-1),Point(0,-1),Point(1,-1),Point(1,0),Point(1,1),Point(0,1),Point(-1,1),Point(-1,0) };

    Mat src = imread(folder_path+"gesture.jpg");
    Mat src_gray;
    cvtColor(src, src_gray,CV_RGB2GRAY);

    /// Reduce noise with a kernel 3x3

    Mat src_gray_inv = Mat(src_gray.rows, src_gray.cols, CV_8UC1,255) - src_gray;

    imwrite(folder_path + "src_gray_inv.png",src_gray_inv);

    Mat src_gray_inv_bw;
    threshold(src_gray_inv, src_gray_inv_bw, 50, 255, CV_THRESH_BINARY);  ///should be small

    //very important
    RemoveSmallRegion(src_gray_inv_bw, src_gray_inv_bw, 40, 0, 1);
    RemoveSmallRegion(src_gray_inv_bw, src_gray_inv_bw, 40, 0, 0);

    RemoveSmallRegion(src_gray_inv_bw, src_gray_inv_bw, 40, 1, 1);
    RemoveSmallRegion(src_gray_inv_bw, src_gray_inv_bw, 40, 1, 0);

    imwrite(folder_path + "src_gray_inv_bw.png",src_gray_inv_bw);

    /************ change to imread **************/

    ///binaryzation
    Mat bw = src_gray.clone();
    threshold(bw, bw, 200, 255, CV_THRESH_BINARY);

    imwrite(folder_path + "bw.png", bw);

//    /*******膨胀操作******/

    Mat bw_thinning = bw.clone();

    Mat bw_inv = Mat(bw.size(), CV_8UC1,255) - bw;

    imwrite(folder_path + "bw_inv.png", bw_inv);

    //remove small hole and cross
    RemoveSmallRegion(bw_inv, bw_inv, 80, 0, 1);
    RemoveSmallRegion(bw_inv, bw_inv, 80, 0, 0);

    RemoveSmallRegion(bw_inv, bw_inv, 80, 1, 1);
    RemoveSmallRegion(bw_inv, bw_inv, 80, 1, 0);

//    imshow("RemoveSmallRegion", bw_inv);
    imwrite(folder_path + "bw_RemoveHole.png",bw_inv);

    /// 腐蚀操作  very important
    int erosion_type = MORPH_RECT;
    int erosion_size = 1;
    Mat element = getStructuringElement( erosion_type,
                                         Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                         Point( erosion_size, erosion_size ) );

    erode(bw, bw, element );

    RemoveSmallRegion(bw, bw, 40, 1, 1);
    RemoveSmallRegion(bw, bw, 40, 1, 0);

//    RemoveSmallRegion(bw, bw, 20, 1, 1);
//    RemoveSmallRegion(bw, bw, 20, 1, 0);
//    imshow("bw", bw);


    Mat thin = bw_inv.clone();//src_gray_inv_bw.clone();
    thinning(thin, thin);

    imwrite(folder_path + "thin.png",thin);

    int dilation_type = MORPH_RECT;
    int dilation_size = 1;

    Mat element1 = getStructuringElement( dilation_type,
                                          Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                          Point( dilation_size, dilation_size ) );

    dilate( bw_inv, bw_inv, element1 );

    threshold(bw_inv, bw_inv, 128, 255, CV_THRESH_BINARY);

    //very important
    RemoveSmallRegion(bw_inv, bw_inv, 40, 0, 1);
    RemoveSmallRegion(bw_inv, bw_inv, 40, 0, 0);

    imwrite(folder_path + "bw_inv_erode.png",bw_inv);


/** here here here **/

    /***************** corners detection **********************/
    ///https://www.cnblogs.com/skyfsm/p/6899627.html

    Mat g_srcImage1;
    g_srcImage1 = thin.clone();

    //初始化 Shi-Tomasi algorithm的一些参数
    vector<Point2f> corners;
    int maxCorners = 36;
    double qualityLevel = 0.01;
    double minDistance = 5;
    int blockSize = 6; // adjusted by hand
    bool useHarrisDetector = true;
    double k = 0.04;
    goodFeaturesToTrack(thin,corners,maxCorners,qualityLevel,minDistance,Mat(),blockSize,useHarrisDetector,k);

    int r = 4;
    for (int i = 0; i < corners.size(); i++)
    {
//        g_srcImage1.at<uchar>(corners[i]) = 128;

        g_srcImage1.at<uchar>(corners[i]) = 128;
//        circle(g_srcImage1, corners[i], r, Scalar(225), -1, 8, 0);
    }

//    imshow("corners detection",g_srcImage1);
    imwrite(folder_path + "corner.png",g_srcImage1);
//    /***************************************/


    /***************change to read start *******************/

    /*******************convex*******************/
    /// blur
//    Mat src_gray_blur;
//    blur(src_gray_inv, src_gray_blur, Size(3,3));

    Mat threshold_output;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    ///binaryzation
    threshold(bw_inv, threshold_output, 80, 255, CV_THRESH_BINARY);  ///should be small

    imwrite(folder_path + "threshold_output.png",threshold_output);


//    Canny(threshold_output,threshold_output,0,10,3);

    ///contours     CV_RETR_TREE
    findContours(threshold_output, contours,hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    /// each contour calculate convex
    vector<vector<Point> >hull( contours.size() );
    for( int i = 0; i < contours.size(); i++ ){
        convexHull( Mat(contours[i]), hull[i], false );
    }

    /// 绘出轮廓及其凸包
//    RNG rng(12345);
    Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3);
    for( int i = 0; i< contours.size(); i++ )
    {
        color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        //         drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );//画轮廓
        drawContours( drawing, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point() );//画凸包
    }

    /// 把结果显示在窗体
    imwrite(folder_path + "drawing.png",drawing);

//    imshow( "Hull demo", drawing );  //400 * 400
    /***************************************/

    /************ flood fill *****************/
    int flags =  4 + (255 << 8) + CV_FLOODFILL_FIXED_RANGE + CV_FLOODFILL_MASK_ONLY;//标识符的0~7位为g_nConnectivity，8~15位为g_nNewMaskVal左移8位的值，16~23位为CV_FLOODFILL_FIXED_RANGE或者0。

    Mat dst = Mat::zeros( threshold_output.size(), CV_8UC3);  //400*400
    Rect ccomp;
    Mat maskImage(dst.rows + 2, dst.cols + 2, CV_8UC1, Scalar(0));   //402*402
    Mat drawing_gray;
    cvtColor(drawing, drawing_gray,CV_BGR2GRAY);
    maskImage(Range(1,drawing_gray.rows + 1),Range(1,drawing_gray.cols + 1)) = drawing_gray;   //row and col start from  0

    floodFill(drawing,maskImage,Point(0,0),Scalar(128,128,128),&ccomp, Scalar(25,25,25), Scalar(25,25,25),flags);

    Mat thinMaskImage(dst.rows + 2, dst.cols + 2, CV_8UC1, Scalar(0));
    thinMaskImage(Range(1,thin.rows + 1),Range(1,thin.cols + 1)) = thin;
    floodFill(thin,thinMaskImage,Point(0,0),Scalar(128,128,128),&ccomp, Scalar(25,25,25), Scalar(25,25,25),flags);
    Mat handContours(dst.rows + 2, dst.cols + 2, CV_8UC1, Scalar(0));
    handContours = thinMaskImage(Range(1,thin.rows + 1),Range(1,thin.cols + 1));
//    Mat black_Contours(handContours.rows, handContours.cols, CV_8UC1, Scalar(255));
//    Mat black_Contours_mini(handContours.rows - 4, handContours.cols - 4, CV_8UC1, Scalar(0));

//    black_Contours_mini.copyTo(black_Contours(Range(2,black_Contours_mini.rows + 2),Range(2,black_Contours_mini.cols + 2)));

//    handContours += black_Contours;

    imwrite(folder_path + "black_Contours.png",handContours);

    Mat mmm(handContours.size(),CV_8UC1,Scalar(255));
    Mat handContours_inv =  mmm - handContours;

    vector<vector<Point>> contours3;
    vector<Vec4i> hierarchy3;
    ///contours
    findContours(handContours_inv, contours3,hierarchy3,CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    Mat draw_handcontours = Mat::zeros( handContours_inv.size(), CV_8UC1);

    drawContours(draw_handcontours, contours3, 0, color, 1, 8, vector<Vec4i>(), 0, Point() );//画轮廓

    imwrite(folder_path + "handContours.png",draw_handcontours);



    double dist, maxdist = -1;

    Point center;  // center of hand
    for(int i = 0;i< draw_handcontours.cols;i++)
    {
        for(int j = 0;j< draw_handcontours.rows;j++)
        {

            dist = pointPolygonTest(contours3[0], Point(i,j),true);
            if(dist > maxdist)
            {
                maxdist = dist;
                center = Point(i,j);
            }
        }
    }

    cv::circle(draw_handcontours, center, maxdist, cv::Scalar(128),1,CV_AA);

    //双阈值   1.6
    cv::circle(draw_handcontours, center, maxdist * 1.67, cv::Scalar(255),1,CV_AA);// adjusted
    cv::circle(draw_handcontours, center, 2, cv::Scalar(255),1,CV_AA);// adjusted

    imwrite(folder_path + "draw_center.png",draw_handcontours);


    vector<vector<Point>> contours2;
    vector<Vec4i> hierarchy2;
    ///contours
    Mat maskImage_contours = maskImage(Range(1,maskImage.rows - 1),Range(1,maskImage.cols - 1));

    findContours(maskImage_contours, contours2,hierarchy2,CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
//    qDebug("maskImage_contours row %d", maskImage_contours.rows);

    /// each contour calculate convex
    /// contours2.size() ======= 1
    if(contours2.size() != 2)
        qDebug("error: found %d contours!", contours2.size());
    vector<vector<Point>>hull2( contours2.size() );
    convexHull( Mat(contours2[0]), hull2[0], false );

    /// 绘出轮廓及其凸包
//    RNG rng(12345);
    Mat contex_img = Mat::zeros( maskImage_contours.size(), CV_8UC1);
    color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
//    drawContours( contex_img, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );//画轮廓
    drawContours( contex_img, hull2, 0, Scalar(255), 1, 8, vector<Vec4i>(), 0, Point() );//画凸包

    flags =  4 + (255 << 8) + CV_FLOODFILL_FIXED_RANGE;//标识符的0~7位为g_nConnectivity，8~15位为g_nNewMaskVal左移8位的值，16~23位为CV_FLOODFILL_FIXED_RANGE或者0。

//    floodFill(contex_img,Point(1,1),Scalar(255),&ccomp, Scalar(25,25,25), Scalar(25,25,25),flags);

    vector<Mat> channels;
//    split(contex_img,channels);//分离色彩通道
    Mat imageBlueChannel;// = channels.at(0);
    Mat imageGreenChannel;// = channels.at(1);
    Mat imageRedChannel;// = channels.at(2);

//    imshow("thin_inv",thin_inv);

    /// 把结果显示在窗体
//    Mat out_contex = Mat::zeros( maskImage_contours.size(), CV_8UC3);

    vector<deque<Point>> out_contour_ines;
    findLines(contex_img,out_contour_ines);

    if(out_contour_ines.size() > 1)
        qDebug("error: more than one convex detected!");
    if(out_contour_ines.size() == 0)
        qDebug("error: no convex detected!");

    //thinning
//    imshow("inv",src_gray_inv);

    Mat g_srcImage2;
    g_srcImage2 = thin.clone();
    vector<Point> Tjunc;
    vector<Point2f> norm1;
    vector<Point2f> norm2;
    Mat thinRemoveT = thin.clone();
    TJunctionDetect(thinRemoveT, corners,Tjunc,norm1,norm2,14); //this r should be adjusted by the whole size of the image
    qDebug("junc.size = %d",Tjunc.size());
    imwrite(folder_path + "thinRemoveT.png",thinRemoveT);

    for (int i = 0; i < Tjunc.size(); i++)
    {
//        g_srcImage2.at<uchar>(Tjunc[i]) = 128;
        circle(g_srcImage2, Tjunc[i], 1, Scalar(128), -1, 8, 0);
    }

//    imshow("T junction detection",g_srcImage2);
    imwrite(folder_path + "corner2.png",g_srcImage2);

    Mat thin_inv = Mat(thinRemoveT.rows, thinRemoveT.cols, CV_8UC1,255) - thinRemoveT;
    /// 腐蚀操作  very important
    int erosion_type1 = MORPH_RECT;
    int erosion_size1 = 3;
    Mat element11 = getStructuringElement( erosion_type1,
                                         Size( 2*erosion_size1 + 1, 2*erosion_size1+1 ),
                                         Point( erosion_size1, erosion_size1 ) );

    erode(thin_inv,thin_inv,element11);

    RemoveSmallRegion(thin_inv, thin_inv, 10, 1, 1);
    RemoveSmallRegion(thin_inv, thin_inv, 10, 1, 0);

    imwrite(folder_path + "thin_inv.png",thin_inv);
    thinning(thin_inv, thin_inv);

    threshold(thin_inv, thin_inv, 128, 255, CV_THRESH_BINARY);


//    thin_inv -= imageGreenChannel;

//    Mat mm = thin_inv + thin;
//    imshow("thin_inv",mm);

    thin_inv -= handContours;

    imwrite(folder_path + "thin_bone_before.png",thin_inv);

    rectangle(thin_inv,Point(0,0),Point(thin_inv.cols - 1,thin_inv.rows - 1),Scalar(0),1);

    vector<Point> rectPt;

    for(int i = 0; i < thin_inv.cols; i ++){
        rectPt.push_back(Point(i,1));
        rectPt.push_back(Point(i,thin_inv.rows - 2));
    }

    for(int j = 0;j < thin_inv.rows;j ++){
        rectPt.push_back( Point(1,j));
        rectPt.push_back( Point (thin_inv.cols - 2,j));
    }

    for(int i = 0; i < rectPt.size(); i ++){
        if(thin_inv.at<uchar>(rectPt[i]) != 0){
            Point this_point = rectPt[i];
            thin_inv.at<uchar>(this_point) = 0;
            int this_flag = 0;
            Point next_point = this_point;
            int next_flag;
            while (findNextPoint(neighbor_points, thin_inv, this_point, this_flag, next_point, next_flag)){
                thin_inv.at<uchar>(this_point) = 255;
                thin_inv.at<uchar>(next_point) = 255;
                int found = 0;

                if(findTjunction(thin_inv,next_point,neighbor_points)){
                    thin_inv.at<uchar>(this_point) = 0;
                    found = 1;
                    break;
                }

                for(int n = 0; n < neighbor_points.size(); n ++){
                    Point tmpPt = next_point + neighbor_points[n];
                    if(findTjunction(thin_inv,tmpPt,neighbor_points)){
                        found = 2;
                        thin_inv.at<uchar>(this_point) = 0;
                        thin_inv.at<uchar>(next_point) = 0;
                        break;
                    }
                }
                if(found){
                    break;
                }
                thin_inv.at<uchar>(this_point) = 0;
                thin_inv.at<uchar>(next_point) = 0;
                this_point = next_point;
                this_flag = next_flag;
            }
        }

    }

    for(int j = 0;j < thin_inv.rows;j ++){
        Point tmpPt1(1,j);
        Point tmpPt2(thin_inv.cols - 2,j);
        if(thin_inv.at<uchar>(tmpPt1) != 0){
            thin_inv.at<uchar>(tmpPt1) = 0;
            Point this_point = tmpPt1;
            int this_flag = 0;
            Point next_point = this_point;
            int next_flag;
            while (findNextPoint(neighbor_points, thin_inv, this_point, this_flag, next_point, next_flag)){
                thin_inv.at<uchar>(this_point) = 255;
                thin_inv.at<uchar>(next_point) = 255;
                if(findTjunction(thin_inv,next_point,neighbor_points)){
                    thin_inv.at<uchar>(this_point) = 0;
                    break;
                }
                thin_inv.at<uchar>(this_point) = 0;
                thin_inv.at<uchar>(next_point) = 0;
                this_point = next_point;
                this_flag = next_flag;
                thin_inv.at<uchar>(this_point) = 0;
            }
        }

        if(thin_inv.at<uchar>(tmpPt2) != 0){
            thin_inv.at<uchar>(tmpPt2) = 0;
            Point this_point = tmpPt2;
            int this_flag = 0;
            Point next_point = this_point;
            int next_flag;
            while (findNextPoint(neighbor_points, thin_inv, this_point, this_flag, next_point, next_flag)){
                thin_inv.at<uchar>(this_point) = 255;
                thin_inv.at<uchar>(next_point) = 255;
                if(findTjunction(thin_inv,next_point,neighbor_points)){
                    thin_inv.at<uchar>(this_point) = 0;
                    break;
                }
                thin_inv.at<uchar>(this_point) = 0;
                thin_inv.at<uchar>(next_point) = 0;
                this_point = next_point;
                this_flag = next_flag;
                thin_inv.at<uchar>(this_point) = 0;
            }
        }
    }

    imwrite(folder_path + "thin_bone.png",thin_inv);

//    Mat thin_inv = imread(folder_path + "thin_inv.png",0);


    /*********** find joints **************/
    Mat imgJoints = thin.clone();
    cvtColor(imgJoints,imgJoints,CV_GRAY2BGR);
    vector<Point> crossPoints;
    for(int i = 0; i < Tjunc.size();i++){
        Point TjuncPoint = Tjunc[i];
        Point2f normVec = norm1[i];
        Point2f normVec2 = norm2[i];
        Point crossPt2;
        Point crossPt;

//        qDebug("point %d,%d ",TjuncPoint.x,TjuncPoint.y);

        Mat findCross_img = thin_inv.clone();

//        if(findXPoint(thin_inv,TjuncPoint,normVec2,crossPt2)){

//        };

        if(findXPoint(thin_inv,TjuncPoint,normVec,crossPt)){
            int mindist = (crossPt - TjuncPoint).dot(crossPt - TjuncPoint);
            Point this_point = crossPt;
            int this_flag = 0;
            Point next_point = this_point;
            int next_flag;
            int ccount = 0;
            Point tmpcrossPt = crossPt;
            while (findNextPoint(neighbor_points, findCross_img, this_point, this_flag, next_point, next_flag)){
                int curdist = (next_point - TjuncPoint).dot(next_point - TjuncPoint);
                this_point = next_point;
                this_flag = next_flag;
                if(curdist < mindist){
                    mindist = curdist;
                    tmpcrossPt = this_point;
                }else
                    break;
                if(ccount ++ > 3)
                    break;
            }
            ccount = 0;
            this_point = crossPt;
            while (findNextPoint(neighbor_points, findCross_img, this_point, this_flag, next_point, next_flag)){
                int curdist = (next_point - TjuncPoint).dot(next_point - TjuncPoint);
                this_point = next_point;
                this_flag = next_flag;
                if(curdist < mindist){
                    mindist = curdist;
                    tmpcrossPt = this_point;
                }else
                    break;
                if(ccount ++ > 3)
                    break;
            }
            crossPt = tmpcrossPt;
            crossPoints.push_back(crossPt);

            line(imgJoints,TjuncPoint,crossPt,Scalar(255,255,255));
//            imgJoints.at<uchar>(TjuncPoint) = Scalar(0,255,0)
            circle(imgJoints, TjuncPoint, 1, Scalar(0,255,0), -1, 8, 0);
            circle(imgJoints, crossPt, 1, Scalar(0,0,255), -1, 8, 0);
        }
        else{
            qDebug("error: point %d,%d found no cross",TjuncPoint.x,TjuncPoint.y);
            circle(imgJoints, TjuncPoint, 3, Scalar(255,128,0), 1, 8, 0);
        }
    }

    qDebug("%d joints found",crossPoints.size());

    /********** find wrast *********/

    Mat thin_inv_moveT = thin_inv.clone();

//    int cccc = 0;
    for(int i = 0; i < thin_inv_moveT.rows;i ++){

        for(int j = 0; j < thin_inv_moveT.cols;j ++){
            Point curPt(j,i);
            if(thin_inv_moveT.at<uchar>(curPt) == 255){
                if(findTjunction(thin_inv_moveT,curPt,neighbor_points)){
                    thin_inv_moveT.at<uchar>(curPt) = 0;
                    for(int ii = 0; ii < neighbor_points.size(); ii ++){
                        thin_inv_moveT.at<uchar>(curPt + neighbor_points[ii]) = 0;
                    }
//                    cccc++;
                }
            }
        }
    }

//    qDebug("%d Tjunctions found in clong",cccc);
    imwrite(folder_path + "thin_inv_moveT.png",thin_inv_moveT);


    /***************************************/


    vector<deque<Point>> lines;
    vector<deque<Point>> lines_ori;

//    imwrite(folder_path + "thin_bone.png",thin_inv);

    findLines(thin_inv, lines);
    lines_ori = lines;

    /*********** delete short lines *************/

    if(lines.size() > 3){
        int line_lenth[lines.size()];
        for(int i = 1; i < lines.size(); i ++){
            line_lenth[i] = lines[i].size();
        }
        qsort(line_lenth, lines.size(),sizeof(int),compare);
        float threshold = line_lenth[2] * 0.3;
        for(int i = 0; i < lines.size(); i ++){
            if(lines[i].size() < threshold){
                lines.erase(lines.begin() + i);
                i--;
            }
        }

    }

    /***************************************/

    imwrite(folder_path + "Contours.png",imgJoints);

    /*******************************/

    //draw lines
    Mat draw_img = Mat(src.rows, src.cols, CV_8UC3, Scalar(0,0,0));
    Mat draw_img_gray = Mat(src.rows, src.cols, CV_8UC1, Scalar(0));

    int count_line = 0;
    for (int i = 0; i < lines.size(); i++)
    {
        count_line ++;
        color = Scalar( rng.uniform(50, 255), rng.uniform(50,255), rng.uniform(50,255) );
        for (int j = 0; j < lines[i].size(); j++)
        {
            draw_img.at<Vec3b>(lines[i][j]) = Vec3b(color[0], color[1], color[2]);
            draw_img_gray.at<uchar>(lines[i][j]) = 255;
        }
    }

//    vector<Mat> channels;
    split(draw_img,channels);//分离色彩通道
    imageBlueChannel = channels.at(0);
    imageGreenChannel = channels.at(1);
    imageRedChannel = channels.at(2);

    imageBlueChannel += thin;
    imageGreenChannel += thin;
    imageRedChannel += thin;

    merge(channels,draw_img);
    imwrite(folder_path + "draw_img.png",draw_img);


    Mat draw_img_all = Mat(src.rows, src.cols, CV_8UC3, Scalar(0,0,0));
    Mat draw_img_gray_all = Mat(src.rows, src.cols, CV_8UC1, Scalar(0));

    for (int i = 0; i < lines_ori.size(); i++)
    {
        color = Scalar( rng.uniform(50, 255), rng.uniform(50,255), rng.uniform(50,255) );
        for (int j = 0; j < lines_ori[i].size(); j++)
        {
            draw_img_all.at<Vec3b>(lines_ori[i][j]) = Vec3b(color[0], color[1], color[2]);
            draw_img_gray_all.at<uchar>(lines_ori[i][j]) = 255;
        }
    }

//    vector<Mat> channels;
    split(draw_img_all,channels);//分离色彩通道
    imageBlueChannel = channels.at(0);
    imageGreenChannel = channels.at(1);
    imageRedChannel = channels.at(2);

    imageBlueChannel += thin;
    imageGreenChannel += thin;
    imageRedChannel += thin;

    merge(channels,draw_img_all);
    imwrite(folder_path + "draw_img_all.png",draw_img_all);
    /*******************************/


    /****************find finger ends********************/
    //detect end and start of each line
    Mat largeCircle(draw_img_gray.rows, draw_img_gray.cols,CV_8UC1, Scalar(0));
    Mat midCircle(draw_img_gray.rows, draw_img_gray.cols,CV_8UC1, Scalar(0));

    float threshold1 = 1.6, threshold2 = 1.05;

    int x1,y1,d1,r1,x2,y2,d2,r2;
    r1 = maxdist * threshold1;
    r2 = maxdist * threshold2;
    x1 = 0;
    y1 = r1;
    d1 = 1-r1;
    x2 = 0;
    y2 = r2;
    d2 = 1-r2;


    int x = x1,y = y1;
    while(x <= y){
        vector<Point> circle_points = {Point(x,y),Point(y,x),Point(y,-x),Point(x,-y),Point(-x,-y),Point(-y,-x),Point(-y,x),Point(-x,y)};
        for(int i = 0; i < circle_points.size(); i++){
            largeCircle.at<uchar>(center + circle_points[i]) = 100;
            midCircle.at<uchar>(center + circle_points[i]) = 100;
        }
        if(d1 < 0){
            d1 += 2*x + 3;
        }else{
            d1 += 2 * ( x - y ) + 5;
            y --;
        }
        x ++;
    }

    x = x2, y = y2;
    while(x <= y){
        vector<Point> circle_points = {Point(x,y),Point(y,x),Point(y,-x),Point(x,-y),Point(-x,-y),Point(-y,-x),Point(-y,x),Point(-x,y)};
        for(int i = 0; i < circle_points.size(); i++){
            midCircle.at<uchar>(center + circle_points[i]) = 100;
        }
        if(d2 < 0){
            d2 += 2*x + 3;
        }else{
            d2 += 2 * ( x - y ) + 5;
            y --;
        }
        x ++;
    }
//    flags = 8 + (255 << 8) + CV_FLOODFILL_FIXED_RANGE;//标识符的0~7位为g_nConnectivity，8~15位为g_nNewMaskVal左移8位的值，16~23位为CV_FLOODFILL_FIXED_RANGE或者0。

    floodFill(largeCircle,center,Scalar(255),&ccomp, Scalar(25), Scalar(25));

    floodFill(midCircle,Point(0,0),Scalar(255),&ccomp, Scalar(25), Scalar(25));
    floodFill(midCircle,center,Scalar(255),&ccomp, Scalar(25), Scalar(25));

    imwrite(folder_path + "largeCircle_flood.png",largeCircle);

    imwrite(folder_path + "midCircle_flood.png",midCircle);

//    cv::circle(largeCircle, center, r1, cv::Scalar(255),-1,CV_AA);// adjusted
//    cv::circle(midCircle, center, r1 + 1, cv::Scalar(0),-1,CV_AA);// adjusted
//    cv::circle(midCircle, center, r2 - 1, cv::Scalar(255),-1,CV_AA);// adjusted

    threshold(largeCircle,largeCircle,128,255,THRESH_BINARY);
    threshold(midCircle,midCircle,128,255,THRESH_BINARY);

    largeCircle = thin_inv - largeCircle;
    midCircle = thin_inv - midCircle;

    Mat finger_ends_image = thin_inv.clone();

    Mat largeCircle_b = largeCircle.clone();

    vector<Point> finger_ends;


    x = x1,y = y1,d1 = 1-r1;

    vector<Point> valid_Points_bc;
    while(x <= y){
        vector<Point> circle_points = {Point(x,y),Point(y,x),Point(y,-x),Point(x,-y),Point(-x,-y),Point(-y,-x),Point(-y,x),Point(-x,y)};
        for(int i = 0; i < circle_points.size(); i++){
            Point tmpPoint = center + circle_points[i];
            largeCircle_b.at<uchar>(tmpPoint) = 128;
            if(largeCircle.at<uchar>(tmpPoint) == 255){
                valid_Points_bc.push_back(tmpPoint);
            }else{
                for(int j = 0;j < neighbor_points.size(); j ++){
                    Point ttmpPoint = tmpPoint + neighbor_points[j];
                    if(largeCircle.at<uchar>(ttmpPoint) == 255){
                        valid_Points_bc.push_back(tmpPoint);
                    }
                }
            }
        }
        if(d1 < 0){
            d1 += 2*x + 3;
        }else{
            d1 += 2 * ( x - y ) + 5;
            y --;
        }
        x ++;
    }

    for(int i = 0; i < valid_Points_bc.size(); i ++){
//        vector<Point> circle_points = {Point(x,y),Point(y,x),Point(y,-x),Point(x,-y),Point(-x,-y),Point(-y,-x),Point(-y,x),Point(-x,y)};
//        for(int i = 0; i < circle_points.size(); i++){

        Point tmpPoint = valid_Points_bc[i];

            if(largeCircle.at<uchar>(tmpPoint) == 255){
                largeCircle.at<uchar>(tmpPoint) = 0;
                //deep search
                Point this_point = tmpPoint;//center + circle_points[i];
                Point next_point;
                int counti;
                int dist = findFarPoint(neighbor_points,largeCircle,this_point,next_point,counti);

//                qDebug("counti ===== %d",counti);
                //                if(counti < 2){
                finger_ends.push_back(next_point);
//                cv::circle(finger_ends_image, next_point, 10, cv::Scalar(255),-1,CV_AA);// adjusted

                this_point = tmpPoint;//center + circle_points[i];
                for(int ii = 0; ii < neighbor_points.size(); ii++){
                    Point tmppt = this_point + neighbor_points[ii];
                    if(midCircle.at<uchar>(tmppt) == 255){
                        midCircle.at<uchar>(tmppt) = 0;
                        findFarPoint(neighbor_points,midCircle,this_point,next_point,counti);
                    }
                }
                //                }
            }else{
//                Point this_point = tmpPoint;
//                Point next_point;
//                int counti;
//                findFarPoint(neighbor_points,midCircle,this_point,next_point,counti);
//                break;
                for(int j = 0;j < neighbor_points.size(); j ++){
                    Point ttmpPoint = tmpPoint + neighbor_points[j];
                    if(largeCircle.at<uchar>(ttmpPoint) == 255){
                        largeCircle.at<uchar>(ttmpPoint) = 0;
                        Point this_point = ttmpPoint;
                        Point next_point;
                        int counti;
                        int dist = findFarPoint(neighbor_points,largeCircle,this_point,next_point,counti);
//                        qDebug("counti ===== %d",counti);

//                        if(counti < 3){
//                        cv::circle(finger_ends_image, next_point, 10, cv::Scalar(255),-1,CV_AA);// adjusted
                        finger_ends.push_back(next_point);

                        break;
//                        }
                    }
                }
                Point this_point = tmpPoint;
                Point next_point;
                int counti;
                findFarPoint(neighbor_points,midCircle,this_point,next_point,counti);
            }

//        }



    }

    Mat midCircle_b = midCircle.clone();

    x = x2,y = y2,d2 = 1- r2;

    while(x <= y){
        vector<Point> circle_points = {Point(x,y),Point(y,x),Point(y,-x),Point(x,-y),Point(-x,-y),Point(-y,-x),Point(-y,x),Point(-x,y)};
        for(int i = 0; i < circle_points.size(); i++){
            midCircle_b.at<uchar>(center + circle_points[i]) = 128;

            if(midCircle.at<uchar>(center + circle_points[i]) == 255){
                midCircle.at<uchar>(center + circle_points[i]) = 0;
                //deep search
                Point this_point = center + circle_points[i];
                Point next_point;
                int counti;
                int dist = findFarPoint(neighbor_points,midCircle,this_point,next_point,counti);

                finger_ends.push_back(next_point);
//                cv::circle(finger_ends_image, next_point, 2, cv::Scalar(255),-1,CV_AA);// adjusted

            }else{
                for(int j = 0;j < neighbor_points.size(); j ++){
                    Point ttmpPoint = center + circle_points[i] + neighbor_points[j];
                    if(midCircle.at<uchar>(ttmpPoint) == 255){
                        Point this_point = ttmpPoint;
                        Point next_point;
                        int counti;
                        int dist = findFarPoint(neighbor_points,midCircle,this_point,next_point,counti);

                        finger_ends.push_back(next_point);
                }
            }
            }
        }

        if(d2 < 0){
            d2 += 2*x + 3;
        }else{
            d2 += 2 * ( x - y ) + 5;
            y --;
        }
        x ++;
    }

    vector<deque<Point>> finger_lines_far;
    findLines(largeCircle, finger_lines_far);
//    qDebug("finger_lines_far.siez = %d",finger_lines_far.size());

    for(int i = 0;i < finger_lines_far.size(); i ++){
        deque<Point> line = finger_lines_far[i];
        if(line.size() < 10)
            continue;

        Point pt1 = *line.begin();
        Point pt2 = *(line.end() - 1);

        int dist1 = (pt1 - center).dot(pt1 - center);
        int dist2 = (pt2 - center).dot(pt2 - center);

        if(dist1 > dist2){
            finger_ends.push_back(pt1);
        }else{
            finger_ends.push_back(pt2);
        }
    }

    Mat finger_ends_imagecl = finger_ends_image.clone();

    for(int i = 0; i < finger_ends.size(); i ++){
        cv::circle(finger_ends_imagecl, finger_ends[i], 5, cv::Scalar(255),1,CV_AA);// adjusted
    }
    imwrite(folder_path + "finger_ends_imagecl.png",finger_ends_imagecl);


    imwrite(folder_path + "largeCircle_b.png",largeCircle_b);
    imwrite(folder_path + "midCircle_b.png",midCircle_b);

    double rad[finger_ends.size()];

    for(int i = 0;i < finger_ends.size();i ++){
        Point dir = finger_ends[i] - center;
        rad[i] = acos(dir.ddot(Point(1,0))/ sqrt(dir.ddot(dir)));
        if(dir.y < 0)
            rad[i] = 2 * CV_PI - rad[i];

//        qDebug("rad %d = %f",i,rad[i]);
    }

    if(finger_ends.size() > 1){
        int idx1, idx2;
        double rad1,rad2;
        double mindistsum = 10000;
        vector<Point> pts1;
        vector<Point> pts2;
        for(int i = 0;i < finger_ends.size();i ++){
            rad1 = rad[i];
            vector<Point> tmppts1;
            vector<Point> tmppts2;
            tmppts1.push_back(finger_ends[i]);
            for(int j = 0;j < finger_ends.size();j ++){
                if( i ==j )
                    continue;
                rad2 = rad[j];
                double dist1 = fabs(rad2 - rad1);
                dist1 = dist1 > CV_PI ? 2 * CV_PI - dist1:dist1;
                if(dist1 < CV_PI * 0.25){
                    tmppts1.push_back(finger_ends[j]);
                }else{
                    tmppts2.push_back(finger_ends[j]);
                }
            }
            if(tmppts1.size() > pts1.size()){
                pts1 = tmppts1;
                pts2 = tmppts2;
            }


//            for(int j = 0;j < i;j ++){
//                rad2 = rad[j];
//                double distsum2 = 0;
//                double distsum1 = 0;
//                vector<Point> tmppts1;
//                vector<Point> tmppts2;

//                for(int k = 0;k < finger_ends.size();k++){
//                    if(k != i && k != j){
//                        double tmprad = rad[k];

//                        double dist1 = fabs(tmprad - rad1);
//                        dist1 = dist1 > CV_PI ? 2 * CV_PI - dist1:dist1;
//                        double dist2 = fabs(tmprad - rad2);
//                        dist2 = dist2 > CV_PI ? 2 * CV_PI - dist2:dist2;

//                        if(dist1 > dist2){
//                            distsum2 += dist2;
//                            tmppts2.push_back(finger_ends[k]);
//                        }else{
//                            distsum1 += dist1;
//                            tmppts1.push_back(finger_ends[k]);
//                        }
//                    }
//                }
//                double tmpdistsum = distsum1 + distsum2;
//                if(tmpdistsum < mindistsum){
//                    mindistsum = tmpdistsum;
//                    idx1 = i;
//                    idx2 = j;
//                    pts1 = tmppts1;
//                    pts2 = tmppts2;
//                }
//            }
        }

        for(int i = 0;i < pts1.size();i++){
            cv::circle(finger_ends_image, pts1[i], 10, cv::Scalar(255),1,CV_AA);// adjusted
        }


        for(int i = 0;i < pts2.size();i++){
            cv::circle(finger_ends_image, pts2[i], 6, cv::Scalar(255),1,CV_AA);// adjusted
        }

//        pts1.push_back(finger_ends[idx1]);
//        pts2.push_back(finger_ends[idx2]);

//        cv::circle(finger_ends_image, finger_ends[idx1], 10, cv::Scalar(255),-1,CV_AA);// adjusted
//        cv::circle(finger_ends_image, finger_ends[idx2], 6, cv::Scalar(255),-1,CV_AA);// adjusted

        vector<Point> pts;
        finger_ends.clear();

        if(pts1.size() > pts2.size()){
            pts = pts1;
        }else{
            pts = pts2;
        }

        int end_dist[pts.size()];
//        if(pts.size() > 5){
            for(int i = 0; i < pts.size();i ++){
                Point pp(pts[i] - center);
                end_dist[i] = pp.dot(pp);
            }
//        }
        qsort(end_dist, pts.size(),sizeof(int),compare);  //from big to small

        for(int i = 0; i < pts.size();i ++){
            Point pp(pts[i] - center);
            double dd = pp.ddot(pp);
            int idx = pts.size() > 5? pts.size() - 5:0;
            if(dd >= end_dist[idx]){
                cv::circle(finger_ends_image, pts[i], 2, cv::Scalar(255),1,CV_AA);// adjusted
                finger_ends.push_back(pts[i]);
            }
        }
//        finger_ends = pts;
//        qDebug("rank = %f,%f",end_dist[0],end_dist[pts.size() - 1]);
        qDebug("fingerends.sze === %d",finger_ends.size());
    }

    imwrite(folder_path + "largeCircle.png",largeCircle);
    imwrite(folder_path + "midCircle.png",midCircle);
    imwrite(folder_path + "finger_ends_image.png",finger_ends_image);

    /***************************************/


    /***************** find line and joints **********************/
//    vector<Point> fingerEnds_onLine[lines.size()];

    vector<vector<Point>> points_onlines;
    vector<vector<int>> points_ol_idx;

    for(int i = 0; i < lines.size(); i ++){
        vector<Point> pts;
        vector<int> idxs;
        points_onlines.push_back(pts);
        points_ol_idx.push_back(idxs);
    }

    Mat finger_img = thin.clone();

    int fingerends_ol_idx[finger_ends.size()];

    for(int i = 0; i < finger_ends.size(); i++){
        Point pt = finger_ends[i];
        int found = 0;
        for(int j = 0; j < lines.size(); j ++){
            Point pp[2] = {Point(*(lines[j].begin())),Point(*(lines[j].end() - 1))};
            for(int k = 0; k < 2;k++){
                Point p = pp[k];
                if(p == pt){
                    found = 1;
                }else{
                    for(int ii = 0;ii < neighbor_points.size(); ii ++){
                        Point tmppoint = pt + neighbor_points[ii];
                        if(p == tmppoint){
                            found = 1;
                            break;
                        }
                    }
                }
                if(found){
                    if(points_onlines[j].size() != 0){
                        qDebug("error: found %d ends on 1 line",points_onlines[j].size() + 1);
                        int halfIdx = lines[j].size() / 2;
                        deque<Point> halfline1(lines[j].begin(),lines[j].begin() + halfIdx - 1);
                        deque<Point> halfline2(lines[j].begin() + halfIdx + 1,lines[j].end());
                        lines[j] = halfline1;
                        if(k == 1){
                            reverse(halfline2.begin(),halfline2.end());
                        }else{
                            qDebug()<<"error: 2 ends not on the end";
                        }
                        lines.push_back(halfline2);
                        vector<Point> pts;
                        vector<int> inds;
                        pts.push_back(p);
                        inds.push_back(0);
                        points_onlines.push_back(pts);
                        points_ol_idx.push_back(inds);
                        qDebug()<<"lines = "<<lines.size()\
                            <<"points online = "<<points_onlines.size()\
                           <<"points_ol_idx = "<<points_ol_idx.size();
                        break;
                    }
                    points_onlines[j].push_back(p);
                    points_ol_idx[j].push_back(0);
                    if(k == 1)
                        reverse(lines[j].begin(),lines[j].end());
                    break;
                }
            }
            if(found){
                fingerends_ol_idx[i] = j;
                break;
            }
        }
    }


    Point fakeEnds(-1, -1);

    for(int i = 0; i < crossPoints.size(); i++){
        Point pt = crossPoints[i];
        int found = 0;
        for(int j = 0; j < lines.size(); j ++){
            for(int k = 0; k < lines[j].size();k++){
                Point p = lines[j][k];
                if(p == pt){
                    if(points_onlines[j].size() == 0){
                        points_onlines[j].push_back(fakeEnds);
                        points_ol_idx[j].push_back(-1);
                    }
                    points_onlines[j].push_back(p);
                    points_ol_idx[j].push_back(k);
                    found = 1;
                    break;
                }else{
                    for(int ii = 0;ii < neighbor_points.size(); ii ++){
                        Point tmppoint = pt + neighbor_points[ii];
                        if(p == tmppoint){
                            if(points_onlines[j].size() == 0){
                                points_onlines[j].push_back(fakeEnds);
                                points_ol_idx[j].push_back(-1);
                            }
                            points_onlines[j].push_back(p);
                            points_ol_idx[j].push_back(k);
                            found = 1;
                            break;
                        }
                    }
                }
                if(found)
                    break;
            }
            if(found)
                break;
        }
    }

    vector<deque<Point>> fingerLines;
    vector<vector<Point>> fingerJoints;
    vector<vector<int>> fingerJointsIdx;

    for(int i = 0; i < lines.size(); i ++){
        if(points_ol_idx[i].size() > 0){
            fingerLines.push_back(lines[i]);
            fingerJoints.push_back(points_onlines[i]);
            fingerJointsIdx.push_back(points_ol_idx[i]);
        }
    }

    qDebug()<<"valid fingers = "<<fingerJoints.size();


//    for(int i = 0; i < bonelenth.size(); i ++){
//        qDebug("bonelenth %d = %d",i,bonelenth[i]);
//    }

    /************* find finger root *********/

//    Mat mmm = finger_img.clone();

    
    for(int i = 0; i < fingerJointsIdx.size(); i ++){
        vector<int> Idx_i = fingerJointsIdx[i];
        qDebug()<<"fingerJointsIdx = "<<i;
        for(int j = 0; j < Idx_i.size(); j ++){
            int tmpidx = Idx_i[j];
            qDebug()<<" idx = "<<tmpidx;

            for(int k = 0; k < j; k++){
                if(tmpidx < Idx_i[k]){
                    Idx_i[j] = Idx_i[k];
                    Idx_i[k] = tmpidx;
                    Point pp = fingerJoints[i][j];
                    fingerJoints[i][j] = fingerJoints[i][k];
                    fingerJoints[i][k] = pp;
                    int ii = fingerJointsIdx[i][j];
                    fingerJointsIdx[i][j] = fingerJointsIdx[i][k];
                    fingerJointsIdx[i][k] = ii;
                    break;
                }
            }
        }
    }

    /************* general joint len *********/
    vector<int> bonelenth;

    Mat bonelenth_img = src.clone();

    for(int i = 0; i < fingerJointsIdx.size(); i ++){
        vector<int> Idx_i = fingerJointsIdx[i];
        for(int j = 0; j < Idx_i.size() - 1; j ++){
            int tmpI = Idx_i[j];
            if(tmpI == -1)
                continue;
            int nextI = Idx_i[j + 1];
            qDebug()<<"i ="<<i<<"tmpI = "<<tmpI<<"nextI ="<<nextI;

            Point pp = (fingerLines[i][tmpI] - fingerLines[i][nextI]);
            int len = int(sqrt(pp.dot(pp)));
            color = random_color(rng);
            cv::line(bonelenth_img, fingerLines[i][tmpI],fingerLines[i][nextI], color,1,CV_AA);
            qDebug("bonelenth %d = %d",bonelenth.size(), len);

            bonelenth.push_back(len);
        }
    }

    sort(bonelenth.begin(),bonelenth.end(),less<int>()); //increase

    imwrite(folder_path + "bonelenth.png",bonelenth_img);

    int refer_len = bonelenth[bonelenth.size() / 2];

    double len_threshold = 0.2;
    vector<Point> finger_roots;
    int maxCount = 0;
    int maxIdx = 0;
    int maxSum = 0;
    for(int i = 0; i < bonelenth.size();i ++){
        int curlen = bonelenth[i];
        int count = 1;
        int sum = curlen;
//        vector<int> cur_valid_len;
        for(int j = 0; j < bonelenth.size(); j ++){
            if(i == j)
                continue;
            int dist = abs(curlen - bonelenth[j]);
            if(dist < curlen * len_threshold){
                count ++;
                sum += bonelenth[j];
            }
        }
//        qDebug("bonelenth [%d]  = %d, count = %d",i,bonelenth[i],count);
        if(count > maxCount){
            maxCount = count;
            maxIdx = i;
            maxSum = sum;
        }
    }

    double adist = (double)maxSum / (double)maxCount;
    qDebug("avr bonelenth = %f",adist);

    qDebug()<<"fingerJoints.size ="<<fingerJoints.size();

    for(int i = 0; i < fingerJoints.size(); i ++){

        Mat jjj = thin_inv.clone();
        cv::circle(jjj, fingerJoints[i][0],5, cv::Scalar(225),1,CV_AA);// adjusted
        char ic = '0' + i;
        imwrite(folder_path + "jjj"+ic+".png",jjj);

        qDebug("finger %d_______  fingerJoints = %d",i,fingerJoints[i].size());


        if(fingerJoints[i][0] == fakeEnds && fingerJoints[i].size() > 3){
        }

        if(fingerJoints[i][0] != fakeEnds && fingerJoints[i].size() == 1){
            qDebug("finger 1 found!");
            Point end = fingerJoints[i][0];
            int endIdx = fingerJointsIdx[i][0];
            Point pt1 = *(fingerLines[i].end() - 1);
            int found = 0;
            for(int j = fingerJointsIdx[i][0] + int(adist - 1); j < fingerLines[i].size();j++){
                pt1 = fingerLines[i][j];
                double ddist = sqrt((end - pt1).dot(end - pt1));
                if(fabs(ddist - adist) < adist * 0.2f){
                    found = 1;
                    fingerJoints[i].push_back(pt1);
                    fingerJointsIdx[i].push_back(j);
                    break;
                }
            }
            if(!found){
                qDebug("pt1 not found!!!!");
                pt1 = *(fingerLines[i].end() - 1);
                int idx1 = fingerLines[i].size();
                double ddist = sqrt((pt1 - end).dot(pt1 - end));
                if(fabs(ddist - adist) < adist * 0.2f && fabs(idx1 - endIdx)){
                    found = 1;
                    fingerJoints[i].push_back(pt1);
                    fingerJointsIdx[i].push_back(fingerLines[i].size() - 1);
                }
            }
            if(found){
                qDebug("pt1 found!!!!");
                cv::circle(finger_img, pt1,3, cv::Scalar(128),1,CV_AA);// adjusted
//                    bonelenth.push_back(adist);
//                    finger_roots.push_back(root);
            }
        }

        if(fingerJoints[i][0] != fakeEnds && fingerJoints[i].size() == 2){
            qDebug("finger 2 found!");
            Point end = fingerJoints[i][0], pt1 = fingerJoints[i][1];
            double dist1 = sqrt((end - pt1).dot(end - pt1));
            double aadist = (adist + dist1) * 0.5;

            if(fabs(dist1 - aadist) < aadist * 0.2){
                Point pt2 = *(fingerLines[i].end() - 1);
                int found = 0;
                qDebug()<<"start from "<<fingerJointsIdx[i][1]<<"+"<<int(adist - 1)<<"to end ="<<fingerLines[i].size();
                for(int j = fingerJointsIdx[i][1] + int(adist - 1); j < fingerLines[i].size();j++){
                    pt2 = fingerLines[i][j];
                    double ddist = sqrt((pt1 - pt2).dot(pt1 - pt2));
                    if(fabs(ddist - aadist) < aadist * 0.2f){
                        found = 1;
                        fingerJoints[i].push_back(pt2);
                        fingerJointsIdx[i].push_back(j);
                        break;
                    }
                }
                if(!found){
                    qDebug("pt2 not found!!!!");
                    pt2 = *(fingerLines[i].end() - 1);
                    double ddist = sqrt((pt2 - pt1).dot(pt2 - pt1));
                    if(fabs(ddist - aadist) < aadist * 0.2f && fingerLines[i].size() - fingerJointsIdx[i][1] < aadist * 2.0f){
                        found = 1;
                        fingerJoints[i].push_back(pt2);
                        fingerJointsIdx[i].push_back(fingerLines[i].size() - 1);
                    }
                }
                if(found){
                    qDebug("pt2 found!!!!");
                    cv::circle(finger_img, pt2,8, cv::Scalar(128),1,CV_AA);// adjusted
//                    bonelenth.push_back(aadist);
//                    finger_roots.push_back(root);
                }
            }else if(fabs(dist1 - 2 * adist) < adist * 0.3){
                int idx0 = fingerJointsIdx[i][0], idx2 = fingerJointsIdx[i][1];
                int idx1 = (idx0 + idx2)/2;
                Point pt1 = fingerLines[i][idx1];
                fingerJoints[i].insert(fingerJoints[i].begin() + 1, fingerLines[i][idx1]);
                fingerJointsIdx[i].insert(fingerJointsIdx[i].begin() + 1,idx1);
                cv::circle(finger_img,pt1 ,8, cv::Scalar(128),1,CV_AA);// adjusted
            }
        }

        // 3 found
        if(fingerJoints[i][0] != fakeEnds && fingerJoints[i].size() == 3){
            qDebug("finger 3 found!");
            Point end = fingerJoints[i][0], pt1 = fingerJoints[i][1], pt2 = fingerJoints[i][2];
            double dist1 = sqrt((end - pt1).dot(end - pt1));
            double dist2 = sqrt((end - pt2).dot(end - pt2));
            if(dist2 < dist1){
                Point tmppt = pt1;
                pt1 = pt2;
                pt2 = tmppt;
                dist1  = dist2;
            }
            //end - pt1 - pt2
            double dist01 = dist1;
            double dist12 = sqrt((pt2 - pt1).dot(pt2 - pt1));
            int found = 0;
            double aadist = dist01 * 0.3 + dist12 * 0.3 + adist * 0.4;

            if(fabs(dist01 - dist12) < max(dist01,dist12) * 0.5){  //adjust
                qDebug("match");
                Point root = *(fingerLines[i].end() - 1);
                for(int j = fingerJointsIdx[i][2] + int(aadist/2); j < fingerLines[i].size();j++){
                    root = fingerLines[i][j];
                    double ddist = sqrt((pt2 - root).dot(pt2 - root));
                    if(fabs(ddist - aadist) < 1.0f){
                        found = 1;
                        fingerJoints[i].push_back(root);
                        fingerJointsIdx[i].push_back(j);
                        break;
                    }
                }
                if(!found){
                    qDebug("root not found!!!!");
                    root = *(fingerLines[i].end() - 1);
                    double ddist = sqrt((pt2 - root).dot(pt2 - root));
                    if(fabs(ddist - adist) < adist * 0.4f && fingerLines[i].size() - fingerJointsIdx[i][2] < adist * 2.0f){
                        found = 1;
                        fingerJoints[i].push_back(root);
                        fingerJointsIdx[i].push_back(fingerLines[i].size() - 1);
                    }
                }
                if(found){
                    qDebug("root found!!!!");
                    cv::circle(finger_img, root,10, cv::Scalar(225),1,CV_AA);// adjusted
//                    bonelenth.push_back(aadist);
                    finger_roots.push_back(root);
                }
            }else if(fabs(dist01 - dist12) < max(dist01,dist12) * 0.7){  //adjust
                qDebug("over match");
                Point root = *(fingerLines[i].end() - 1);
                root = *(fingerLines[i].end() - 1);
                double ddist = sqrt((pt2 - root).dot(pt2 - root));
                if(fabs(ddist - adist) < adist * 0.2f && fingerLines[i].size() - fingerJointsIdx[i][2] < adist * 2.0f){
                    found = 1;
                    fingerJoints[i].push_back(root);
                    fingerJointsIdx[i].push_back(fingerLines[i].size() - 1);
                    finger_roots.push_back(root);
                    cv::circle(finger_img, root,10, cv::Scalar(225),1,CV_AA);// adjusted
                    qDebug("root found!!!!");
                }
            }
        }
    }

    for(int i = 0; i < fingerLines.size(); i ++){
        for(int j = 0; j < fingerLines[i].size(); j ++){
            finger_img.at<uchar>( fingerLines[i][j]) = 255;
        }
    }

    for(int i = 0;i < fingerJoints.size();i++){
        for(int j = 0; j < fingerJointsIdx[i].size(); j ++){
            if( fingerJoints[i][j] == fakeEnds || fingerJointsIdx[i][j] == -1)
                continue;
            if(fingerJointsIdx[i][j] < 0){
                qDebug("error: fingerJointsIdx[i][j]  == %d",fingerJointsIdx[i][j]);
                continue;
            }
            if(fingerJointsIdx[i][j] >= fingerLines[i].size()){
                qDebug("error:fingerJointsIdx[i][j] %d > fingerLines[i].size() %d",fingerJointsIdx[i][j],fingerLines[i].size());
                continue;
            }
            Point tmppoint = fingerLines[i][fingerJointsIdx[i][j]];
            if (tmppoint.x > 0 && tmppoint.y > 0 && tmppoint.x < finger_img.cols&&tmppoint.y < finger_img.rows){
                finger_img.at<uchar>(tmppoint) = 128;
                int size = 2 * j + 2;
//                switch(j){
//                    case 0:
//                }

                cv::circle(finger_img, tmppoint,size, cv::Scalar(255),1,CV_AA);// adjusted
            }else{
                qDebug("error: col rows = %d %d, points = %d, %d",finger_img.cols, finger_img.rows,tmppoint.x,tmppoint.y);
            }
        }
    }

    imwrite(folder_path + "finger_img.png",finger_img);
    imwrite("/Users/ebao/study/lab/Gesture/images/gestures_pre/finger_img" + to_string(indexofImg) + ".png",finger_img);

    /***************************************/

//    QImage img = cvMat2QImage(src_gray);

    QImage img = cvMat2QImage(src_gray);

    QPainter painter(this);
    painter.drawImage(QRect(0, 0, width(), height()), img);
    painter.end();

    indexofImg ++;
    qDebug()<<"_________________________finished__________________________";
    if(indexofImg < 9)
        update();

//    cv::waitKey(0);

//    emit changed();
}

void RenderWidget::setCannyPara(int v){
    lowThreshold = v;
    update();
}


void RenderWidget::readFile()
{
    /********
    QFile f("test.txt");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Open failed.";
        return;
    }

    QTextStream txtInput(&f);
    QString lineStr;
    lineStr = txtInput.readLine();
    QList<QString> lst;
    lst=lineStr.split('\t');
    float x = lst[0].toInt();
    float y = lst[1].toInt();

    PickPoint *p0 = pickPointArray[0];
    p0->setGeometry(x,y,p0->width(),p0->height());
    p0->setVisible(true);

    int i = 0;
    while(!txtInput.atEnd())
    {
        lineStr = txtInput.readLine();
        lst=lineStr.split('\t');
        for(int j = 0;j < 4;j ++){
            PickPoint *p = pickPointArray[1+i * 4 + j];
            int x = lst[j * 2].toInt();
            int y = lst[j * 2 + 1].toInt();
            p->setGeometry(x,y,p->width(),p->height());
            p->setVisible(true);
        }
        i++;
    }
    step = JOINTS_NUMBER;

    f.close();
    *********/
}

void RenderWidget::writeFile()
{
    if(step >= JOINTS_NUMBER){
        QFile f("test.txt");
        if(!f.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Open failed.";
            return;
        }
        QTextStream txtOutput(&f);

        PickPoint *p0 = pickPointArray[0];
        float x = p0->x();
        float y = p0->y();
        txtOutput << x << "\t" << y << "\t\n";

        for(int i = 0;i < 5;i ++){
            for(int j = 0;j < 4;j ++){
                PickPoint *p = pickPointArray[1+i * 4 + j];
                int x = p->x();
                int y = p->y();
                txtOutput << x << "\t" << y << "\t";
            }
            txtOutput <<"\n";
        }
        f.close();
    }else{
        QString string = "Joints not enough:" + QString::number(step);
        QMessageBox::about(NULL, "Write Error", string);
    }
}


/**
 * Perform one thinning iteration.
 * Normally you wouldn't call this function directly from your code.
 *
 * Parameters:
 * 		im    Binary image with range = [0,1]
 * 		iter  0=even, 1=odd
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

/**  @function Erosion  */
void Erosion( int, void* )
{
//  int erosion_type = MORPH_RECT;
//  int erosion_size = 0;
////  if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
////  else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
////  else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

//  Mat element = getStructuringElement( erosion_type,
//                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
//                                       Point( erosion_size, erosion_size ) );

//  /// 腐蚀操作
//  erode( src, erosion_dst, element );
//  imshow( "Erosion Demo", erosion_dst );
}


/********* find far point ***************/

int findFarPoint(vector<Point> &_neighbor_points, Mat &_image, Point _inpoint, Point& _outpoint, int &count){

    count = 0;
    int maxdist = 0;
    _outpoint = _inpoint;

    _image.at<uchar>(_inpoint) = 0;


    for(int i = 0; i < _neighbor_points.size();i ++){
        Point tmppoint  = _inpoint + _neighbor_points[i];
        if (tmppoint.x > 0 && tmppoint.y > 0 && tmppoint.x < _image.cols&&tmppoint.y < _image.rows){
            int tmpdist = 0;
            Point tmpoutPoint;
            if(_image.at<uchar>(tmppoint) != 0){
                _image.at<uchar>(tmppoint) = 0;
                int _count;
                tmpdist = 1 + findFarPoint(_neighbor_points,_image,tmppoint,tmpoutPoint,_count);
//                count += _count;
                if(tmpdist > maxdist){
                    maxdist = tmpdist;
                    _outpoint = tmpoutPoint;
                    count ++;
                }
            }
        }
        if(maxdist > 200)
            break;
    }

    return maxdist;
}


/********* find curves ***************/
bool findNextPoint(vector<Point> &_neighbor_points, Mat &_image, Point _inpoint, int flag, Point& _outpoint, int &_outflag)
{
    int i = flag;
    int count = 1;
    bool success = false;

    while (count <= 8)  //////////////// 7
    {
        Point tmppoint = _inpoint + _neighbor_points[i];
        if (tmppoint.x > 0 && tmppoint.y > 0 && tmppoint.x < _image.cols&&tmppoint.y < _image.rows)
        {
            if (_image.at<uchar>(tmppoint) == 255)
            {
                _outpoint = tmppoint;
                _outflag = i;
                success = true;
                _image.at<uchar>(tmppoint) = 0;  ///////////  0
                break;
            }
        }
        if (count % 2)
        {
            i += count;
            if (i > 7)
            {
                i -= 8;
            }
        }
        else
        {
            i += -count;
            if (i < 0)
            {
                i += 8;
            }
        }
        count++;
    }
    return success;
}
//寻找图像上的第一个点
bool findFirstPoint(Mat &_inputimg, Point &_outputpoint)
{
    bool success = false;
    for (int i = 0; i < _inputimg.rows; i++)
    {
        uchar* data = _inputimg.ptr<uchar>(i);
        for (int j = 0; j < _inputimg.cols; j++)
        {
            if (data[j] == 255)
            {
                success = true;
                _outputpoint.x = j;
                _outputpoint.y = i;
                data[j] = 0;
                break;
            }
        }
        if (success)
            break;
    }
    return success;
}

bool isLine(Point p, double k, Point P0,double eps){
    double y = k * (p.x - P0.x) + P0.y;
    return abs(y - p.y) < eps;
}

//寻找曲线
void findLines(Mat &inputimg, vector<deque<Point>> &_outputlines)
{
    Mat _inputimg = inputimg.clone();
    vector<Point> neighbor_points = { Point(-1,-1),Point(0,-1),Point(1,-1),Point(1,0),Point(1,1),Point(0,1),Point(-1,1),Point(-1,0) };
    Point first_point;
    while (findFirstPoint(_inputimg, first_point))
    {
        vector<Point> vecLine;
        deque<Point> line;
        line.push_back(first_point);
        vecLine.push_back(first_point);
        //由于第一个点不一定是线段的起始位置，双向找
        Point this_point = first_point;
        int this_flag = 0;
        Point next_point;
        int next_flag;
        while (findNextPoint(neighbor_points, _inputimg, this_point, this_flag, next_point, next_flag))
        {
            vecLine.push_back(next_point);
            line.push_back(next_point);
            this_point = next_point;
            this_flag = next_flag;
        }
        //找另一边
        this_point = first_point;
        this_flag = 0;
        //cout << "flag:" << this_flag << endl;
        while (findNextPoint(neighbor_points, _inputimg, this_point, this_flag, next_point, next_flag))
        {
            line.push_front(next_point);
//            vecLine.push_front(next_point);
            vecLine.insert(vecLine.begin(),next_point);

            this_point = next_point;
            this_flag = next_flag;
        }
        cv::Vec4f line_para;
        cv::fitLine(vecLine, line_para, cv::DIST_L2, 0, 1e-2, 1e-2);
        Point point0;
        point0.x = line_para[2];
        point0.y = line_para[3];
        double k = line_para[1] / line_para[0];

        float eps = 1e-2;
        if (line.size() > 10)  //adjusted
        {
            Point p_end = line.at(line.size() - 1);
            Point p_start = line.at(0);
            int range = 3;
            if(p_start.x < range||p_start.y < range||p_end.x < range|| p_end.y < range){
                continue;
            }
            if(isLine(line.at(0),k,point0,eps) \
                    && isLine(line.at(line.size() - 1),k,point0,eps)\
                    && isLine(line.at(line.size()/3),k,point0,eps) \
                    && isLine(line.at(int(line.size() * 0.6)),k,point0,eps))
                continue;
            else
                _outputlines.push_back(line);
        }
    }
}
//随机取色 用于画线的时候
Scalar random_color(RNG& _rng)
{
    int icolor = (unsigned)_rng;
    return Scalar(icolor & 0xFF, (icolor >> 8) & 0xFF, (icolor >> 16) & 0xFF);
}

/*********************/

/*********** line thinning  ************/

static int Get_gray(Mat* im, int x, int y) {
  return static_cast<int>(im->at<uchar>(Point(x, y)));

//  int c = cv gdImageGetPixel(ptr, x, y);
//  int r = gdImageRed(ptr, c);
//  int g = gdImageGreen(ptr, c);
//  int b = gdImageBlue(ptr, c);
//  return GRAY(r, g, b);
}

//四周细化算法[/backcolor]
void Refine(Mat& image)
{
        int p[8];
        int top=1, down=1, right=1, left=1;
        vector<Point> del;
        int grayvalue = 0;
        int height = image.rows;   //获取图像高度
        int width = image.cols;           //获取图像宽度
        Mat *im = reinterpret_cast<Mat*>((void*)&image);    //获取像素点信息
        //上下收缩
        for (int i = 1; i < height-1; i++)
        {
                for (int j = 1; j < width-1; j++)
                {
                        grayvalue = Get_gray(im, j, i);  //获取指定点灰度值
                        if (grayvalue != 0)   //判断中心点是否为前景
                        {
                                p[0] = (Get_gray(im, j + 1, i) == 0) ? 0 : 1;
                                p[1] = (Get_gray(im, j + 1, i - 1) == 0) ? 0 : 1;
                                p[2] = (Get_gray(im, j, i - 1) == 0) ? 0 : 1;
                                p[3] = (Get_gray(im, j - 1, i - 1) == 0) ? 0 : 1;
                                p[4] = (Get_gray(im, j - 1, i) == 0) ? 0 : 1;
                                p[5] = (Get_gray(im, j - 1, i + 1) == 0) ? 0 : 1;
                                p[6] = (Get_gray(im, j, i + 1) == 0) ? 0 : 1;
                                p[7] = (Get_gray(im, j + 1, i + 1) == 0) ? 0 : 1;
                                if (i < height - 2)
                                        down = (Get_gray(im, j, i + 2) == 0) ? 0 : 1;
                                else
                                        down = 1;
                                //  横向直线
                                if (p[6] && (p[5] || p[7] || p[0] || p[4]) && !(p[1] || p[3]) && p[2] == 0 && down)
                                {
                                        del.push_back(Point(j, i));
                                }
                                if (p[2] && (p[1] || p[3] || p[0] || p[4]) && !( p[5] || p[7]) && p[6] == 0)
                                {
                                        del.push_back(Point(j, i));
                                }
                        }
                }
        }

        for (int i = 1; i < height - 2; i++)
        {
                grayvalue = Get_gray(im, 0, i);
                if (grayvalue != 0)
                {
                        if ( Get_gray(im, 0, i - 1) && Get_gray(im, 1, i - 1) && Get_gray(im, 0, i + 1)==0 && Get_gray(im, 1, i)==0) //上2，上1，右上1，下1=0，右1=0
                        {
                                del.push_back(Point(0, i));
                        }
                        if (Get_gray(im, 0, i - 1) == 0 && Get_gray(im, 1, i + 1) && Get_gray(im, 1, i) == 0 && Get_gray(im, 0, i+2))//上1=0，下1，右下1，右1=0，下2
                        {
                                del.push_back(Point(0, i));
                        }
                }
                if (grayvalue != 0)
                {
                        if (Get_gray(im, width - 1, i - 1) && Get_gray(im, width - 2, i - 1) && Get_gray(im, width - 1, i + 1) == 0 && Get_gray(im, width - 2, i) == 0) //上2，上1，左上1，下1=0，左1=0
                        {
                                del.push_back(Point(width - 1, i));
                        }
                        if (Get_gray(im, width - 1, i - 1) == 0 && Get_gray(im, width - 2, i + 1) && Get_gray(im, width - 2, i) == 0 && Get_gray(im, width - 1, i + 2))//上1=0，下1，左下1，左1=0，下2
                        {
                                del.push_back(Point(width - 1, i));
                        }
                }
        }
        for (int i = 0; i < del.size();i++)
        {
                uchar* data = image.ptr<uchar>(del[i].y);
                data[del[i].x]=0;
        }

        //左右收缩
        for (int i = 1; i < height - 1; i++)
        {
                for (int j = 1; j < width - 1; j++)
                {
                        grayvalue = Get_gray(im, j, i);  //获取指定点灰度值
                        if (grayvalue != 0)   //判断中心点是否为前景
                        {
                                p[0] = (Get_gray(im, j + 1, i) == 0) ? 0 : 1;
                                p[1] = (Get_gray(im, j + 1, i - 1) == 0) ? 0 : 1;
                                p[2] = (Get_gray(im, j, i - 1) == 0) ? 0 : 1;
                                p[3] = (Get_gray(im, j - 1, i - 1) == 0) ? 0 : 1;
                                p[4] = (Get_gray(im, j - 1, i) == 0) ? 0 : 1;
                                p[5] = (Get_gray(im, j - 1, i + 1) == 0) ? 0 : 1;
                                p[6] = (Get_gray(im, j, i + 1) == 0) ? 0 : 1;
                                p[7] = (Get_gray(im, j + 1, i + 1) == 0) ? 0 : 1;
                                if (j < width - 2)
                                        right = (Get_gray(im, j + 2, i) == 0) ? 0 : 1;
                                else
                                        right = 1;


                                //竖直线
                                if (p[0] && (p[1] || p[7] || p[2] || p[6]) && !(p[3] || p[5]) && p[4] == 0 && right)
                                {
                                        del.push_back(Point(j, i));
                                }
                                if (p[4] && (p[3] || p[5] || p[2] || p[6]) && !(p[1] || p[7]) && p[0] == 0)
                                {
                                        del.push_back(Point(j, i));
                                }

                        }
                }
        }

        for (int j = 1; j < width - 2; j++)
        {
                grayvalue = Get_gray(im, j, 0);
                if (grayvalue != 0)
                {
                        if (Get_gray(im, j - 1, 0) == 0 && Get_gray(im, j + 1, 0) && Get_gray(im, j + 2, 0) && Get_gray(im, j, 1) == 0 && Get_gray(im, j+1, 1)) //左1=0，右1，右2，下1=0，右下1
                        {
                                del.push_back(Point(j, 0));
                        }
                        if (Get_gray(im, j - 1, 0) && Get_gray(im, j+1, 0)==0 && Get_gray(im, j, 1) == 0 && Get_gray(im, j-1, 1))//左1，右1=0，下1=0，左下1
                        {
                                del.push_back(Point(j, 0));
                        }
                }
        }
        for (int j = 1; j < width - 2; j++)
        {
                grayvalue = Get_gray(im, j, height-1);
                if (grayvalue != 0)
                {
                        if (Get_gray(im, j - 1, height - 1) == 0 && Get_gray(im, j + 1, height - 1) && Get_gray(im, j + 2, height - 1) && Get_gray(im, j, height - 2) == 0 && Get_gray(im, j + 1, height - 2)) //左1=0，右1，右2，下1=0，右下1
                        {
                                del.push_back(Point(j, height - 1));
                        }
                        if (Get_gray(im, j - 1, height - 1) && Get_gray(im, j + 1, height - 1) == 0 && Get_gray(im, j, height - 2) == 0 && Get_gray(im, j - 1, height - 2))//左1，右1=0，下1=0，左下1
                        {
                                del.push_back(Point(j, height - 1));
                        }
                }
        }

        for (int i = 0; i < del.size(); i++)
        {
                uchar* data = image.ptr<uchar>(del[i].y);
                data[del[i].x] = 0;
        }
}
/**********************************/


vector<Point> neighbor_points5 = { Point(-2,-2),Point(-1,-2),Point(0,-2),Point(1,-2),Point(2,-2),\
                                   Point(2,-1),Point(2,0),Point(2,1),\
                                   Point(2,2),Point(1,2),Point(0,2),Point(-1,2),Point(-2,2),\
                                   Point(-2,1),Point(-2,0),Point(-2,-1) };

/************** T junction detection ********************/

void TJunctionDetect(Mat &img, vector<Point2f> &corners, vector<Point> &Tjunc, vector<Point2f> &norm1,vector<Point2f> &norm2, int r){

    CV_Assert(img.channels() == 1);
    CV_Assert(img.depth() != sizeof(uchar));
    CV_Assert(img.rows > 3 && img.cols > 3);

//    vector<Point2f>::const_iterator itCorners = corners.begin();
    vector<Point> neighbor_points = { Point(-1,-1),Point(0,-1),Point(1,-1),Point(1,0),Point(1,1),Point(0,1),Point(-1,1),Point(-1,0) };

    vector<Point> Tjunction;
//    vector<int> crossPtsIdx;


    for(int i = 0; i < img.rows; i ++){
        uchar* data = img.ptr<uchar>(i);
        for(int j = 0; j < img.cols; ++j)
        {
            bool found = false;
            Point cenPt(j, i);

            if(data[j] != 0){
                if(findTjunction(img, cenPt, neighbor_points)){
                    Tjunction.push_back(cenPt);
                    found = true;
                }
            }
//        }
//    for ( ; itCorners!=corners.end(); ++itCorners)
//    {
//        bool found = false;

//        Point cenPt = (*itCorners);
//        if (img.at<uchar>(cenPt) != 0){
//            if(findTjunction(img, cenPt, neighbor_points)){
//                Tjunction.push_back(cenPt);
//                found = true;
//            }
//        }
//        if (!found){
//            int rMax = 6; // 1 - 3x3 2 - 5x5 3 - 7x7 4 - 9x9
//            for(int s = 1;s <= rMax;s++){
//                vector<Point> round_points;
//                int w = 2 * s + 1;
//                int h = 2 * s - 1;
//                int curx = -s;
//                int cury = -s;
//                for(int i = 0; i < 8 * s; i++){
//                    Point tmpPt(curx, cury);
//                    round_points.push_back(tmpPt);
//                    if(i < w - 1){
//                        curx ++;
//                    }else if(i > (w + h - 1) && i < (w + h + w - 1)){
//                        curx --;
//                    }
//                    if(i > (w - 2) && i < (w + h)){
//                        cury ++;
//                    }else if(i > (w + h + w - 2)){
//                        cury --;
//                    }
//                }
//                for(int i = 0;i < round_points.size();i++){
//                    Point tmpPoint = cenPt + round_points[i];
//                    if(findTjunction(img, tmpPoint, neighbor_points)){
//                        Tjunction.push_back(tmpPoint);
//                        found = true;
//                        break;
//                    }
//                }
//                if(found)break;
//            }
//        }
    }
    }

    vector<Point>::const_iterator itTjunc = Tjunction.begin();
//    qDebug("Tjunction.size = %d",Tjunction.size());

    Mat imgclone1 = img.clone();

    Mat imgclone2 = img.clone();

    Mat imgclone = img.clone();

    Mat imgclone3 = img.clone();

//    imwrite(folder_path + "Tjunc_src.png",img);

    for ( ; itTjunc!=Tjunction.end(); ++itTjunc){

//        qDebug("for ( ; itTjunc!=Tjunction.end(); ++itTjunc){  %d", itTjunc - Tjunction.begin());

        Point center_point = (*itTjunc);

        imgclone1.at<uchar>(center_point) = 128;

        vector<Point> crossPts;
        vector<int> crossPtsIdx;

        int last = 0;
        Point pp = center_point + neighbor_points[neighbor_points.size() - 1];
        if (img.at<uchar>(pp) != 0)
            last = 1;
        for(int i = 0;i < neighbor_points.size();i++){
            // clear  qDebug("int i = 0;i < neighbor_points.size();i++");

            Point tmppoint = center_point + neighbor_points[i];
            if (tmppoint.x > 0 && tmppoint.y > 0 && tmppoint.x < img.cols&&tmppoint.y < img.rows){

                if (img.at<uchar>(tmppoint) != 0)
                {
                    if(last != 1){
                        crossPtsIdx.push_back(i);
                        crossPts.push_back(tmppoint);
                        imgclone1.at<uchar>(tmppoint) = 180;
                    }
                    last = 1;
                    if(i % 2 == 1){
                        crossPtsIdx.pop_back();
                        crossPts.pop_back();

                        crossPtsIdx.push_back(i);
                        crossPts.push_back(tmppoint);
                    }
                }else{
                    last = 0;
                }

            }
        }

//        for(int i = 0;i < neighbor_points.size();i++){
//            Point tmppoint = cenPt + neighbor_points[i];
//            if (img.at<uchar>(tmppoint) != 0)
//            {
//                if(i % 2 == 1){
//                    crossPts.push_back(tmpPoint);
//                    crossPtsIdx.push_back(2 * i + 1);
//                }else{
//                    if(i >= 1)
//                    Point lastpoint = cenPt + neighbor_points[i - 1];
//                    Point nextpoint = cenPt + neighbor_points[(i + 1)%neighbor_points.size()];

//                }
//            }
//        }

//        for(int i = 0;i < 3; i ++){
//            Point tmpPoint = center_point + neighbor_points[crossPtsIdx[i]];
//            crossPts.push_back(tmpPoint);
//        }
//        for(int i = 0;i < neighbor_points.size()/2;i++){
//            Point tmpPoint = center_point + neighbor_points[2 * i + 1];
//            if (img.at<uchar>(tmpPoint) != 0){
//                crossPts.push_back(tmpPoint);
//                crossPtsIdx.push_back(2 * i + 1);
//            }
//        }

        if(crossPts.size() != 3 || crossPtsIdx.size() != 3){
//            qDebug("error: T junction with %d branches!",crossPts.size());
            continue;
        }

        int juncCount = 0;
//        int lineCount[] = {0,0,0};
        vector<Point> vecLine[3];

        int idx;

        for(int i = 0; i < 3; i++){
//            qDebug("Mat imgclone = img.clone();");

            // init image

//            imwrite(folder_path + "imgcloneb.png",imgclone);

            Mat imgclone = img.clone();

            imgclone.at<uchar>(center_point) = 0;
            for(int j = 0;j < 3;j ++){
                imgclone.at<uchar>(crossPts[j]) = 0;
            }
            Point this_point = crossPts[i];
            int this_flag = 0;
            Point next_point;
            int next_flag;

            vecLine[i].push_back(center_point);
            vecLine[i].push_back(this_point);

            Mat _imgclone = imgclone.clone();
            int counti;
            int dist = 2 + findFarPoint(neighbor_points,_imgclone,this_point,next_point,counti);

            this_point = crossPts[i];

            while (findNextPoint(neighbor_points, imgclone, this_point, this_flag, next_point, next_flag))
            {
                //clear qDebug("findNextPoint(neighbor_points, imgclone, this_point");

//                lineCount[i] ++;

                imgclone3.at<uchar>(next_point) -= 60;

                vecLine[i].push_back(next_point);
                this_point = next_point;
                this_flag = next_flag;
                if(vecLine[i].size() > 1 * r )break;
            }
            ///// have holes here
            ///
//            if(vecLine[i].size() > r){
//            }
            if(dist <= r){
                idx = i;
            }else{
                juncCount++;
            }
        }

        if(juncCount == 2 ){

            imwrite(folder_path + "Tjunc_line.png",imgclone3);

            imgclone2.at<uchar>(vecLine[0][0]) = 200;

            Point2f norm3[3];
            for(int j = 0;j < 3;j ++){
                int idx1 = (idx + j)%3;
                cv::Vec4f line1_para;
                if(vecLine[idx1].size() >= 2){
                    cv::fitLine(vecLine[idx1], line1_para, cv::DIST_L2, 0, 1e-2, 1e-2);
                    Point point1;
                    point1.x = line1_para[2];
                    point1.y = line1_para[3];
                    if(fabs(line1_para[0]) < 1e-2){
                        norm3[j] = Point2f(0,1.0);
                    }else{
                        double k1 = line1_para[1] / line1_para[0];
                        norm3[j] = Point2f(1.0,k1);
                        norm3[j] /= sqrt(norm3[j].ddot(norm3[j]));
                    }
                }else if(vecLine[idx1].size() < 2) {
                    norm3[j] = Point2f(1.0,0);
                }

                Point2f vecdir = (*(vecLine[idx1].end() - 1)) - (*(vecLine[idx1].begin() + 1));
                if(norm3[j].ddot(vecdir) < 0)
                    norm3[j] *= -1.0;
            }

//            cv::fitLine(vecLine[idx2], line2_para, cv::DIST_L2, 0, 1e-2, 1e-2);
//            Point point2;
//            point2.x = line2_para[2];
//            point2.y = line2_para[3];
//            double k2 = line2_para[1] / line2_para[0];
//            Point2f norm2;
//            float len = sqrt(1 + k2 * k2);
//            if(vecLine[idx1].end().x >= vecLine[idx1].begin().x)
//                norm1 = Point2f(1.0/len,k/len);
//            else
//                norm1 = Point2f(-1.0/len,-k/len);

//            norm1 = - crossPts[(idx + 1)%3] + center_point;
//            norm2 = - crossPts[(idx + 2)%3] + center_point;

            Point2f tmp_norm;
//            if(norm1.x + norm2.x == 0 || norm1.y + norm2.y == 0 )
//                tmp_norm = crossPts[idx] - center_point;
//            else

            tmp_norm = norm3[1] + norm3[2];
            if(tmp_norm.ddot(norm3[0]) < 0)tmp_norm *= -1.0;

            tmp_norm /= sqrt(tmp_norm.ddot(tmp_norm));

//            float angle = acos(tmp_norm.ddot(norm3[0]));

            tmp_norm /= sqrt(tmp_norm.ddot(tmp_norm));
            norm3[0] /= sqrt(norm3[0].ddot(norm3[0]));
            norm1.push_back(tmp_norm);
            norm2.push_back(norm3[0]);

//            if(angle > CV_PI/3.0){
//                tmp_norm = tmp_norm * 0.5 + norm3[0] * 0.5;

//            }

            tmp_norm /= sqrt(tmp_norm.ddot(tmp_norm));
//            qDebug("tmp_norm pre = %f,%f",tmp_norm.x,tmp_norm.y);

//            findEnd(mask, center_point, tmp_norm);

//            qDebug("tmp_norm after = %f,%f",tmp_norm.x,tmp_norm.y);

//            norm.push_back(tmp_norm);
            Tjunc.push_back(center_point);


            /*************** remove T **************/


            //release the center
//            qDebug("vecLine[idx][i] = %d",idx,0);

            for(int i = 1; i < vecLine[idx].size(); i++){
//                qDebug("vecLine[idx][i] = %d,%d",vecLine[idx][i].x,vecLine[idx][i].y);
                img.at<uchar>(vecLine[idx][i]) = 0;
            }

       ////// rush here
            RemoveSmallRegion(img,img,4,1,1);
//            for(int i = 0; i < neighbor_points.size();i++){
//                Point tmpPoint = (*vecLine[idx].end() + neighbor_points[i]);
//                img.at<uchar>(tmpPoint) = 0;
//            }

            /*
            int count5 = 0;
            for(int k = 0;k <neighbor_points5.size();k++){
                Point tmppoint = center_point + neighbor_points5[k];
                if (img.at<uchar>(tmppoint) != 0){
                    count5++;
                }
            }
            if(count5 != 3)
                qDebug("point at %d, %d, count = %d",center_point.x,center_point.y,count5);
            */
        }
    }


    imwrite(folder_path + "Tjunc1.png",imgclone1);
    imwrite(folder_path + "Tjunc2.png",imgclone2);


//    img.at<Vec3b>(i, j)[0];
}

bool findTjunction(Mat &img, Point cenPt, vector<Point> &neighbor_points){


    if (img.at<uchar>(cenPt) != 0){
        int last = 0;
        int white = 0;
        int black = 0;

        Point pp = cenPt + neighbor_points[neighbor_points.size() - 1];
        if (img.at<uchar>(pp) != 0)
            last = 1;
        for(int i = 0;i < neighbor_points.size();i++){
            Point tmppoint = cenPt + neighbor_points[i];
            if (img.at<uchar>(tmppoint) != 0)
            {
                if(last != 1){
                    white ++;
//                    crossPtsIdx.push_back(i);
                }
                last = 1;
//                if(i % 2 == 1){
//                    crossPtsIdx.pop_back();
//                    crossPtsIdx.push_back(i);
//                }
            }else{
                if(last != 0)
                    black ++;
                last = 0;
            }
        }
        if(white == 3 && black == 3)
            return true;
    }
    return false;
}

void InitMat(Mat& m,float* num)
{
    for(int i=0;i<m.rows;i++)
        for(int j=0;j<m.cols;j++)
            m.at<float>(i,j)=*(num+i*m.rows+j);
}

int maxPixelLen = 25; ///////adjusted here
int igPixelLen = 5;

void findEnd(Mat &img, Point centerPoint, Point2f &vecDir){
    int r = 10; // range larger than 10
    vector<Point> neighbor_points = { Point(-1,-1),Point(0,-1),Point(1,-1),Point(1,0),Point(1,1),Point(0,1),Point(-1,1),Point(-1,0) };
    int divide = 2;
    float tmpAngle = 0;
    int minPixelLen = img.cols + img.rows;

    Point2f minVecDir = vecDir;

//    int compareArray[3] = {0,0,0};

    float dAngle = CV_PI/4.0/divide;
    for(int i = -divide/2; i < divide/2; i ++){
        tmpAngle = dAngle * i;
        float m0[]={ cos(tmpAngle),sin(tmpAngle),-sin(tmpAngle),cos(tmpAngle)};
//        Mat M0(2,2,CV_32F);
//        InitMat(M0,m0);
        Point2f tmpVecDir(m0[0] * vecDir.x+ m0[1] * vecDir.y, m0[2] * vecDir.x + m0[3] * vecDir.y);

        if(fabs(tmpVecDir.ddot(tmpVecDir) - 1) > 1e-2)
            qDebug("error: the vector is not normalized");

        float absX = fabs(tmpVecDir.x);
        float absY = fabs(tmpVecDir.y);

        int startNbIdx;

        if(absX > absY){
            tmpVecDir /= absX;
            if(tmpVecDir.x > 0){
                startNbIdx = 1;
            }else{
                startNbIdx = 5;
            }
        }
        else{
            tmpVecDir /= absY;
            if(tmpVecDir.y > 0)
                startNbIdx = 3;
            else
                startNbIdx = 7;
        }

        bool foundEnd = false;
        int tmpPixelLen = maxPixelLen;
        for(int j = igPixelLen; j < maxPixelLen; j++){
            Point tmpPoint = centerPoint + (Point)(tmpVecDir * j);
            for(int k = 0; k < 5;k ++){
                int idx = (startNbIdx + k)%8;
                Point tmpNbPoint = tmpPoint + neighbor_points[idx];
                if(tmpNbPoint.x > img.cols - 2 || tmpNbPoint.y > img.rows - 2)
                    continue;
//                qDebug("coor = %d,%d",tmpNbPoint.x,tmpNbPoint.y);
                if (img.at<uchar>(tmpNbPoint) != 0){
                    foundEnd = true;
                    break;
                }
            }
            if(foundEnd){
                tmpPixelLen = j;
                break;
            }
        }
        if(minPixelLen > tmpPixelLen){
            minPixelLen = tmpPixelLen;
            minVecDir = tmpVecDir;
        }
    }
    vecDir = minVecDir/sqrt(minVecDir.ddot(minVecDir));
}

//CheckMode: 0代表去除黑区域，1代表去除白区域; NeihborMode：0代表4邻域，1代表8邻域;
void RemoveSmallRegion(Mat& Src, Mat& Dst, int AreaLimit, int CheckMode, int NeihborMode)
{
   int RemoveCount=0;       //记录除去的个数
   //记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查
   Mat Pointlabel = Mat::zeros( Src.size(), CV_8UC1 );

   if(CheckMode==1)
   {
//       cout<<"Mode: 去除小区域. ";
       for(int i = 0; i < Src.rows; ++i)
       {
           uchar* iData = Src.ptr<uchar>(i);
           uchar* iLabel = Pointlabel.ptr<uchar>(i);
           for(int j = 0; j < Src.cols; ++j)
           {
               if (iData[j] < 10)
               {
                   iLabel[j] = 3;
               }
           }
       }
   }
   else
   {
//       cout<<"Mode: 去除孔洞. ";
       for(int i = 0; i < Src.rows; ++i)
       {
           uchar* iData = Src.ptr<uchar>(i);
           uchar* iLabel = Pointlabel.ptr<uchar>(i);
           for(int j = 0; j < Src.cols; ++j)
           {
               if (iData[j] > 10)
               {
                   iLabel[j] = 3;
               }
           }
       }
   }

   vector<Point2i> NeihborPos;  //记录邻域点位置
   NeihborPos.push_back(Point2i(-1, 0));
   NeihborPos.push_back(Point2i(1, 0));
   NeihborPos.push_back(Point2i(0, -1));
   NeihborPos.push_back(Point2i(0, 1));
   if (NeihborMode==1)
   {
//       cout<<"Neighbor mode: 8邻域."<<endl;
       NeihborPos.push_back(Point2i(-1, -1));
       NeihborPos.push_back(Point2i(-1, 1));
       NeihborPos.push_back(Point2i(1, -1));
       NeihborPos.push_back(Point2i(1, 1));
   }
//   else cout<<"Neighbor mode: 4邻域."<<endl;
   int NeihborCount=4+4*NeihborMode;
   int CurrX=0, CurrY=0;
   //开始检测
   for(int i = 0; i < Src.rows; ++i)
   {
       uchar* iLabel = Pointlabel.ptr<uchar>(i);
       for(int j = 0; j < Src.cols; ++j)
       {
           if (iLabel[j] == 0)
           {
               //********开始该点处的检查**********
               vector<Point2i> GrowBuffer;                                      //堆栈，用于存储生长点
               GrowBuffer.push_back( Point2i(j, i) );
               Pointlabel.at<uchar>(i, j)=1;
               int CheckResult=0;                                               //用于判断结果（是否超出大小），0为未超出，1为超出

               for ( int z=0; z<GrowBuffer.size(); z++ )
               {

                   for (int q=0; q<NeihborCount; q++)                                      //检查四个邻域点
                   {
                       CurrX=GrowBuffer.at(z).x+NeihborPos.at(q).x;
                       CurrY=GrowBuffer.at(z).y+NeihborPos.at(q).y;
                       if (CurrX>=0&&CurrX<Src.cols&&CurrY>=0&&CurrY<Src.rows)  //防止越界
                       {
                           if ( Pointlabel.at<uchar>(CurrY, CurrX)==0 )
                           {
                               GrowBuffer.push_back( Point2i(CurrX, CurrY) );  //邻域点加入buffer
                               Pointlabel.at<uchar>(CurrY, CurrX)=1;           //更新邻域点的检查标签，避免重复检查
                           }
                       }
                   }

               }
               if (GrowBuffer.size()>AreaLimit) CheckResult=2;                 //判断结果（是否超出限定的大小），1为未超出，2为超出
               else {CheckResult=1;   RemoveCount++;}
               for (int z=0; z<GrowBuffer.size(); z++)                         //更新Label记录
               {
                   CurrX=GrowBuffer.at(z).x;
                   CurrY=GrowBuffer.at(z).y;
                   Pointlabel.at<uchar>(CurrY, CurrX) += CheckResult;
               }
               //********结束该点处的检查**********


           }
       }
   }

   CheckMode=255*(1-CheckMode);
   //开始反转面积过小的区域
   for(int i = 0; i < Src.rows; ++i)
   {
       uchar* iData = Src.ptr<uchar>(i);
       uchar* iDstData = Dst.ptr<uchar>(i);
       uchar* iLabel = Pointlabel.ptr<uchar>(i);
       for(int j = 0; j < Src.cols; ++j)
       {
           if (iLabel[j] == 2)
           {
               iDstData[j] = CheckMode;
           }
           else if(iLabel[j] == 3)
           {
               iDstData[j] = iData[j];
           }
       }
   }

//   cout<<RemoveCount<<" objects removed."<<endl;
}

bool findXPoint(Mat &img, Point point0,Point2f vecDir,Point &crossPt){

    if(fabs(vecDir.ddot(vecDir) - 1) > 1e-2){
//        qDebug("error: the vector is not normalized");
        vecDir /= sqrt(vecDir.ddot(vecDir));
    }

    float absX = fabs(vecDir.x);
    float absY = fabs(vecDir.y);

    int startNbIdx;

    if(absX > absY){
        vecDir /= absX;
        if(vecDir.x > 0){
            startNbIdx = 1;
        }else{
            startNbIdx = 5;
        }
    }
    else{
        vecDir /= absY;
        if(vecDir.y > 0)
            startNbIdx = 3;
        else
            startNbIdx = 7;
    }

    vector<Point> neighbor_points = { Point(-1,-1),Point(0,-1),Point(1,-1),Point(1,0),Point(1,1),Point(0,1),Point(-1,1),Point(-1,0) };
    bool foundEnd = false;

    for(int j = igPixelLen; j < maxPixelLen; j++){
        Point tmpPoint = point0 + (Point)(vecDir * j);
        Point tmpNbPoint;
        for(int k = 0; k < 5;k ++){
            int idx = (startNbIdx + k)%8;
            tmpNbPoint = tmpPoint + neighbor_points[idx];
            if(tmpNbPoint.x > img.cols - 1 || tmpNbPoint.y > img.rows - 1)
                continue;
//                qDebug("coor = %d,%d",tmpNbPoint.x,tmpNbPoint.y);
            if (img.at<uchar>(tmpNbPoint) != 0){
                foundEnd = true;
                break;
            }
        }
        if(foundEnd){
            crossPt = tmpNbPoint;
            break;
        }
    }
    return foundEnd;
}

void splitFingerLine(vector<Point> &fingerLine,  vector<vector<Point>> &output_fingerLines, vector<Point> &jointPoints,\
                     int size){

}
