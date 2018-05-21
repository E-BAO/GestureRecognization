#include "training.h"

#include <QFile>

int indexofImg = 1;

//随机取色 用于画线的时候
Scalar random_color(RNG& _rng)
{
    int icolor = (unsigned)_rng;
    return Scalar(icolor & 0xFF, (icolor >> 8) & 0xFF, (icolor >> 16) & 0xFF);
}

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

double compared (const void * a, const void * b)
{
  return ( *(double*)a - *(double*)b );
}

void training(string folder_path, vector<Point> &out_para, Mat &output_img)
{
//    qDebug()<<"image gesture"<<indexofImg;

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
        Point2f normVec1 = norm1[i];
        Point2f normVec2 = norm2[i];
        Point crossPt2;
        Point crossPt1;

//        qDebug("point %d,%d ",TjuncPoint.x,TjuncPoint.y);

        Mat findCross_img = thin_inv.clone();

//        if(findXPoint(thin_inv,TjuncPoint,normVec2,crossPt2)){

//        };

        int mindist1 = 100000,mindist2 = 100000;
        int foundcross = 0;
        if(findXPoint(thin_inv,TjuncPoint,normVec1,crossPt1)){
            mindist1 = (crossPt1 - TjuncPoint).dot(crossPt1 - TjuncPoint);
            foundcross = 1;
        }

        if(findXPoint(thin_inv,TjuncPoint,normVec2,crossPt2)){
            mindist2 = (crossPt2 - TjuncPoint).dot(crossPt2 - TjuncPoint);
            foundcross = 1;
        }


        if(foundcross){

            int mindist;
            Point2f normVec;
            Point crossPt;

            if(mindist1 < mindist2){
                mindist = mindist1;
                normVec = normVec1;
                crossPt = crossPt1;
            }else{
                mindist = mindist2;
                normVec = normVec2;
                crossPt = crossPt2;
            }

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

            if(fabs(dist01 - dist12) < max(dist01,dist12) * 0.3){  //adjust
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
            }else if(fabs(dist01 - dist12) < max(dist01,dist12) * 0.5){  //adjust
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

    output_img = finger_img;

    QString filepath = QString::fromStdString(folder_path) + "parameters.txt";
    QFile f(filepath);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Open failed.";
        return;
    }
    QTextStream txtOutput(&f);

    txtOutput << fingerLines.size() << "\t\n";
    txtOutput << center.x<<"\t"<<center.y<< "\t\n";

    for(int i = 0; i < fingerLines.size();i ++){
        deque<Point> fingerLine = fingerLines[i];
        qDebug() << "fingerLine"<<i<<"size = "<<fingerLine.size();

        for(int j = 0;j < fingerLine.size(); j++){
            Point pt = fingerLine[j];
            int x = pt.x;
            int y = pt.y;
            txtOutput << x << "\t" << y << "\t";
        }
        txtOutput <<"\n";
    }

    for(int i = 0; i < fingerJointsIdx.size();i ++){
        vector<int> JointsIdx = fingerJointsIdx[i];
        for(int j = 0;j < JointsIdx.size(); j++){
            int idx = JointsIdx[j];
            txtOutput << idx << "\t";
        }
        txtOutput <<"\n";
    }

    f.close();
}
