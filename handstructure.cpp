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
    CalGesture();
}

void HandStructure::CalPoints()
{    
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

void HandStructure::CalGesture()
{
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

    points2D[0].setX(x);
    points2D[0].setY(y);

    int i = 0;
    while(!txtInput.atEnd())
    {
        lineStr = txtInput.readLine();
        lst=lineStr.split('\t');
        for(int j = 0;j < 4;j ++){
            int x = lst[j * 2].toInt();
            int y = lst[j * 2 + 1].toInt();
            points2D[1 + i * 4 + j].setX(x);
            points2D[1 + i * 4 + j].setY(y);
        }
        i++;
    }
    f.close();

    Finger *finger = fingers[0];
    QVector2D vec2 = points2D[1] - points2D[0];
    float length3D = finger->length[0];
    float scale = length3D / vec2.length();

//    QVector3D axis_y(0,1,0);
//    QVector3D axis_x(1,0,0);
//    QVector3D axis_z(0,0,1);

//    float z;
//    float temp;// = sqrtf(pow(finger->length[0],2) - pow(vec2.length(),2));

    //palm plane
    CvMat *points_mat = cvCreateMat(5, 3, CV_32FC1);//定义用来存储需要拟合点的矩阵

    points_mat->data.fl[0] = joints[0][0].x();
    points_mat->data.fl[1] = joints[0][0].y();
    points_mat->data.fl[2] = joints[0][0].z();

    for(int i = 1;i < 5;i ++){
        finger = fingers[i];
        vec2 = points2D[1 + i * 4] - points2D[0];
        vec2 *= scale;
        float temp = pow(finger->length[0],2) - pow(vec2.length(),2);
        float z = temp > 0 ? sqrtf(temp):0;
        QVector3D vec3 = joints[i][0] + QVector3D(vec2.x(),vec2.y(),z);
        points_mat->data.fl[i*3+0] = vec3.x();//矩阵的值进行初始化   X的坐标值
        points_mat->data.fl[i * 3 + 1] = vec3.y();//  Y的坐标值
        points_mat->data.fl[i * 3 + 2] = vec3.z();
    }

    float plane12[4] = { 0 };//定义用来储存平面参数的数组
    cvFitPlane(points_mat, plane12);//调用方程

    qDebug() << " plane =" << plane12[0] <<  plane12[1]<< plane12[2]<< plane12[3];
//    return;

    //palm normal vector
    QVector3D n_vec(plane12[0],plane12[1],plane12[2]);

    //four finger
    for(int i = 1;i < 5;i ++){
        finger = fingers[i];
        vec2 = points2D[1 + i * 4] - points2D[0];
        vec2 *= scale;
        float z = plane12[3] - plane12[0] * vec2.x() - plane12[1] * vec2.y();
        z /= plane12[2];
        QVector3D vec3(vec2.x(),vec2.y(),z);
        joints[i][1] = joints[i][0] + vec3;
        float plane[4] = { 0 };
        calPlane(vec3,n_vec,joints[i][1],plane);
        for(int j = 1;j < 4;j ++){
            vec2 = points2D[1 + i * 4 + j] - points2D[i * 4 + j];
            vec2 *= scale;
            QVector3D vec3 = joints[i][j] + QVector3D(vec2.x(),vec2.y(),0);
            z = plane[3] - plane[0] * vec3.x() - plane[1] * vec3.y();
            z /= plane[2];
            joints[i][j + 1] = QVector3D(vec3.x(),vec3.y(),z);
        }
    }

    //thumb
    CvMat *thumb_mat = cvCreateMat(4, 3, CV_32FC1);//定义用来存储需要拟合点的矩阵

    finger = fingers[0];
    for(int j = 0; j < 4;j ++){
        vec2 = points2D[1 + j] - points2D[j];
        vec2 *= scale;
        float temp = pow(finger->length[j],2) - pow(vec2.length(),2);
        float z = temp > 0 ? sqrtf(temp):0;
        QVector3D vec3 = joints[0][j] + QVector3D(vec2.x(),vec2.y(),z);
        joints[0][j + 1] = vec3;
        thumb_mat->data.fl[j*3+0] = vec3.x();//矩阵的值进行初始化   X的坐标值
        thumb_mat->data.fl[j * 3 + 1] = vec3.y();//  Y的坐标值
        thumb_mat->data.fl[j * 3 + 2] = vec3.z();
    }
    float planethumb[4] = { 0 };//定义用来储存平面参数的数组
    cvFitPlane(thumb_mat, planethumb);//调用方程
    for(int j = 0; j < 4;j ++){
        QVector3D vec3 = joints[0][j + 1];
        float z = planethumb[3] - planethumb[0] * vec3.x() - planethumb[1] * vec3.y();
        z /= planethumb[2];
        joints[0][j + 1].setZ(z);
    }

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
