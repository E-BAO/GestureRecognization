#ifndef RENDERAREA_H
#define RENDERAREA_H

#include "framework.h"
#include "pickpoint.h"

#include <QWidget>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QMouseEvent>


class RenderArea : public QWidget
{
    Q_OBJECT
public:
    explicit RenderArea(QWidget *parent = 0);
    PickPoint *pickPoint;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};



#endif // RENDERAREA_H
