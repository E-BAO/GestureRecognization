#ifndef FLOODFILLAREA_H
#define FLOODFILLAREA_H

#include <QWidget>
#include "framework.h"

class FloodFillArea : public QWidget
{
    Q_OBJECT
public:
    explicit FloodFillArea(QWidget *parent = 0);

signals:

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

public slots:
};

#endif // FLOODFILLAREA_H
