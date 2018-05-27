#include "handstructure2d.h"
#include "bline.h"

#include <QVector2D>
#include <QFile>
#include <QMatrix4x4>

typedef struct color Color;

struct color{
    int b,g,r;
    color(int blue,int green, int red):b(blue),g(green),r(red){}
};

//BGR
Color c_red(0,0,255);
Color c_yellow(0,255,255);
Color c_green(0,255,0);
Color c_cyan(255,255,0);
Color c_blue(255,0,0);
Color c_black(0, 0, 0);

Color paintColor[6] = {
    c_red,c_yellow,c_green,c_cyan,c_blue,c_black
};

HandStructure2D::HandStructure2D()
{
//    calGesture();
}

void HandStructure2D::calBLine(){

    qDebug()<<"-------------cal bline--------------";

    Mat img = imread(folder_path + "gesture.jpg");

    for(int i = 0;i < 5;i++){

        qDebug()<<"bline i="<<i;
        Mat img_i(img.rows,img.cols,CV_8UC1,Scalar(0));
        vector<Point> pts;
        vector<Point> ptss;
        pts.push_back(centerPt);
        pts.push_back(centerPt);
        for(int j = 0; j < 4;j ++){
            if(joint_valid[i][j] != -1){
                pts.push_back(jointPts[i][j]);
            }
//            if(pts.size() > 2)
//                break;
        }
        pts.push_back(pts[pts.size() - 1]);

//        qDebug()<<"bline "<<i<<pts.size();
//        Point ptend = 2 * pts[pts.size() - 1] - pts[pts.size() - 2];
//        pts.push_back(ptend);
//        Point ptstart = 2 * pts[0] - pts[1];
//        pts.insert(pts.begin(),ptstart);
//        qDebug()<<"bline "<<i<<pts.size();

        Scalar colorr(paintColor[i].b,paintColor[i].g,paintColor[i].r);
        for ( int j=0; j<pts.size(); j++)
        {
            cv::circle(img,pts[j],2,colorr,-1,CV_AA);
        }

        createCurve(pts,ptss);


//        Point dir = pts[pts.size() - 1] - pts[0];
//        QVector3D dirvec(dir.x,dir.y,0);
//        float rad = acos(QVector3D::dotProduct(dirvec,QVector3D(1.0,0.0,0.0)) / dirvec.length());
//        float cross = QVector3D::crossProduct(dirvec,QVector3D(1.0,0.0,0.0)).z();
//        rad = cross > 0? rad:2 * CV_PI - rad;
//        float angle = rad / CV_PI * 180;
//        QMatrix4x4 rotate_m;
//        Point midPt = pts[0];

//        rotate_m.setToIdentity();
//        rotate_m.translate(midPt.x,midPt.y,0);
//        rotate_m.rotate( angle,0,0,1.0f);
//        rotate_m.translate(-midPt.x,-midPt.y,0);

//        vector<Point> newpoints;
//        for(int j = 0; j < pts.size(); j ++){
//            QVector3D point_vec(pts[j].x,pts[j].y,0);
//            point_vec = rotate_m * point_vec;
//            int sameX = 0;
//            for(int k = 0;k < j; k ++){
//                if(point_vec.x() == newpoints[k].x)
//                    sameX = 1;
//                break;
//            }
//            if(!sameX){
//                newpoints.push_back(Point(point_vec.x(),point_vec.y()));
//                Point pt = newpoints[newpoints.size() - 1];
//                qDebug()<<"pt==="<<pt.x<<pt.y;
//                cv::circle(img,Point(point_vec.x(),point_vec.y()),5,colorr,1,CV_AA);

//                if(fabs(point_vec.z()) > 1e-6){
//                    qDebug()<<"error lineCurve:rotate makes z >0 = "<<point_vec.z();
//                }
//            }else{
//                qDebug()<<"error lineCurve: sameX exist in curve";
//            }

//        }

//        Mat A;
//        polynomial_curve_fit(newpoints, 2, A);

//        qDebug()<<"fasdfasfdasf";

//        double a = A.at<double>(3, 0), b = A.at<double>(2, 0), c = A.at<double>(1, 0), d = A.at<double>(0, 0);

////        double b = A.at<double>(2, 0), c = A.at<double>(1, 0), d = A.at<double>(0, 0);

//        int x_range = newpoints[newpoints.size() - 1].x - newpoints[0].x;
//        int deltaX = x_range > 0? 1:-1;

//        rotate_m.setToIdentity();
//        rotate_m.translate(midPt.x,midPt.y,0);
//        rotate_m.rotate(-angle,0,0,1.0f);
//        rotate_m.translate(-midPt.x,-midPt.y,0);

//        qDebug()<<"fasdgdfgdfgfasfdasf";

//        for(int j = -400; j < 400;j +=1){
//            double x = newpoints[0].x + j;
//            double y = d + c * x + b*pow(x, 2) + a*pow(x, 3);
//            QVector3D pt_vec(x,y,0);
//            pt_vec = rotate_m * pt_vec;
//            Point pt(pt_vec.x(),pt_vec.y());
//            if (pt.x > 0 && pt.y > 0 && pt.x < img.cols&&pt.y < img.rows){
//                img.at<Vec3b>(pt) = Vec3b(paintColor[i].b,paintColor[i].g,paintColor[i].r);
//                img_i.at<uchar>(pt) = 255;
//            }
//        }

//        qDebug()<<"sadfasdfs";

        for(int j = 0; j < ptss.size();j++){
//            if(ptss[j] == *(pts.end() - 1))
//                break;
            if (ptss[j].x > 0 && ptss[j].y > 0 && ptss[j].x < img.cols&&ptss[j].y < img.rows){
                img.at<Vec3b>(ptss[j]) = Vec3b(paintColor[i].b,paintColor[i].g,paintColor[i].r);
                img_i.at<uchar>(ptss[j]) = 255;
            }
//            qDebug()<<ptss[j].x<<ptss[j].y;
        }

        char ic = '0' + i;
        imwrite(folder_path + "bline"+ic+".png",img_i);
    }

    qDebug()<<"drawdraw bline";

    cv::circle(img,centerPt,5,Scalar(paintColor[5].b,paintColor[5].g,paintColor[5].r),1,CV_AA);

    imwrite(folder_path + "bline.png",img);
}

void HandStructure2D::findRoot(){


    Mat img = imread(folder_path + "gesture.jpg");

    qDebug()<<"draw valid";

    cv::circle(img,centerPt,5,Scalar(paintColor[5].b,paintColor[5].g,paintColor[5].r),1,CV_AA);


    int finger_rotate[5];


    int rotate_clock = 0;
    int rotate_anti = 0;

    for(int i = 0; i < 5; i ++){
        vector<QVector3D> vec_dirs;

        Color colori = paintColor[i];
        qDebug()<<"=========="<<i;
        Point last_validpt = centerPt;

        for(int j = 0; j < 4; j ++){
//            qDebug()<<joint_valid[i][j];
            if(joint_valid[i][j] > -1){
                if(joint_valid[i][j] == 0){
                    cv::circle(img,jointPts[i][j],5,Scalar(128,128,128),1,CV_AA);
                }else if(joint_valid[i][j] == 1){
                    cv::circle(img,jointPts[i][j],5,Scalar(colori.b,colori.g,colori.r),1,CV_AA);
                }

                cv::line(img,jointPts[i][j],last_validpt,Scalar(colori.b,colori.g,colori.r),1,CV_AA);

                QVector2D vec_dir(jointPts[i][j].x - last_validpt.x,jointPts[i][j].y - last_validpt.y);
                QVector3D vec_dir_3(vec_dir.x(),vec_dir.y(),0);
                vec_dirs.push_back(vec_dir_3);

                last_validpt = jointPts[i][j];
            }
        }

//        qDebug()<<"finger "<<i<<"vecsize="<<vec_dirs.size();

        if(vec_dirs.size() == 2){
            double rad = acos(QVector3D::dotProduct(vec_dirs[0],vec_dirs[1]) / vec_dirs[0].length()/vec_dirs[1].length());
            double cross = QVector3D::crossProduct(vec_dirs[0],vec_dirs[1]).z();
            if(cross > 0)
                finger_rotate[i] = 1;
            else
                finger_rotate[i] = 0;

        }else if(vec_dirs.size() == 3){
            double rad1 = acos(QVector3D::dotProduct(vec_dirs[0],vec_dirs[1]) / vec_dirs[0].length()/vec_dirs[1].length());
            double rad2 = acos(QVector3D::dotProduct(vec_dirs[1],vec_dirs[2]) / vec_dirs[1].length()/vec_dirs[2].length());
            double cross1 = QVector3D::crossProduct(vec_dirs[0],vec_dirs[1]).z();
            double cross2 = QVector3D::crossProduct(vec_dirs[1],vec_dirs[2]).z();
            if(cross1 > 0 && cross2 > 0)
                finger_rotate[i] = 1;
            else if(cross1 < 0 && cross2 < 0)
                finger_rotate[i] = 0;
            else if(rad1 > rad2){
                if(cross1 > 0)
                    finger_rotate[i] = 1;
                else
                    finger_rotate[i] = 0;
            }
            else{
                if(cross2 > 0)
                    finger_rotate[i] = 1;
                else
                    finger_rotate[i] = 0;
            }
        }else if(vec_dirs.size() == 4){
            double rad[3] = {acos(QVector3D::dotProduct(vec_dirs[0],vec_dirs[1]) / vec_dirs[0].length()/vec_dirs[1].length()),
                             acos(QVector3D::dotProduct(vec_dirs[1],vec_dirs[2]) / vec_dirs[1].length()/vec_dirs[2].length()),
                             acos(QVector3D::dotProduct(vec_dirs[2],vec_dirs[3]) / vec_dirs[2].length()/vec_dirs[3].length())};
            double cross[3] = {QVector3D::crossProduct(vec_dirs[0],vec_dirs[1]).z(),\
            QVector3D::crossProduct(vec_dirs[1],vec_dirs[2]).z(),\
            QVector3D::crossProduct(vec_dirs[2],vec_dirs[3]).z()};

            int count = 0;
            for(int i = 0; i < 3; i ++){
                if(cross[i] > 0)
                    count ++;
            }

            if(count >= 2)
                finger_rotate[i] = 1;
            else if(count <= 1)
                finger_rotate[i] = 0;
        }else{
            qDebug()<<"error: "<<vec_dirs.size()<<"rotate found";
        }
        if(finger_rotate[i] == 1){
            cv::line(img,last_validpt,last_validpt + Point(50,0),Scalar(colori.b,colori.g,colori.r),2,CV_AA);
        }else{
            cv::line(img,last_validpt,last_validpt + Point(-50,0),Scalar(colori.b,colori.g,colori.r),2,CV_AA);
        }
        qDebug()<<"finger "<<i<<"rotate ="<<finger_rotate[i];

        if(thumbIdx > -1 && i !=0 && joint_valid[i][3] == 1){
            if(finger_rotate[i] == 1){
                rotate_clock ++;
            }else{
                rotate_anti ++;
            }
        }

    }

    imwrite(folder_path + "hand_joints_g.png",img);

    if(rotate_clock == rotate_anti)
        qDebug()<<"error: cant find rotate"<<rotate_clock<<rotate_anti;
    else{
        qDebug()<<"find rotate"<<rotate_clock<<rotate_anti;

        int valid_dir = rotate_clock > rotate_anti? 1:0;

        for(int i = 0;i < 5; i ++){
            if(thumbIdx > -1 && i == 0)
                continue;

            if(joint_valid[i][3] != 1){
                if(finger_rotate[i] != valid_dir){
                    qDebug()<<"finger"<<i<<"rotate valid";

                    swap(jointPts[i][0],jointPts[i][3]);
                    swap(jointPts[i][1],jointPts[i][2]);
                    swap(joint_valid[i][0],joint_valid[i][3]);
                    swap(joint_valid[i][1],joint_valid[i][2]);
                    while(joint_valid[i][3] == -1){
                        qDebug()<<"finger"<<i<<"exchange valid";
                        joint_valid[i][3] = joint_valid[i][2];
                        joint_valid[i][2] = joint_valid[i][1];
                        joint_valid[i][1] = joint_valid[i][0];
                        joint_valid[i][0] = -1;

                        jointPts[i][3] = jointPts[i][2];
                        jointPts[i][2] = jointPts[i][1];
                        jointPts[i][1] = jointPts[i][0];
                        jointPts[i][0] = fakeend;
                    }
                }
            }else if(finger_rotate[i] != valid_dir){

                vector<Point> neighbor_points = { Point(-1,-1),Point(0,-1),Point(1,-1),Point(1,0),Point(1,1),Point(0,1),Point(-1,1),Point(-1,0) };

                if(joint_valid[i][2] != 1){

                    int newdir = finger_rotate[i];
                    int range = 1;
                    int maxRange = 15;
                    Point newpt;
                    while(newdir != valid_dir && range < maxRange){

                        for(int k = 0; k < neighbor_points.size();k++){
                            newpt = jointPts[i][2] + neighbor_points[k] * range;
                            QVector3D vec1(newpt.x - centerPt.x,newpt.y - centerPt.y,0);
                            QVector3D vec2(jointPts[i][3].x - newpt.x,jointPts[i][3].y  - newpt.y,0);
                            if(QVector3D::crossProduct(vec1,vec2).z() > 0){
                                newdir = 1;
                            }else{
                                newdir = 0;
                            }
                        }
                        range ++;
                    }

                    if(newdir == valid_dir){
                        qDebug()<<"new dire"<<i<<jointPts[i][2].x<<jointPts[i][2].y<<newpt.x<<newpt.y;
                        jointPts[i][2] = newpt;
                    }else{
                        qDebug()<<"new dire";
                    }
                }
            }
        }
    }

}

void HandStructure2D::calGesture(){

//    indexofImg = 2;

    folder_path = "/Users/ebao/study/lab/Gesture/images/gesture" + to_string(indexofImg) + "/";

    QString filepath = QString::fromStdString(folder_path) + "parameters.txt";

    readFile(filepath, fingerlines, fingerJointsIdx,centerPt,thumbIdx,adist);

    center.setX(centerPt.x);
    center.setY(centerPt.y);

    scale = adist/length[2][2];

    for(int i = 0; i < 5;i ++){
        Finger2D *f = new Finger2D;
        for(int j = 0;j < 4;j ++){
            f->length[j] = scale * length[i][j];
            f->Dof[j] = 0.0f;
        }
        f->Dof[0] = angle[i] / 180.0f * CV_PI;
        fingers[i] = f;
    }

    if(thumbIdx < -1){
        Finger2D *f = fingers[0];
        for(int j = 0;j < 4;j ++){
            f->length[j] = scale * length[1][j];
            f->Dof[j] = 0.0f;
        }
    }

    initPoints();

    fitSkeleton();

    calPoints();

    adjustSkeleton();

    drawPoints();

    filepath = QString::fromStdString(folder_path) + "skeleton.txt";
    writeFile(filepath);
}

void HandStructure2D::sortFingers(){
    qDebug()<<"------------sortFingers------------";

    if(thumbIdx >= 0 && thumbIdx < fingerlines.size()){

    }else{

        float fingerAngle[5];
        qDebug()<<"before sort";

        for(int i = 0;i < 5; i ++){
            Finger2D *f = fingers[i];
            fingerAngle[i] = f->Dof[0];
            //        qDebug()<<"finger "<<i<<"="<<f->Dof[0];
        }

        qsort(fingerAngle, 5,sizeof(float),comparef);  //from big to small
        qDebug()<<fingerAngle[0]<<fingerAngle[1]<<fingerAngle[2]<<fingerAngle[3]<<fingerAngle[4];

        Finger2D *tmpFingers[5];
        for(int i = 0;i < 5; i ++){
            for(int j = 0; j < 5; j ++){
                Finger2D *f = fingers[j];
                float value = f->Dof[0];
                if(fabs(value - fingerAngle[i]) < 1e-6){
                    tmpFingers[i] = f;
                    qDebug()<<"finger "<<j;
                    break;
                }
            }
        }

        float fingerDofSum[5];
        int maxDofIdx = 0;
        float maxDofSum = 0.0;
        for(int i = 0; i < 5; i ++){
            fingerDofSum[i] = 0.0;

            for(int j = 0;j < 5; j ++){
                if(i == j)
                    continue;

                float tmpDof = fabs(tmpFingers[j]->Dof[0] - tmpFingers[i]->Dof[0]);
                if(tmpDof > CV_PI)
                    tmpDof = 2 * CV_PI - tmpDof;
                fingerDofSum[i] += tmpDof;
            }
            if(fingerDofSum[i] > maxDofSum){
                maxDofSum = fingerDofSum[i];
                maxDofIdx = i;
            }
        }


        if(maxDofIdx != 0){
            qDebug()<<"exchange finger 0 "<<maxDofIdx;
            Finger2D *tmpF = tmpFingers[0];
            tmpFingers[0] =  tmpFingers[maxDofIdx];
            tmpFingers[maxDofIdx] =  tmpF;
        }

        for(int i = 0; i < 5; i ++){
            fingers[i] = tmpFingers[i];
        }

//        for(int i = 0; i < 4; i ++){
//            fingers[0]->length[i] = scale * length[0][i];
//        }
    }

    basisThumbAngle = fingers[0]->Dof[0];
    float fingerAngle[4];
    //    qDebug()<<"before sort";

    for(int i = 1;i < 5; i ++){
        Finger2D *f = fingers[i];
        fingerAngle[i - 1] = fabs(f->Dof[0] - basisThumbAngle);
        if(fingerAngle[i - 1] > CV_PI)
            fingerAngle[i - 1] = 2 * CV_PI - fingerAngle[i - 1];
        //        qDebug()<<"finger "<<i<<"="<<f->Dof[0];
    }

    qsort(fingerAngle, 4,sizeof(float),comparef);  //from big to small
    qDebug()<<fingerAngle[0]<<fingerAngle[1]<<fingerAngle[2]<<fingerAngle[3];

    Finger2D *tmpFingers[4];
    for(int i = 0;i < 4; i ++){
        for(int j = 1; j < 5; j ++){
            Finger2D *f = fingers[j];
            float value = fabs(f->Dof[0] - basisThumbAngle);
            if(value > CV_PI)
                value = 2 * CV_PI - value;

            if(fabs(value - fingerAngle[i]) < 1e-6){
                tmpFingers[i] = f;
            }
        }
    }
    for(int i = 1; i < 5; i ++){
        fingers[i] = tmpFingers[i - 1];
    }
}

void HandStructure2D::initPoints_parb(){
    qDebug()<<"------------initPoints_parb------------";



}

void HandStructure2D::initPoints(){
    qDebug()<<"------------initPoints------------";

    vector<vector<int>> fingerJointsValid;

    for(int i = 0;i < fingerJointsIdx.size();i++){
        vector<int > joints_valid;
        for(int j = 0; j < fingerJointsIdx[i].size(); j ++){
            if(fingerJointsIdx[i][j] == fakePointIdx){
                joints_valid.push_back(-1);
            }else{
                joints_valid.push_back(1);
            }
        }
        fingerJointsValid.push_back(joints_valid);
    }

    for(int i = 0;i < fingerJointsIdx.size();i++){

//        if(i == thumbIdx){
//            continue;

        int minLen = 10;

        if(fingerJointsIdx[i][0] == fakePointIdx){

            if(fingerJointsIdx[i][1] > minLen){
                int idxx = fingerJointsIdx[i][1] - scale;
                int maxidx = 3;
                fingerJointsIdx[i][0] = max(maxidx,idxx);
                fingerJointsValid[i][0] = 0;

            }
            qDebug()<<"changechange head";
        }

        if(fingerJointsIdx[i].size() <= 3 && \
                fabs(fingerlines[i].size() - 1 - fingerJointsIdx[i][fingerJointsIdx[i].size() - 1]) > minLen){
            int idxx = fingerJointsIdx[i][fingerJointsIdx[i].size() - 1] + scale;
            int maxidx = fingerlines[i].size() - 3;
            fingerJointsIdx[i].push_back(min(maxidx,idxx));
            fingerJointsValid[i].push_back(0);
            qDebug()<<"pushpushpush end";
        }

//        if(fingerJointsIdx[i].size() <= 3 && fingerJointsIdx[i].size() > 0
//                && fabs(fingerJointsIdx[i][0]) > minLen){
//            int idxx = fingerJointsIdx[i][0] - scale;
//            int maxidx = 3;
//            fingerJointsIdx[i].insert(fingerJointsIdx[i].begin(),max(maxidx,idxx));
//            fingerJointsValid[i].insert(fingerJointsValid[i].begin(),0);

//            qDebug()<<"pushpushpush head";
//        }

//        if(fingerJointsIdx[i].size() == 1){
//            fingerJointsIdx[i].push_back(fingerlines[i].size() - 1);
//        }
    }

    for(int i = 0;i < fingerJointsIdx.size();i++){
        while(fingerJointsIdx[i].size() < 4){
            fingerJointsIdx[i].push_back(fakePointIdx);
            fingerJointsValid[i].push_back(-1);
        }
    }

    if(thumbIdx >= 0 && thumbIdx < fingerlines.size()){
        deque<Point> line = fingerlines[thumbIdx];
        vector<int> idxs = fingerJointsIdx[thumbIdx];
        vector<int> valids = fingerJointsValid[thumbIdx];
        for(int j = 0; j < idxs.size(); j ++){
            Point pt;
            int valid;
            if(idxs[j] == fakePointIdx){
                pt = fakeend;
                valid = -1;
            }else{
                pt = line[idxs[j]];
                valid = valids[j];
            }
            jointPts[0][3 - j] = pt;
            joint_valid[0][3 - j] = valid;
        }

        int count = 1;
        for(int i = 0; i < fingerlines.size(); i ++){
            if(i == thumbIdx)
                continue;
            deque<Point> line = fingerlines[i];
            vector<int> idxs = fingerJointsIdx[i];
            vector<int> valids = fingerJointsValid[i];

            for(int j = 0; j < idxs.size(); j ++){
                Point pt;
                int valid;
                if(idxs[j] == fakePointIdx){
                    pt = fakeend;
                    valid = -1;
                }else{
                    pt = line[idxs[j]];
                    valid = valids[j];
                }
                jointPts[count][3 - j] = pt;
                joint_valid[count][3 - j] = valid;
            }
            count ++;
        }

    }else{
        qDebug()<<"error: thumb idx = "<<thumbIdx<<"from 0 to "<<fingerlines.size();
        Finger2D *f0 = fingers[0];
        Finger2D *f1 = fingers[1];
        for(int i = 0; i < 4; i ++){
            f0->length[i] = f1->length[i];
        }

        for(int i = 0; i < fingerlines.size(); i ++){
            deque<Point> line = fingerlines[i];
            vector<int> idxs = fingerJointsIdx[i];
            vector<int> valids = fingerJointsValid[i];

            for(int j = 0; j < idxs.size(); j ++){
                Point pt;
                int valid;
                if(idxs[j] == fakePointIdx){
                    pt = fakeend;
                    valid = -1;
                }else{
                    pt = line[idxs[j]];
                    valid = valids[j];
                }
                jointPts[i][3 - j] = pt;
                joint_valid[i][3 - j] = valid;
            }
        }
    }

    findRoot();

    Mat img = imread(folder_path + "gesture.jpg");

    qDebug()<<"draw exchange finger";

    cv::circle(img,centerPt,5,Scalar(paintColor[5].b,paintColor[5].g,paintColor[5].r),1,CV_AA);

    for(int i = 0; i < 5; i ++){
        Color colori = paintColor[i];
        Point last_validpt = centerPt;

        for(int j = 0; j < 4; j ++){
//            qDebug()<<joint_valid[i][j];
            if(joint_valid[i][j] > -1){
                if(joint_valid[i][j] == 0){
                    cv::circle(img,jointPts[i][j],5,Scalar(128,128,128),1,CV_AA);
                }else if(joint_valid[i][j] == 1){
                    cv::circle(img,jointPts[i][j],5,Scalar(colori.b,colori.g,colori.r),1,CV_AA);
                }

                cv::line(img,jointPts[i][j],last_validpt,Scalar(colori.b,colori.g,colori.r),1,CV_AA);

                last_validpt = jointPts[i][j];
            }else{
                cv::circle(img,last_validpt + Point(10,0),5,Scalar(30,30,30),1,CV_AA);
            }

        }
    }

    imwrite(folder_path + "finger_rotate.png",img);

    syncVec();

//    calBLine();
}


void HandStructure2D::fitSkeleton_parb(){
    qDebug()<<"------------fitSkeleton_parb------------";

    Mat img = imread(folder_path + "gesture.jpg");

    for(int i = 0; i < 5; i ++){
        QVector2D pt[5] = {center,joints[i][0],joints[i][1],joints[i][2],joints[i][3]};
        qDebug()<<"joints  "<<i<<"="<<joints[i][0];

        for(int j = 0; j < 5; j ++){
            QVector2D pt1 = pt[j];
            QVector2D pt2 = pt[j + 1];

//            if(pt1 == fakeendvec || pt2 == fakeendvec){
//                dir_valid[j] = 0;
//                qDebug()<<i<<j<<pt1<<pt2;
//            }else{

        }
    }

}

void HandStructure2D::fitSkeleton(){
    qDebug()<<"------------fitSkeleton------------";

    Mat img = imread(folder_path + "gesture.jpg");

    QFile f("/Users/ebao/study/lab/Gesture/images/valid.txt");
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Open failed.";
        return;
    }
    QTextStream txtOutput(&f);
    txtOutput<<"gesture "<<indexofImg<<"\n";

//    double std_finger_length;
//    double std_root_length;
//    for(int i = 0;i < 5; i ++){
//        QVector2D pt[5] = {center,joints[i][0],joints[i][1],joints[i][2],joints[i][3]};
//        int valid[5] = {2,joint_valid[i][0]+1,joint_valid[i][1]+1,joint_valid[i][2]+1,joint_valid[i][3]+1};

//        if()
//        for(int j = 0;j < 4; j ++){
//            joint_valid[i][0]

//        }


//    }

    for(int i = 0; i < 5; i ++){
//        txtOutput<<"finger "<<i<<"\n";

        QVector2D pt[5] = {center,joints[i][0],joints[i][1],joints[i][2],joints[i][3]};
        qDebug()<<"joints  "<<i<<"="<<joints[i][0];

        QVector2D dir[4];

        double rad[4] = {0,0,0,0};
        int valid[5] = {2,joint_valid[i][0]+1,joint_valid[i][1]+1,joint_valid[i][2]+1,joint_valid[i][3]+1};
        Finger2D *f = fingers[i];

        float len[4] = {f->length[0],f->length[1],f->length[2],f->length[3]};

        QVector2D last_dir(1,0);

        int dir_valid[4] = {1,1,1,1};
        int amount = 0;
        for(int j = 0; j < 4; j ++){
            QVector2D pt1 = pt[j];
            QVector2D pt2 = pt[j + 1];            

            if(valid[j] == 0 || valid[j+1] == 0){
                dir_valid[j] = 0;
                qDebug()<<i<<j<<pt1<<pt2;
            }else{
                dir[j] = pt2 - pt1;
                len[j] = dir[j].length();
                dir[j].normalize();
                last_dir.normalize();
                rad[j] = acos(QVector2D::dotProduct(dir[j],last_dir));


                QVector3D dir3D_1(last_dir,0);
                QVector3D dir3D_2(dir[j],0);

                last_dir = dir[j];

                double cross = QVector3D::crossProduct(dir3D_1,dir3D_2).z();
                if(cross < 0)
                    rad[j] = - rad[j];

//                qDebug()<<dir[j]<<"acos ="<<rad[j];

//                if(dir[j].y() < 0){
//                    rad[j] = - rad[j];
//                    qDebug()<<"re acos ="<<rad[j];
//                }else{
//                    qDebug()<<"no acos";
//                }

                double all_rad = 0.0;
                for(int mm = 0;mm <= j;mm ++){
                    all_rad += rad[mm];
                }
                Point pppt1(pt1.x(),pt1.y());
                Point pppt2(pt1.x()+ 20 * cos(all_rad),pt1.y() + 20 * sin(all_rad));
//                qDebug()<<"draw from"<<pppt1.x<<pppt1.y<<"to "<<pppt2.x<<pppt2.y;

//                qDebug()<<"rad["<<j<<"]="<<rad[j]<<cos(rad[j])<<sin(rad[j])<<"scale = "<<scale;

                line(img,pppt1,pppt2,Scalar(paintColor[i].b,paintColor[i].g,paintColor[i].r),1,CV_AA);
                circle(img,pppt1,3,Scalar(0,255,0));
                circle(img,pppt2,3,Scalar(0,0,128));

            }

            amount += dir_valid[j];
        }

        qDebug()<<"amount"<<amount;
        //all 4 points found

        int amount_b = dir_valid[3] * 2* 2* 2 + dir_valid[2] * 2* 2 + dir_valid[1] * 2 + dir_valid[0];
        int valid_x = valid[4] * pow(10,3) + valid[3] * pow(10,2) + valid[2] * 10 + valid[1];

        txtOutput<<valid_x<<"\t";

        qDebug()<<"amount_b="<<amount_b<<"valid_x="<<valid_x;

        QVector2D empdir;
        float empradave;
        float emprad;
        double std_lenth;


        switch(valid_x){

        //0111 1111
        case 2222:
            break;

        case 2100:
        {
            QVector2D dir_43 = pt[3] - pt[4];
            len[3] = (f->length[3] + dir_43.length())*0.5;
            dir_43.normalize();
            pt[3] =  pt[4] + dir_43 * len[3];
        }
        case 2200:{
            QVector2D dir_43 = pt[3] - pt[4];
            len[3] = (f->length[3] + dir_43.length())*0.5;
        }
        case 2210:
        case 1210:
        {
            double std_len = len[3];

            QVector2D dir_43 = pt[3] - pt[4];
            dir_43.normalize();
            QVector2D dir_34 = -dir_43;
            QVector2D dir_03 = pt[3] - pt[0];
            dir_03.normalize();
            QVector2D dir_30 = -dir_03;
//            QVector2D dir_23 = pt[3] - pt[2];
//            dir_23.normalize();
//            QVector2D dir_32 = -dir_23;

            QVector2D startVec1 = dir_43;
            QVector2D endVec1 = dir_30;

            QVector3D startVec1_3D(startVec1,0);
            QVector3D endVec1_3D(endVec1,0);

            double radrange1 = acos(QVector2D::dotProduct(startVec1,endVec1));

            double cross1 = QVector3D::crossProduct(startVec1_3D,endVec1_3D).z();

            double angleRange1;

            if(cross1 < 0)
                angleRange1 = -radrange1/CV_PI * 180;
            else
                angleRange1 = radrange1/CV_PI * 180;


//            endVec1 = dir_32;

//            double radrange2 = acos(QVector2D::dotProduct(startVec1,endVec1));

//            double cross2 = QVector3D::crossProduct(startVec1_3D,endVec1_3D).z();

//            double angleRange2;

//            if(cross2 < 0)
//                angleRange2 = -radrange2/CV_PI * 180;
//            else
//                angleRange2 = radrange2/CV_PI * 180;

//            if(2 * fabs(angleRange2) < fabs(angleRange1)){
//                qDebug()<<"range"<<2 * fabs(angleRange2) <<"< "<<angleRange1;
////                angleRange1 = 2 * angleRange2;
//            }

            int stepN1 = 10;
            double angleStep1 = angleRange1/stepN1;

            Mat img = imread(folder_path + "gesture.jpg");

            startVec1_3D *= std_len;
            double minDist_out = 1e6;
            for(int k = 0;k < stepN1; k ++){
                double rotateAngle1 = angleStep1 * k;
                QMatrix4x4 m_rotate;
                m_rotate.setToIdentity();
                m_rotate.rotate(rotateAngle1,0,0,1);
                QVector3D tmpVec1_3D = m_rotate * startVec1_3D;
                QVector2D tmpVec1_2D(tmpVec1_3D.x(),tmpVec1_3D.y());
                QVector2D p2 = pt[3] + tmpVec1_2D;
                QVector2D dir_32 = tmpVec1_2D;//(tmpVec3D.x(),tmpVec3D.y());
                dir_32.normalize();
                QVector2D dir_23 = - dir_32;

                //find pt1
                QVector2D dir_20 = pt[0] - p2;
                dir_20.normalize();
//                QVector2D dir_32 = p2 - pt[3];
//                dir_32.normalize();

                QVector2D startVec = dir_20;
                QVector2D endVec = dir_32;

                QVector3D startVec3D(startVec,0);
                QVector3D endVec3D(endVec,0);

                double radrange = acos(QVector2D::dotProduct(startVec,endVec));

                double cross = QVector3D::crossProduct(startVec3D,endVec3D).z();

                double angleRange;

                if(cross < 0)
                    angleRange = -radrange/CV_PI * 180;
                else
                    angleRange = radrange/CV_PI * 180;

                int stepN = 5;
                double angleStep = angleRange/stepN;

//                QVector2D dir_23 = - dir_32;
                startVec3D.normalize();
                startVec3D *= std_len;
                double minDist = minDist_out;

                for(int kk = 0;kk < stepN; kk ++){
                    double rotateAngle = angleStep * kk;
                    QMatrix4x4 m_rotate;
                    m_rotate.setToIdentity();
                    m_rotate.rotate(rotateAngle,0,0,1);
                    QVector3D tmpVec3D = m_rotate * startVec3D;
                    QVector2D tmpVec2D(tmpVec3D.x(),tmpVec3D.y());
                    QVector2D p1 = pt[2] + tmpVec2D;
                    QVector2D dir_21 = tmpVec2D;//(tmpVec3D.x(),tmpVec3D.y());
                    dir_21.normalize();
                    QVector2D dir_12 = - dir_21;
                    QVector2D dir_10 = pt[0] - p1;
                    dir_10.normalize();
                    dir_23.normalize();
                    dir_32 = -dir_23;
                    dir_34.normalize();
                    double rad123 = acos(QVector2D::dotProduct(dir_21,dir_23));
                    double rad012 = acos(QVector2D::dotProduct(dir_12,dir_10));
                    double rad234 = acos(QVector2D::dotProduct(dir_34,dir_32));
                    qDebug()<<"rate in = "<<rad123/rad012;
                    double tmpDist = fabs(rad123 - rad012) + fabs(rad123 - rad234);
                    if(tmpDist < minDist){
                        minDist = tmpDist;
                        qDebug()<<"good dist in = "<<rad123<<rad012<<rad234;
                        pt[1] = p1;
                    }

                    circle(img,Point(p1.x(),p1.y()),2,Scalar(paintColor[i + 1].b,paintColor[i + 1].g,paintColor[i + 1].r),1);

                }

                if(minDist < minDist_out){
                    minDist_out = minDist;
                    pt[2] = p2;
                }

                circle(img,Point(p2.x(),p2.y()),2,Scalar(paintColor[i].b,paintColor[i].g,paintColor[i].r),-1);
            }

            imwrite(folder_path + "finger2210.png",img);

        }
            break;

        case 2220:
        case 1220:
        case 2221:
        case 1221:
        {
            qDebug()<<valid_x<<" hhh";
            double len12 = (len[2] + len[3]) * 0.5;

            QVector2D dir_20 = pt[0] - pt[2];
            dir_20.normalize();
            QVector2D dir_32 = pt[2] - pt[3];
            dir_32.normalize();


            QVector2D startVec = dir_20;
            QVector2D endVec = dir_32;


            QVector3D startVec3D(startVec,0);
            QVector3D endVec3D(endVec,0);

            double radrange = acos(QVector2D::dotProduct(startVec,endVec));

            double cross = QVector3D::crossProduct(startVec3D,endVec3D).z();

            double angleRange;

            if(cross < 0)
                angleRange = -radrange/CV_PI * 180;
            else
                angleRange = radrange/CV_PI * 180;


            int stepN = 10;
            double angleStep = angleRange/stepN;

            QVector2D dir_23 = - dir_32;
            startVec3D.normalize();
            startVec3D *= len12;
            double minDist = 1e6;
            int found = 0;

            Mat img = imread(folder_path + "gesture.jpg");
            for(int k = 0;k < stepN; k ++){
                double rotateAngle = angleStep * k;
                QMatrix4x4 m_rotate;
                m_rotate.setToIdentity();
                m_rotate.rotate(rotateAngle,0,0,1);
                QVector3D tmpVec3D = m_rotate * startVec3D;
                QVector2D tmpVec2D(tmpVec3D.x(),tmpVec3D.y());
                QVector2D p1 = pt[2] + tmpVec2D;
                QVector2D dir_21 = tmpVec2D;//(tmpVec3D.x(),tmpVec3D.y());
                dir_21.normalize();
                QVector2D dir_12 = - dir_21;
                QVector2D dir_10 = pt[0] - p1;
                dir_10.normalize();
                dir_23.normalize();
                double rad123 = acos(QVector2D::dotProduct(dir_21,dir_23));
                double rad012 = acos(QVector2D::dotProduct(dir_12,dir_10));
                qDebug()<<"rate = "<<rad123/rad012;
                if(fabs(rad123 - rad012) < minDist){
                    minDist = fabs(rad123 - rad012);
                    qDebug()<<"good dist = "<<rad123<<"-"<<rad012<<rad123 - rad012;
                    pt[1] = p1;
                }

                circle(img,Point(p1.x(),p1.y()),2,Scalar(paintColor[i].b,paintColor[i].g,paintColor[i].r),1);

                imwrite(folder_path + "finger2220.png",img);

            }
        }
            break;

        default:
            qDebug()<<"no solution found:"<<valid_x;

//        case 2222:
//            break;
//        case 2222:
//            break;
//        case 2222:
//            break;
        }


        //rotate rad[2] cross
        last_dir = QVector2D(1,0);

        for(int j = 0;j < 4;j++){

            QVector2D pt1 = pt[j];
            QVector2D pt2 = pt[j + 1];

            dir[j] = pt2 - pt1;
            len[j] = dir[j].length();
            dir[j].normalize();
            last_dir.normalize();
            rad[j] = acos(QVector2D::dotProduct(dir[j],last_dir));

            QVector3D dir3D_1(last_dir,0);
            QVector3D dir3D_2(dir[j],0);

            last_dir = dir[j];

            double cross = QVector3D::crossProduct(dir3D_1,dir3D_2).z();
            if(cross < 0)
                rad[j] = - rad[j];
        }


        /*
        switch(amount_b){

        //end -3 -2-1-0-root
        //0000
        case 0:
            qDebug()<<"error: no direction found in finger i"<<i;
            break;
            //0001
        case 1:
            break;
            //0010
        case 2:
            break;
            //0100
        case 4:
            break;
            //1000
        case 8:
            qDebug()<<"//1000";

//            std_lenth = fingers[i]->length[3];
//            if(valid[2] == 1){
//                std_lenth = len[3];


//            }


//            if(valid[2] == 1){

                empdir = pt[4] - pt[0];
                emprad = acos(QVector2D::dotProduct(empdir,QVector2D(1,0)) / empdir.length());
                if(empdir.y() < 0){
                    emprad = 2 * CV_PI - emprad;
                }

                qDebug()<<"rad[3] = "<<rad[3]<<"emprad"<<emprad;
                if(rad[2] - emprad > CV_PI){
                    empradave = (2 * CV_PI - (rad[3] - emprad))/ 4.0;
                    rad[0] = emprad + empradave;
                    rad[1] = emprad;
                    rad[2] = emprad - empradave;
                }else{
                    empradave = (rad[3] - emprad)/ 4.0;
                    rad[0] = emprad - empradave;
                    rad[1] = emprad;
                    rad[2] = emprad + empradave;
                }

                if(valid[3] != 1 || valid[2] != 1){
                    len[3] = len[3];

                }else{

                }

//            }


            break;

        //0011 1+ 2
        case 3:
            rad[2] = rad[1] + rad[1] - rad[0];
            rad[3] = rad[2] + rad[2] - rad[1];
            break;
            //0101 1 + 4
        case 5:
            rad[1] = (rad[0] + rad[2]) * 0.5;
            rad[3] = rad[2] + rad[2] - rad[1];
            break;
            //0110 2 + 4
        case 6:
            rad[0] = rad[1] - (rad[2] - rad[1]);
            rad[3] = rad[2] + rad[2] - rad[1];
            break;
            //1001 1 + 8
        case 9:
            rad[1] = rad[0] + (rad[3] - rad[0]) / 3.0;
            rad[2] = rad[3] - (rad[3] - rad[0]) / 3.0;
            break;
            //1010 2 + 8
        case 10:
            rad[2] = (rad[1] + rad[3]) * 0.5;
            rad[0] = rad[1] - (rad[2] - rad[1]);
            break;
            //1100 4 + 8
        case 12:
            qDebug()<<"1100 i = "<<i;
            empdir = pt[3] - pt[0];
            emprad = acos(QVector2D::dotProduct(empdir,QVector2D(1,0)) / empdir.length());

            if(empdir.y() < 0){
                emprad = 2 * CV_PI - emprad;
            }

            qDebug()<<"rad[2] = "<<rad[2]<<"emprad"<<emprad;
            if(rad[2] - emprad > CV_PI){
                empradave = (2 * CV_PI - (rad[2] - emprad))/ 3.0;
                rad[0] = emprad + empradave;
                rad[1] = emprad - empradave;
            }else{
                empradave = (rad[2] - emprad)/ 3.0;
                rad[0] = emprad - empradave;
                rad[1] = emprad + empradave;
            }

            break;
            //1111
        case 15:
            qDebug()<<"finger" <<i<< "has full joints";

            break;
        default:
            qDebug()<<"error: finger" <<i<<"amount ="<<  amount_b;
            break;
        };
        */



        for(int j = 0; j < 4; j ++){
//            if(j > 0)
//                f->Dof[j] = rad[j] - rad[j - 1];
//            else
                f->Dof[j] = rad[j] - 0;

            f->length[j] = len[j];
        }

        txtOutput<<"\n";

    }

    f.close();
    sortFingers();

//    for(int i = 1; i < 5; i ++){
//        Finger2D *f = fingers[i];
//        qDebug()<<"finger "<<i<<"="<<f->Dof[0];
//    }

    imwrite(folder_path + "hand_conect.png",img);

}


void HandStructure2D::calPoints(){

    qDebug()<<"------------calPoints------------";
    qDebug()<<"cos 30 ="<<cos(30)<<"cos PI/6 = "<<cos(CV_PI /6.0);

    for(int i = 0;i < 5;i ++){
        Finger2D *f = fingers[i];
        QVector2D dirvec0(cos(f->Dof[0]),sin(f->Dof[0]));
        dirvec0.normalize();
        joints[i][0] = center + f->length[0] * dirvec0;
        for(int j = 1;j < 4;j ++){
            float rad = 0;
//            rad = f->Dof[j];
            for(int k = 0; k <= j; k++)
                rad += f->Dof[k];
            QVector2D dirvec(cos(rad),sin(rad));
            joints[i][j] = joints[i][j - 1] + f->length[j] * dirvec;
        }
    }

    syncPts();
}

void HandStructure2D::syncPts(){
    for(int i = 0 ; i < 5; i ++){
        for(int j = 0; j < 4; j ++){
            jointPts[i][j].x = joints[i][j].x();
            jointPts[i][j].y = joints[i][j].y();
        }
    }
    centerPt.x = center.x();
    centerPt.y = center.y();
}

void HandStructure2D::syncVec(){
    qDebug()<<"----------------syncVec----------------";
    for(int i = 0 ; i < 5; i ++){
        qDebug()<<" i ="<<i;
        for(int j = 0; j < 4; j ++){
            joints[i][j].setX(jointPts[i][j].x);
            joints[i][j].setY(jointPts[i][j].y);
            qDebug()<<joints[i][j];
        }
    }
    center.setX(centerPt.x);
    center.setY(centerPt.y);
}

void HandStructure2D::adjustSkeleton(){
    qDebug()<<"-------------adjustSkeleton---------------";

    int notfit = 1;
    int count = 0;
    int minDistance = 1e6;
    Point center_dir[5] = {Point(0,0),Point(1,1),Point(1,0),Point(-1,0),Point(-1,-1)};
    int valid_dir_i = 0;
    while(notfit){
        if(count ++ > -1)
            break;

//        for(int k = 0;k < 5; k ++){

//            centerPt = newcenter;

            int tmpnotfit = 0;


            float radsum = 0;
            float threshold1 = CV_PI / 2.0;
            float threshold2 = CV_PI / 4.0;
            float threshold3 = pow(CV_PI / 6.0,2) * 3;


            for(int i = 0; i < 4; i ++){
                Finger2D *f1 = fingers[i];
                Finger2D *f2 = fingers[i + 1];

                float rad1 = f1->Dof[0];
                float rad2 = f2->Dof[0];
                float deltaRad = fabs(rad2 - rad1);
                if(deltaRad > CV_PI)
                    deltaRad = 2 * CV_PI - deltaRad;

                if(i == 0){
                    if(deltaRad > threshold1){
                        qDebug()<<"threshold1";
                        tmpnotfit = 1;
                    }
                }else{
                    if(deltaRad > threshold2){
                        qDebug()<<"threshold2";
                        tmpnotfit = 1;
                    }
                    radsum += pow(deltaRad, 2);
                }

                qDebug()<<"deltaRad "<<i<<deltaRad;
            }


            if(radsum > threshold3){
                qDebug()<<"threshold3";
                tmpnotfit = 1;
            }
            qDebug()<<"thresh"<<threshold1<<threshold2<<threshold3<<"radsum"<<radsum;
            if(tmpnotfit){
                qDebug()<<"center redefine";
                QVector2D moveDir(0,0);
                for(int i = 0; i < 5; i ++){
                    QVector2D tmpdir = joints[i][0] - center;
                    tmpdir.normalize();
                    moveDir = moveDir + tmpdir;
                }
                moveDir = center - joints[2][0];//-moveDir/5.0/2.0;
                moveDir.normalize();
                //            moveDir =  - joints[2][0] + center;
                //            moveDir.normalize();
                Mat img = imread(folder_path + "gesture.jpg");

                qDebug()<<scale<<moveDir;
                cv::line(img,Point(center.x(),center.y()),Point(center.x() + scale * moveDir.x(),center.y() + scale * moveDir.y()),Scalar(128,128,128));
                char c = '0' + (count - 1);
                imwrite(folder_path+"center_redef"+c+".png",img);
                QVector2D newcenter = center + scale * moveDir;

                initPoints();
                center = newcenter;

                fitSkeleton();
                calPoints();
            }

        notfit = tmpnotfit;
    }
    qDebug()<<"adjust count ="<<count;
}

float HandStructure2D::calVariance(){
    float var = 0.0;
    return var;
}

void HandStructure2D::drawPoints(){
    qDebug()<<"------------drawPoints------------";

    Mat img = imread(folder_path + "gesture.jpg");

    qDebug()<<centerPt.x<<centerPt.y;

    cv::circle(img,centerPt,5,Scalar(paintColor[5].b,paintColor[5].g,paintColor[5].r),1,CV_AA);

    for(int i = 0; i < 5; i ++){
        Color colori = paintColor[i];
        for(int j = 0; j < 4; j ++){
            if(jointPts[i][j] != fakeend){
                cv::circle(img,jointPts[i][j],5,Scalar(colori.b,colori.g,colori.r),1,CV_AA);
                if(j > 0 && jointPts[i][j - 1]!=fakeend){
                    cv::line(img,jointPts[i][j],jointPts[i][j - 1],Scalar(colori.b,colori.g,colori.r),1,CV_AA);
                }else{
                    cv::line(img,jointPts[i][j],centerPt,Scalar(colori.b,colori.g,colori.r),1,CV_AA);
                }
            }
        }
    }

    imwrite(folder_path + "hand_joints.png",img);
    imwrite("/Users/ebao/study/lab/Gesture/images/gestures_pre/skeleton_img" + to_string(indexofImg) + ".png",img);

}

void HandStructure2D::readFile(QString filename,vector<deque<Point>> &fingerlines, vector<vector<int>> &fingerJointsIdx, Point &center, int &thumbIdx, int &adist)
{
    QFile f(filename);
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
    valid_fingerN = lst[0].toInt();

    lineStr = txtInput.readLine();
    lst=lineStr.split('\t');
    center.x = lst[0].toInt();
    center.y = lst[1].toInt();

    thumbIdx = lst[2].toInt();
    adist = lst[3].toInt();

    qDebug()<<"adist"<<adist;

    int countii = 0;
    while(countii < valid_fingerN)
    {
        lineStr = txtInput.readLine();
        lst=lineStr.split('\t');
        deque<Point> fingerline;
        for(int j = 0;j < (lst.size() - 1) / 2;j ++){
            int x = lst[j * 2].toInt();
            int y = lst[j * 2 + 1].toInt();
            fingerline.push_back(Point(x,y));
        }
        fingerlines.push_back(fingerline);
        countii++;
    }

    countii = 0;
    while(countii < valid_fingerN)
    {
        lineStr = txtInput.readLine();
        lst=lineStr.split('\t');
        vector<int> fingerIdx;
        for(int j = 0;j < lst.size() - 1;j ++){
            int idx = lst[j].toInt();
            fingerIdx.push_back(idx);
        }
        fingerJointsIdx.push_back(fingerIdx);

        qDebug()<<"fingerIdx"<<fingerIdx;
        countii++;
    }

//    qDebug()<<"end";


    f.close();
}


void HandStructure2D::writeFile(QString filename)
{
    QFile f(filename);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Open failed.";
        return;
    }
    QTextStream txtOutput(&f);

    //total fingers and thumb
    txtOutput << 5 << "\t" << 0 << "\t"<<basisThumbAngle<<"\n";

    for(int i = 0; i < 5;i ++){
        Finger2D *f = fingers[i];
        for(int j = 0;j < 4; j ++){
            txtOutput << f->Dof[j]<< "\t";
        }
        txtOutput <<"\n";
    }
    f.close();
}
