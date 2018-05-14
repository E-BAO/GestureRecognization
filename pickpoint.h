#ifndef PICKPOINT_H
#define PICKPOINT_H

#include <QWidget>
#include <QLabel>
#include <QColor>
#include <QMouseEvent>

class PickPoint : public QLabel
{
    Q_OBJECT
public:
    explicit PickPoint(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
//    void mouseReleaseEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
//    QSize maximumSizeHint() const Q_DECL_OVERRIDE;

signals:
    void moved();
protected:
    void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
public slots:
private:
    QColor *brushColor;
    int size;
    int lX, lY;
    int cX, cY;
};

#endif // PICKPOINT_H
