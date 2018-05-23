#include "handframe.h"
#include "qmath.h"
#include <QMatrix4x4>

HandFrame::HandFrame()
{
    handStructure = new HandStructure;

    m_data.resize(2500 * 6);

//    QVector2
    GLfloat x1 = 0.0;
    GLfloat y1 = 0.0;
    GLfloat z1 = 0.2f;

    GLfloat x2 = -0.15;
    GLfloat y2 = -0.3;
    GLfloat z2 = -0.2f;

//    skeleton(x1,y1,0.0f,x2,y2,0.0f,0.01f);

//    skeleton(x1,y1,z1,x2,y2,z2,0.002f);

//    extrude(x1, y1, x2, y2);
//    extrude(x2, y2, y2, x2);
//    extrude(y2, x2, y1, x1);
//    extrude(y1, x1, x1, y1);
//    extrude(x3, y3, x4, y4);
//    extrude(x4, y4, y4, x4);
//    extrude(y4, x4, y3, x3);

//    const int NumSectors = 100;

//    for (int i = 0; i < NumSectors; ++i) {
//        GLfloat angle = (i * 2 * M_PI) / NumSectors;
//        GLfloat angleSin = qSin(angle);
//        GLfloat angleCos = qCos(angle);
//        const GLfloat x5 = 0.30f * angleSin;
//        const GLfloat y5 = 0.30f * angleCos;
//        const GLfloat x6 = 0.20f * angleSin;
//        const GLfloat y6 = 0.20f * angleCos;

//        angle = ((i + 1) * 2 * M_PI) / NumSectors;
//        angleSin = qSin(angle);
//        angleCos = qCos(angle);
//        const GLfloat x7 = 0.20f * angleSin;
//        const GLfloat y7 = 0.20f * angleCos;
//        const GLfloat x8 = 0.30f * angleSin;
//        const GLfloat y8 = 0.30f * angleCos;

//        quad(x5, y5, x6, y6, x7, y7, x8, y8);

//        extrude(x6, y6, x7, y7);
//        extrude(x8, y8, x5, y5);
//    }
    draw();
}

void HandFrame::clean()
{
    m_count = 0;
}

void HandFrame::draw()
{
    clean();

    for(int i = 0;i < 5; i ++)
        for(int j = 0; j <5; j ++){
            cubic(handStructure->joints[i][j],0.2f);
            if(j < 4)
                skeleton(handStructure->joints[i][j],handStructure->joints[i][j+1],0.02f);
        }
}

void HandFrame::add(const QVector3D &v, const QVector3D &n)
{
    GLfloat *p = m_data.data() + m_count;
    *p++ = v.x();
    *p++ = v.y();
    *p++ = v.z();
    *p++ = n.x();
    *p++ = n.y();
    *p++ = n.z();
    m_count += 6;
}

void HandFrame::triangle(GLfloat x1, GLfloat y1, GLfloat x2)
{

}

void HandFrame::quad(GLfloat points[4][3])
{
    //zheng
    QVector3D n = QVector3D::normal(QVector3D(points[3][0] - points[0][0], points[3][1] - points[0][1], points[3][2] - points[0][2]),
            QVector3D(points[1][0] - points[0][0], points[1][1] - points[0][1],points[1][2] - points[0][2]));

    add(QVector3D(points[0][0],points[0][1],points[0][2]), n);
    add(QVector3D(points[3][0],points[3][1],points[3][2]), n);
    add(QVector3D(points[1][0],points[1][1],points[1][2]), n);

    add(QVector3D(points[2][0],points[2][1],points[2][2]), n);
    add(QVector3D(points[1][0],points[1][1],points[1][2]), n);
    add(QVector3D(points[3][0],points[3][1],points[3][2]), n);
}

void HandFrame::extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2,GLfloat scale)
{
    QVector3D n = QVector3D::normal(QVector3D(0.0f, 0.0f, -0.1f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, +scale), n);
    add(QVector3D(x1, y1, -scale), n);
    add(QVector3D(x2, y2, +scale), n);

    add(QVector3D(x2, y2, -scale), n);
    add(QVector3D(x2, y2, +scale), n);
    add(QVector3D(x1, y1, -scale), n);
}

void HandFrame::joint(GLfloat x, GLfloat y, GLfloat z)
{

}

void HandFrame::cubic(GLfloat x, GLfloat y, GLfloat z,GLfloat size)
{
    size = size/2.0f;

    //qian
    const GLfloat x1 = x + size;
    const GLfloat y1 = y + size;
    const GLfloat z1 = z + size;

    const GLfloat x2 = x - size;
    const GLfloat y2 = y + size;
    const GLfloat z2 = z + size;

    const GLfloat x3 = x - size;
    const GLfloat y3 = y - size;
    const GLfloat z3 = z + size;

    const GLfloat x4 = x + size;
    const GLfloat y4 = y - size;
    const GLfloat z4 = z + size;

    const GLfloat x5 = x + size;
    const GLfloat y5 = y + size;
    const GLfloat z5 = z - size;

    const GLfloat x6 = x - size;
    const GLfloat y6 = y + size;
    const GLfloat z6 = z - size;

    const GLfloat x7 = x - size;
    const GLfloat y7 = y - size;
    const GLfloat z7 = z - size;

    const GLfloat x8 = x + size;
    const GLfloat y8 = y - size;
    const GLfloat z8 = z - size;

    //front
    GLfloat points1[4][3] = {
        x1,y1,z1,
        x2,y2,z2,
        x3,y3,z3,
        x4,y4,z4,
    };

    quad(points1);

    //back
    GLfloat points2[4][3] = {
        x8,y8,z8,
        x7,y7,z7,
        x6,y6,z6,
        x5,y5,z5,
    };

    quad(points2);

    //left
    GLfloat points3[4][3] = {
        x6,y6,z6,
        x7,y7,z7,
        x3,y3,z3,
        x2,y2,z2,
    };

    quad(points3);

    //right
    GLfloat points4[4][3] = {
        x5,y5,z5,
        x1,y1,z1,
        x4,y4,z4,
        x8,y8,z8,
    };
    quad(points4);

    //top
    GLfloat points5[4][3] = {
        x5,y5,z5,
        x6,y6,z6,
        x2,y2,z2,
        x1,y1,z1,
    };
    quad(points5);

    //bottom
    GLfloat points6[4][3] = {
        x7,y7,z7,
        x8,y8,z8,
        x4,y4,z4,
        x3,y3,z3,
    };

    quad(points6);

//    extrude(x1, y1, x2, y2,size);
//    extrude(x2, y2, x3, y3,size);
//    extrude(x3, y3, x4, y4,size);
//    extrude(x4, y4, x1, y1,size);
}

void HandFrame::cubic(QVector3D v, GLfloat size){
    cubic(v.x(),v.y(),v.z(),size);
}

void HandFrame::skeleton(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat r)
{
    const int NumSectors = 10;

    QVector3D dir(x1-x2,y1-y2,z1-z2); //from 2 -> 1

    QVector3D temp(0,1,0);
    if(dir == temp)
        temp = QVector3D(1,0,0);

    QVector3D r_vector = QVector3D::normal(temp, dir);
    r_vector.normalize();
    r_vector *= r;

    QVector3D off_vector = QVector3D::normal(r_vector, dir);
    off_vector.normalize();
    off_vector *= r;

    QVector3D start1 = r_vector + QVector3D(x1,y1,z1) + off_vector;
    QVector3D start2 = - r_vector + QVector3D(x1,y1,z1) + off_vector;
    QVector3D end1 = r_vector + QVector3D(x2,y2,z2) + off_vector;
    QVector3D end2 = - r_vector + QVector3D(x2,y2,z2) + off_vector;

    QMatrix4x4 m_rotate;
    m_rotate.setToIdentity();
    m_rotate.translate(x2,y2,z2);
    m_rotate.rotate(90.0f,dir);
    m_rotate.translate(-x2,-y2,-z2);


    GLfloat points1[4][3] ={
        start1.x(),start1.y(),start1.z(),
        start2.x(),start2.y(),start2.z(),
        end2.x(),end2.y(),end2.z(),
        end1.x(),end1.y(),end1.z(),
    };

    quad(points1);

    QVector3D start11 = m_rotate * start1;
    QVector3D start22 = m_rotate * start2;
    QVector3D end11 = m_rotate * end1;
    QVector3D end22 = m_rotate * end2;

    GLfloat points2[4][3] ={
        start11.x(),start11.y(),start11.z(),
        start22.x(),start22.y(),start22.z(),
        end22.x(),end22.y(),end22.z(),
        end11.x(),end11.y(),end11.z(),
    };

    quad(points2);

    m_rotate.setToIdentity();
    m_rotate.translate(x2,y2,z2);
    m_rotate.rotate(180.0f,dir);
    m_rotate.translate(-x2,-y2,-z2);

    start11 = m_rotate * start1;
    start22 = m_rotate * start2;
    end11 = m_rotate * end1;
    end22 = m_rotate * end2;

    GLfloat points3[4][3] ={
        start11.x(),start11.y(),start11.z(),
        start22.x(),start22.y(),start22.z(),
        end22.x(),end22.y(),end22.z(),
        end11.x(),end11.y(),end11.z(),
    };

    quad(points3);

    m_rotate.setToIdentity();
    m_rotate.translate(x2,y2,z2);
    m_rotate.rotate(270.0f,dir);
    m_rotate.translate(-x2,-y2,-z2);

    start11 = m_rotate * start1;
    start22 = m_rotate * start2;
    end11 = m_rotate * end1;
    end22 = m_rotate * end2;

    GLfloat points4[4][3] ={
        start11.x(),start11.y(),start11.z(),
        start22.x(),start22.y(),start22.z(),
        end22.x(),end22.y(),end22.z(),
        end11.x(),end11.y(),end11.z(),
    };

    quad(points4);
}

void HandFrame::skeleton(QVector3D v1, QVector3D v2, GLfloat r)
{
    skeleton(v1.x(),v1.y(),v1.z(),v2.x(),v2.y(),v2.z(),r);
}

void HandFrame::sylinder(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat r)
{
    const int NumSectors = 10;

    QVector3D dir(x1-x2,y1-y2,z1-z2); //from 2 -> 1

    QVector3D temp(0,1,0);
    if(dir == temp)
        temp = QVector3D(1,0,0);

    QVector3D start = QVector3D::normal(temp, dir);
    start.normalize();

    for (int i = 0; i < NumSectors; ++i) {
        GLfloat angle = (i * 2 * M_PI) / NumSectors;
        GLfloat angleSin = qSin(angle);
        GLfloat angleCos = qCos(angle);
        const GLfloat x5 = r * angleSin;
        const GLfloat y5 = r * angleCos;
        const GLfloat x6 = 0;
        const GLfloat y6 = 0;

        angle = ((i + 1) * 2 * M_PI) / NumSectors;
        angleSin = qSin(angle);
        angleCos = qCos(angle);
        const GLfloat x7 = 0;
        const GLfloat y7 = 0;
        const GLfloat x8 = r * angleSin;
        const GLfloat y8 = r * angleCos;

//        quad(x5, y5, x6, y6, x7, y7, x8, y8,r);

        extrude(x6, y6, x7, y7,r);
        extrude(x8, y8, x5, y5,r);
    }
}

void HandFrame::generatePoints()
{
    qDebug()<<"generate points";
//    handStructure->CalGesture();
    draw();
}

void pathDetector(QVector3D p0, Mat mat){

}
