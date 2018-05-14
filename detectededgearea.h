#ifndef DETECTEDEDGEAREA_H
#define DETECTEDEDGEAREA_H

#include <QWidget>
#include "framework.h"

class DetectedEdgeArea : public QWidget
{
    Q_OBJECT
public:
    explicit DetectedEdgeArea(QWidget *parent = 0);

signals:


protected:
    int edgeThresh;
    int lowThreshold;
    int const max_lowThreshold = 100;
    int ratio;
    int kernel_size;

    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

public slots:
    void setCannyPara(int);

};

#endif // DETECTEDEDGEAREA_H
