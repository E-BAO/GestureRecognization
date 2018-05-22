#include "handstructure.h"

#include <QMatrix4x4>
#include <qmath.h>
#include <QFile>
#include <QList>
#include <QString>

HandStructure::HandStructure()
{
    position = QVector3D(0.0f,0.0f,0.0f);
    rotation = QVector3D(0.0f,0.0f,0.0f);

    for(int i = 0; i < 5;i ++){
        Finger *f = new Finger;
        for(int j = 0;j < 4;j ++){
            f->length[j] = length[i][j];
            f->Dof[j] = 0.0f;
        }
        f->rz0 = angle[i];
        fingers[i] = f;
    }

    CalPoints();


    int indexofImg = 9;

    string folder_path = "/Users/ebao/study/lab/Gesture/images/gesture" + to_string(indexofImg) + "/";

    QString filepath = QString::fromStdString(folder_path) + "parameters.txt";

    vector<deque<Point>> fingerlines;
    vector<vector<int>> fingerJointsIdx;

    Point center;
    readFile(filepath, fingerlines, fingerJointsIdx,center,thumbIdx,adist);
    qDebug()<<"center = "<<center.x<<center.y<<"thumb ="<<thumbIdx<<"adist = "<<adist;

    fitSkeleton(fingerlines, fingerJointsIdx,center,thumbIdx,adist);

    CalGesture();
}

void HandStructure::CalPoints()
{    
    qDebug()<<"cal pointshhhh";
    QVector3D joint0(0.0f,0.0f,0.0f);

    for(int i = 0;i < 5; i ++){
        joints[i][0] = joint0;

        float rz0 = fingers[i]->rz0;
        float length = fingers[i]->length[0];
        QVector3D basic(0.0f,length,0.0f); //along y

        QMatrix4x4 m_rotate;
        m_rotate.setToIdentity();
        m_rotate.rotate(rz0,0.0f,0.0f,1.0f);
        QVector3D Skeleton01 = m_rotate * basic;

        QVector3D joint1 = joint0 + Skeleton01;

        joints[i][1] = joint1;

        float rx1 = fingers[i]->Dof[0];
        float rz1 = fingers[i]->Dof[1];
        float rx2 = fingers[i]->Dof[2];
        float rx3 = fingers[i]->Dof[3];

        length = fingers[i]->length[1];
        QVector3D basic1(0.0f,length,0.0f);
//        m_rotate.setToIdentity();
        m_rotate.rotate(rx1,1.0f,0.0f,0.0f);
        m_rotate.rotate(rz1,0.0f,0.0f,1.0f);
        QVector3D Skeleton12 = m_rotate * basic1;

        joints[i][2] = joint1 + Skeleton12;

        length = fingers[i]->length[2];
        QVector3D basic2(0.0f,length,0.0f);
//        m_rotate.setToIdentity();
        m_rotate.rotate(rx2,1.0f,0.0f,0.0f);
        QVector3D Skeleton23 = m_rotate * basic2;

        joints[i][2] = joint1 + Skeleton12;

        length = fingers[i]->length[3];
        QVector3D basic3(0.0f,length,0.0f);
//        m_rotate.setToIdentity();
        m_rotate.rotate(rx3,1.0f,0.0f,0.0f);
        QVector3D Skeleton34 = m_rotate * basic3;

        joints[i][2] = joint1 + Skeleton12;
        joints[i][3] = joints[i][2] + Skeleton23;
        joints[i][4] = joints[i][3] + Skeleton34;
    }
}


void HandStructure::fitSkeleton(vector<deque<Point> > &fingerlines, vector<vector<int> > &fingerJointsIdx, Point &center, int &thumbIdx,int &adist){
    QVector2D *p0 = &points2D[0];
    Point fakePoint(-1,-1);
    int fakePointIdx = -1;
    p0->setX(center.x);
    p0->setY(center.y);

    for(int i = 0;i < fingerJointsIdx.size();i++){
        while(fingerJointsIdx[i].size() < 4){
            fingerJointsIdx[i].push_back(fakePointIdx);
        }
    }

    deque<Point> fingerThumb = fingerlines[thumbIdx];
    vector<int> fingerThumbIdx = fingerJointsIdx[thumbIdx];

    for(int j = 4; j > 0;j --){
        qDebug()<<"line = "<<4 - j<<"idx = "<<fingerThumbIdx[4 - j];
        QVector2D *p = &points2D[j];
        Point pp;
        if(fingerThumbIdx[4 - j] == fakePointIdx){
            pp = fakePoint;
        }else{
            pp = fingerThumb[fingerThumbIdx[4 - j]];
        }
        p->setX(pp.x);
        p->setY(pp.y);
    }

    int count = 0;
    for(int i = 0; i < 5; i ++){
        if(i > fingerlines.size() -  1){
            for(int j = 0;j < 4; j ++){
                Point pp = fakePoint;
                QVector2D *p = &points2D[5 + count * 4 + j];
                p->setX(pp.x);
                p->setY(pp.y);
            }
            count ++;
            continue;
        }

        if(i == thumbIdx)
            continue;

        deque<Point> fingerLine = fingerlines[i];
        vector<int> fingerIdx = fingerJointsIdx[i];
        for(int j = 0;j < 4; j ++){

            qDebug()<<"line = "<<i<<"finger = "<<3 - j<<"idx = "<<fingerIdx[3 - j];

            Point pp;
            QVector2D *p = &points2D[5 + count * 4 + j];
            if(fingerIdx[3 - j] == fakePointIdx)
                pp = fakePoint;
            else
                pp = fingerLine[fingerIdx[3 - j]];

            p->setX(pp.x);
            p->setY(pp.y);
        }
        count++;
    }
}


void HandStructure::readFile(QString filename,vector<deque<Point>> &fingerlines, vector<vector<int>> &fingerJointsIdx, Point &center, int &thumbIdx,int &adist)
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
    int n = lst[0].toInt();

    lineStr = txtInput.readLine();
    lst=lineStr.split('\t');
    center.x = lst[0].toInt();
    center.y = lst[1].toInt();

    thumbIdx = lst[2].toInt();
    adist = lst[3].toInt();
    //    PickPoint *p0 = pickPointArray[0];
//    p0->setGeometry(x,y,p0->width(),p0->height());
//    p0->setVisible(true);

    int countii = 0;
    while(countii < n)
    {
        lineStr = txtInput.readLine();
        lst=lineStr.split('\t');
        deque<Point> fingerline;
        for(int j = 0;j < (lst.size() - 1) / 2;j ++){
//            PickPoint *p = pickPointArray[1+i * 4 + j];
            int x = lst[j * 2].toInt();
            int y = lst[j * 2 + 1].toInt();
            fingerline.push_back(Point(x,y));
//            p->setGeometry(x,y,p->width(),p->height());
//            p->setVisible(true);
        }
        fingerlines.push_back(fingerline);
//        qDebug()<<"fingerline"<<countii<<" size = "<<fingerline.size();
        countii++;
    }
    qDebug()<<"fingerline total"<<fingerlines.size();


    countii = 0;
    while(countii < n)
    {
        lineStr = txtInput.readLine();
        lst=lineStr.split('\t');
        vector<int> fingerIdx;
        for(int j = 0;j < lst.size() - 1;j ++){
//            PickPoint *p = pickPointArray[1+i * 4 + j];
            int idx = lst[j].toInt();
            fingerIdx.push_back(idx);

//            p->setGeometry(x,y,p->width(),p->height());
//            p->setVisible(true);
        }
        fingerJointsIdx.push_back(fingerIdx);
        qDebug()<<"fingerIdx"<<countii<<" size = "<<fingerIdx.size();

        countii++;
    }

    qDebug()<<"fingerJointsIdx total"<<fingerJointsIdx.size();

//    step = JOINTS_NUMBER;

    f.close();
}

void HandStructure::CalGesture()
{
//    QFile f("test.txt");
//    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        qDebug() << "Open failed.";
//        return;
//    }

//    QTextStream txtInput(&f);
//    QString lineStr;
//    lineStr = txtInput.readLine();
//    QList<QString> lst;
//    lst=lineStr.split('\t');
//    float x = lst[0].toInt();
//    float y = lst[1].toInt();

//    points2D[0].setX(x);
//    points2D[0].setY(y);

//    int i = 0;
//    while(!txtInput.atEnd())
//    {
//        lineStr = txtInput.readLine();
//        lst=lineStr.split('\t');
//        for(int j = 0;j < 4;j ++){
//            int x = lst[j * 2].toInt();
//            int y = lst[j * 2 + 1].toInt();
//            points2D[1 + i * 4 + j].setX(x);
//            points2D[1 + i * 4 + j].setY(y);
//        }
//        i++;
//    }
//    f.close();

    Finger *finger = fingers[0];
    QVector2D vec2 = points2D[1] - points2D[0];
    float length3D = fingers[2]->length[2];
//    float scale = length3D / vec2.length();
    float scale = length3D / adist;

//    QVector3D axis_y(0,1,0);
//    QVector3D axis_x(1,0,0);
//    QVector3D axis_z(0,0,1);

//    float z;
//    float temp;// = sqrtf(pow(finger->length[0],2) - pow(vec2.length(),2));

    qDebug()<<"cal gesture";
    QVector2D fakepoint(-1,-1);

    int count = 0;
    for(int i = 1;i < 5;i ++){
        finger = fingers[i];
        QVector2D point = points2D[1 + i * 4];
        if(point == fakepoint){
            continue;
        }
        count++;
    }

    qDebug()<<"plan:: valid finger root = "<<count;

    //palm plane
    CvMat *points_mat = cvCreateMat(count + 1, 3, CV_32FC1);//定义用来存储需要拟合点的矩阵

    points_mat->data.fl[0] = joints[0][0].x();
    points_mat->data.fl[1] = joints[0][0].y();
    points_mat->data.fl[2] = joints[0][0].z();


    QVector3D validRoot3D[count];

    int counti = 1;
    for(int i = 1;i < 5;i ++){
        finger = fingers[i];
        QVector2D point = points2D[1 + i * 4];
        if(point == fakepoint){
            continue;
        }
        vec2 = point - points2D[0];
        vec2 *= scale;
        float temp = pow(finger->length[0],2) - pow(vec2.length(),2);
        float z = temp > 0 ? sqrtf(temp):0;
        QVector3D vec3 = joints[i][0] + QVector3D(vec2.x(),vec2.y(),z);
        points_mat->data.fl[counti*3+0] = vec3.x();//矩阵的值进行初始化   X的坐标值
        points_mat->data.fl[counti * 3 + 1] = vec3.y();//  Y的坐标值
        points_mat->data.fl[counti * 3 + 2] = vec3.z();
        counti ++;
    }

    float plane12[4] = { 0 };//定义用来储存平面参数的数组
    cvFitPlane(points_mat, plane12);//调用方程

    qDebug() << " plane =" << plane12[0] <<  plane12[1]<< plane12[2]<< plane12[3];
//    return;

    //palm normal vector
    QVector3D n_vec(plane12[0],plane12[1],plane12[2]);

    //four finger
    counti = 0;
    for(int n = 0;n < 5;n ++){
//        if(n == thumbIdx)
//            continue;
        int i = counti ++;
        finger = fingers[i];
        QVector2D point = points2D[1 + n * 4];
        float plane[4] = { 0 };

        if(point != fakepoint){
            QVector2D vec2 = point - points2D[0];
            vec2 *= scale;
            float z = plane12[3] - plane12[0] * vec2.x() - plane12[1] * vec2.y();
            z /= plane12[2];
            QVector3D vec3 = QVector3D(vec2.x(),vec2.y(),z);
            joints[i][1] = joints[i][0] + vec3;
//            qDebug()<<"hahhahah"<<joints[i][0] .x()<<joints[i][0] .y()<<vec3.x()<<vec3.y();
            calPlane(joints[i][1],n_vec,vec3,plane);

//            vec3 = joints[i][1] - joints[i][0];
//            vec2 = QVector2D(vec3.x(),vec3.y());
//            vec2 /= scale;
//            QVector2D rePoint = vec2 + points2D[0];
//            qDebug()<<"repoint = "<<rePoint.x()<<rePoint.y()<<"init point ="<<point.x()<<point.y();

        }else{
//            continue;
            qDebug()<<"finger "<<i<<"= fakeend";
            QVector3D root_dir;
            int found = 0;
            for(int j = 1;j <= 3; j ++){
                QVector2D pointlast = points2D[n * 4 + j];
                QVector2D pointnext = points2D[n * 4 + j + 1];
                if(pointlast == fakepoint || pointnext == fakepoint){
                    continue;
                }else{
                    found = 1;
                    QVector2D vec2 = pointnext - pointlast;
                    vec2 *= scale;
                    float temp = pow(finger->length[j],2) - pow(vec2.length(),2);
                    float z = temp > 0 ? sqrtf(temp):0;
                    QVector3D vec3 = QVector3D(vec2.x(),vec2.y(),z);
                    root_dir = joints[i][0] + vec3;
                    calPlane(root_dir,n_vec,joints[i][0],plane);
                    qDebug()<<"cal plan here";
//                    break;
                }
            }

            if(! found)
                continue;

            QVector3D palm_n = n_vec;
            QVector3D finger_n(plane[0],plane[1],plane[2]);
            QVector3D root_vec;
            calInterLine(palm_n,finger_n,root_vec);
            if(QVector3D::dotProduct(root_vec,root_dir) < 0)
                root_vec *= -1;

            root_vec.normalize();
            root_vec *= finger->length[0];
            joints[i][1] = root_vec;



            QVector3D vec3 = joints[i][1] - joints[i][0];
            QVector2D vec2 = QVector2D(vec3.x(),vec3.y());
            vec2 /= scale;
            points2D[1 + n * 4] = vec2 + points2D[0];
        }


        for(int j = 1;j < 4;j ++){
            QVector2D vec2 = points2D[1 + i * 4 + j] - points2D[i * 4 + j];
            vec2 *= scale;
            QVector3D vec3 = joints[i][j] + QVector3D(vec2.x(),vec2.y(),0);
            float z = plane[3] - plane[0] * vec3.x() - plane[1] * vec3.y();
            z /= plane[2];
            joints[i][j + 1] = QVector3D(vec3.x(),vec3.y(),z);
        }

    }

    qDebug()<<"finger ends 4 finished";

//    //thumb
//    CvMat *thumb_mat = cvCreateMat(3, 3, CV_32FC1);//定义用来存储需要拟合点的矩阵

//    finger = fingers[0];
//    for(int j = 0; j < 4;j ++){

//        if(points2D[j] == fakepoint)
//            continue;

//        vec2 = points2D[1 + j] - points2D[j];
//        vec2 *= scale;
//        float temp = pow(finger->length[j],2) - pow(vec2.length(),2);
//        float z = temp > 0 ? sqrtf(temp):0;
//        QVector3D vec3 = joints[0][j] + QVector3D(vec2.x(),vec2.y(),z);
//        joints[0][j + 1] = vec3;
//        thumb_mat->data.fl[j*3+0] = vec3.x();//矩阵的值进行初始化   X的坐标值
//        thumb_mat->data.fl[j * 3 + 1] = vec3.y();//  Y的坐标值
//        thumb_mat->data.fl[j * 3 + 2] = vec3.z();
//    }
//    float planethumb[4] = { 0 };//定义用来储存平面参数的数组
//    cvFitPlane(thumb_mat, planethumb);//调用方程
//    for(int j = 1; j < 4;j ++){
//        QVector3D vec3 = joints[0][j + 1];
//        float z = planethumb[3] - planethumb[0] * vec3.x() - planethumb[1] * vec3.y();
//        z /= planethumb[2];
//        joints[0][j + 1].setZ(z);
//    }

    // normalize
    for(int i = 0;i < 5;i ++){
        finger = fingers[i];
        for(int j = 0;j < 4;j ++){
            QVector3D vec3 = joints[i][j + 1] -joints[i][j];
//            vec3.setZ(-vec3.z());

            vec3.normalize();
            vec3 = vec3 * finger->length[j];
            joints[i][j + 1] = joints[i][j] + vec3;
        }
    }

    // mirror
    for(int i = 0;i < 5;i ++){
        for(int j = 0;j < 4;j ++){
            joints[i][j + 1].setY(-joints[i][j + 1].y());
        }
    }

//    for(int i = 0;i < 5;i ++){
//        finger = fingers[i];
//        for(int j = 0;j < 4;j ++){
//            if(j == 0){
//                vec2 = points2D[1 + i * 4] - points2D[0];
//                vec2 *= scale;
//                float z = plane12[3] - plane12[0] * vec2.x() - plane12[1] * vec2.y();
//                z /= plane12[2];
//                joints[i][1] = joints[i][0] + QVector3D(vec2.x(),vec2.y(),z);

//            }
//            else{
//                vec2 = points2D[1 + i * 4 + j] - points2D[i * 4 + j];
//                vec2 *= scale;
//                float temp = pow(finger->length[j],2) - pow(vec2.length(),2);
//                float z = temp > 0 ? -sqrtf(temp):0;
//                joints[i][j + 1] = joints[i][j] + QVector3D(vec2.x(),vec2.y(),z);
//            }
//        }
//    }


//    temp = pow(finger->length[0],2) - pow(vec2.length(),2);
//    z = temp > 0 ? sqrtf(temp):0;
//    qDebug()<<" z = "<<pow(finger->length[0],2)<<"-"<<pow(vec2.length(),2)<<"="<<z;
//    joints[0][1] = QVector3D(vec2.x(),vec2.y(),z);

//    vec2 = points2D[2] - points2D[1];
//    vec2 *= scale;
//    temp = pow(finger->length[1],2) - pow(vec2.length(),2);
//    z = temp > 0 ? sqrtf(temp):0;
////    z = sqrtf(pow(finger->length[1],2) - pow(vec2.length(),2));
//    joints[0][2] = joints[0][1] + QVector3D(vec2.x(),vec2.y(),z);

//    vec2 = points2D[3] - points2D[2];
//    vec2 *= scale;
//    temp = pow(finger->length[2],2) - pow(vec2.length(),2);
//    z = temp > 0 ? sqrtf(temp):0;
////    z = sqrtf(pow(finger->length[2],2) - pow(vec2.length(),2));
//    joints[0][3] = joints[0][2] + QVector3D(vec2.x(),vec2.y(),z);

//    vec2 = points2D[4] - points2D[3];
//    vec2 *= scale;
//    temp = pow(finger->length[3],2) - pow(vec2.length(),2);
//    z = temp > 0 ? sqrtf(temp):0;
//    joints[0][4] = joints[0][3] + QVector3D(vec2.x(),vec2.y(),z);
}
