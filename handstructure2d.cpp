#include "handstructure2d.h"

#include <QVector2D>
#include <QFile>

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

    indexofImg = 4;

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
    basisThumbAngle = fingers[0]->Dof[0];
    float fingerAngle[4];
//    qDebug()<<"before sort";

    for(int i = 1;i < 5; i ++){
        Finger2D *f = fingers[i];
        fingerAngle[i - 1] = fabs(f->Dof[0] - basisThumbAngle);
//        qDebug()<<"finger "<<i<<"="<<f->Dof[0];
    }

    qsort(fingerAngle, 4,sizeof(float),comparef);  //from big to small
    qDebug()<<fingerAngle[0]<<fingerAngle[1]<<fingerAngle[2]<<fingerAngle[3];

    Finger2D *tmpFingers[4];
    for(int i = 0;i < 4; i ++){
        for(int j = 1; j < 5; j ++){
            Finger2D *f = fingers[j];
            float value = fabs(f->Dof[0] - basisThumbAngle);
            if(fabs(value - fingerAngle[i]) < 1e-6){
                tmpFingers[i] = f;
            }
        }
    }
    for(int i = 1; i < 5; i ++){
        fingers[i] = tmpFingers[i - 1];
    }
}

void HandStructure2D::initPoints(){
    qDebug()<<"------------initPoints------------";

    for(int i = 0;i < fingerJointsIdx.size();i++){

        if(i == thumbIdx)
            continue;

        if(fingerJointsIdx[i].size() <= 3 && fabs(fingerlines[i].size() - 1 - fingerJointsIdx[i][fingerJointsIdx[i].size() - 1]) > scale * 0.5){
            fingerJointsIdx[i].push_back(fingerJointsIdx[i][fingerJointsIdx[i].size() - 1] + int(scale * 0.5));
            qDebug()<<"pushpushpush";
        }

        if(fingerJointsIdx[i].size() == 1){
            fingerJointsIdx[i].push_back(fingerlines[i].size() - 1);
        }
    }

    for(int i = 0;i < fingerJointsIdx.size();i++){
        while(fingerJointsIdx[i].size() < 4){
            fingerJointsIdx[i].push_back(fakePointIdx);
        }
    }

    if(thumbIdx >= 0 && thumbIdx < fingerlines.size()){
        deque<Point> line = fingerlines[thumbIdx];
        vector<int> idxs = fingerJointsIdx[thumbIdx];
        for(int j = 0; j < idxs.size(); j ++){
            Point pt;
            if(idxs[j] == fakePointIdx){
                pt = fakeend;
            }else{
                pt = line[idxs[j]];
            }
            jointPts[0][3 - j] = pt;
        }

    }else{
        qDebug()<<"error: thumb idx = "<<thumbIdx<<"from 0 to "<<fingerlines.size();
    }

    int count = 1;
    for(int i = 0; i < fingerlines.size(); i ++){
        if(i == thumbIdx)
            continue;
        deque<Point> line = fingerlines[i];
        vector<int> idxs = fingerJointsIdx[i];
        for(int j = 0; j < idxs.size(); j ++){
            Point pt;
            if(idxs[j] == fakePointIdx){
                pt = fakeend;
            }else{
                pt = line[idxs[j]];
            }
            jointPts[count][3 - j] = pt;
        }
        count ++;
    }
    syncVec();
}


void HandStructure2D::fitSkeleton(){
    qDebug()<<"------------fitSkeleton------------";

    Mat img = imread(folder_path + "gesture.jpg");

    for(int i = 0; i < 5; i ++){
        QVector2D pt[5] = {center,joints[i][0],joints[i][1],joints[i][2],joints[i][3]};
        QVector2D dir[4];

        float rad[4];
        Finger2D *f = fingers[i];

        int dir_valid[4] = {1,1,1,1};
        int amount = 0;
        for(int j = 0; j < 4; j ++){
            QVector2D pt1 = pt[j];
            QVector2D pt2 = pt[j + 1];

            if(pt1 == fakeendvec || pt2 == fakeendvec){
                dir_valid[j] = 0;
                qDebug()<<i<<j<<pt1<<pt2;
            }else{
                dir[j] = pt2 - pt1;
                dir[j].normalize();
                rad[j] = acos(QVector2D::dotProduct(dir[j],QVector2D(1,0)) / dir[j].length());
                qDebug()<<dir[j]<<"acos ="<<rad[j];

                if(dir[j].y() < 0){
                    rad[j] = 2 * CV_PI - rad[j];
                    qDebug()<<"re acos ="<<rad[j];
                }else{
                    qDebug()<<"no acos";
                }

                Point pppt1(pt1.x(),pt1.y());
                Point pppt2(pt1.x()+ 20 * cos(rad[j]),pt1.y() + 20 * sin(rad[j]));
//                qDebug()<<"draw from"<<pppt1.x<<pppt1.y<<"to "<<pppt2.x<<pppt2.y;

//                qDebug()<<"rad["<<j<<"]="<<rad[j]<<cos(rad[j])<<sin(rad[j])<<"scale = "<<scale;

                line(img,pppt1,pppt2,Scalar(255,0,0),1,CV_AA);
                circle(img,pppt1,3,Scalar(0,255,0));
                circle(img,pppt2,3,Scalar(0,0,128));

            }

            amount += dir_valid[j];
        }

        qDebug()<<"amount"<<amount;
        //all 4 points found

        int amount_b = dir_valid[3] * 2* 2* 2 + dir_valid[2] * 2* 2 + dir_valid[1] * 2 + dir_valid[0];

        qDebug()<<"amount_b="<<amount_b;

        QVector2D empdir;
        float empradave;
        float emprad;

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
            empdir = pt[4] - pt[0];
            emprad = acos(QVector2D::dotProduct(empdir,QVector2D(1,0)) / empdir.length());

            if(empdir.y() < 0){
                emprad = 2 * CV_PI - emprad;
            }

            qDebug()<<"rad[2] = "<<rad[3]<<"emprad"<<emprad;
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

        for(int j = 0; j < 4; j ++){
            if(j > 0)
                f->Dof[j] = rad[j] - rad[j - 1];
            else
                f->Dof[j] = rad[j] - 0;
        }

    }

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
//    while(notfit){
//        if(count ++ > 0)
//            break;
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
        moveDir = -moveDir/5.0/2.0;
        Mat img = imread(folder_path + "gesture.jpg");

        qDebug()<<scale<<moveDir;
        cv::line(img,Point(center.x(),center.y()),Point(center.x() + scale * moveDir.x(),center.y() + scale * moveDir.y()),Scalar(128,128,128));
        imwrite(folder_path+"center_redef.png",img);
        center = center + scale * moveDir;

        initPoints();
        fitSkeleton();
        calPoints();
    }
    notfit = tmpnotfit;
//    }
}

void HandStructure2D::drawPoints(){
    qDebug()<<"------------drawPoints------------";

    Mat img = imread(folder_path + "gesture.jpg");

    qDebug()<<centerPt.x<<centerPt.y;

    cv::circle(img,centerPt,5,Scalar(paintColor[5].b,paintColor[5].g,paintColor[5].r),1,CV_AA);

    for(int i = 0; i < 5; i ++){
        Color colori = paintColor[i];
        for(int j = 0; j < 4; j ++){
            cv::circle(img,jointPts[i][j],5,Scalar(colori.b,colori.g,colori.r),1,CV_AA);
            if(j > 0){
                cv::line(img,jointPts[i][j],jointPts[i][j - 1],Scalar(colori.b,colori.g,colori.r),1,CV_AA);
            }else{
                cv::line(img,jointPts[i][j],centerPt,Scalar(colori.b,colori.g,colori.r),1,CV_AA);
            }
        }
    }

    imwrite(folder_path + "hand_joints.png",img);
    imwrite("/Users/ebao/study/lab/Gesture/images/gestures_pre/finger_img" + to_string(indexofImg) + ".png",img);

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
