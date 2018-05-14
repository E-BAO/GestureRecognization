#ifndef HANDFRAME_H
#define HANDFRAME_H

#include "handstructure.h"
#include <qopengl.h>
#include <QVector>
#include <QVector3D>

class HandFrame
{
public:
    HandFrame();
    const GLfloat *constData() const { return m_data.constData(); }
    int count() const { return m_count; }
    int vertexCount() const { return m_count / 6; }

private:
    HandStructure* handStructure;
    void triangle(GLfloat x1, GLfloat y1, GLfloat x2);
    void quad(GLfloat points[4][3]);
    void extrude(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat scale);
    void add(const QVector3D &v, const QVector3D &n);
    void cubic(GLfloat x, GLfloat y, GLfloat z, GLfloat size);
    void cubic(QVector3D, GLfloat size);
    void joint(GLfloat x, GLfloat y,GLfloat z);
    void skeleton(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat r);
    void skeleton(QVector3D, QVector3D, GLfloat r);
    void sylinder(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat r);

    void clean();
    void draw();

    QVector<GLfloat> m_data;
    int m_count;

public slots:
    void generatePoints();

};

#endif // HANDFRAME_H
