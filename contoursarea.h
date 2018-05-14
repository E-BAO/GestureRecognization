#ifndef CONTOURSAREA_H
#define CONTOURSAREA_H
#include <QWidget>
#include "framework.h"

class ContoursArea: public QWidget
{
    Q_OBJECT

public:
    ContoursArea();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};

#endif // CONTOURSAREA_H
