#include "tjunctiondetect.h"

vector<Point> neighbor_points5 = { Point(-2,-2),Point(-1,-2),Point(0,-2),Point(1,-2),Point(2,-2),\
                                   Point(2,-1),Point(2,0),Point(2,1),\
                                   Point(2,2),Point(1,2),Point(0,2),Point(-1,2),Point(-2,2),\
                                   Point(-2,1),Point(-2,0),Point(-2,-1) };



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
        }
    }

    vector<Point>::const_iterator itTjunc = Tjunction.begin();
//    qDebug("Tjunction.size = %d",Tjunction.size());

    Mat imgclone1 = img.clone();

    Mat imgclone2 = img.clone();

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

//            imwrite(folder_path + "Tjunc_line.png",imgclone3);

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


//    imwrite(folder_path + "Tjunc1.png",imgclone1);
//    imwrite(folder_path + "Tjunc2.png",imgclone2);


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

bool findXPoint(Mat &img, Point point0,Point2f vecDir,Point &crossPt){

    int maxPixelLen = 25; ///////adjusted here
    int igPixelLen = 5;

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
