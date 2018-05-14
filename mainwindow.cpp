#include "mainwindow.h"
#include "renderarea.h"
#include <QGridLayout>
#include <QString>
#include <QDebug>
#include <QColor>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

QString hintStringArray[] = {
    "Please Choose Center",
    "Please Choose Thumb",
    "Please Choose Index Finger",
    "Please Choose Middle Finger",
    "Please Choose Ring Finger",
    "Please Choose Little Finger"
};

QColor markColor[] = {
    QColor(Qt::black),
    QColor(Qt::red),
    QColor(Qt::magenta),
    QColor(Qt::yellow),
    QColor(Qt::green),
    QColor(Qt::blue)
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    glWidget = new GLWidget;

    xSlider = createSlider();
    ySlider = createSlider();
    zSlider = createSlider();

    connect(xSlider, &QSlider::valueChanged, glWidget, &GLWidget::setXRotation);
    connect(glWidget, &GLWidget::xRotationChanged, xSlider, &QSlider::setValue);
    connect(ySlider, &QSlider::valueChanged, glWidget, &GLWidget::setYRotation);
    connect(glWidget, &GLWidget::yRotationChanged, ySlider, &QSlider::setValue);
    connect(zSlider, &QSlider::valueChanged, glWidget, &GLWidget::setZRotation);
    connect(glWidget, &GLWidget::zRotationChanged, zSlider, &QSlider::setValue);

//    xSlider->setValue(180 * 16);

    stepN = 0;
//    renderArea = new RenderArea;
    renderWidget = new RenderWidget;
    contoursArea = new ContoursArea;
    floodfillArea = new FloodFillArea;
    detectedEdgeArea = new DetectedEdgeArea;

    generateBtn = new QPushButton("Generate");
    passBtn = new QPushButton("Pass");
    ReadFileBtn = new QPushButton("Read Data");
    WriteFileBtn = new QPushButton("Write Data");
    topHintLabel = new QLabel(hintStringArray[stepN]);

    cannySlider = new QSlider(Qt::Horizontal);
    cannySlider->setRange(0,400);
    cannyLabel = new QLabel("0");

//    pickPoint = new PickPoint();

//    generateBtn->setEnabled(false);
    passBtn->setEnabled(false);

    connect(cannySlider, SIGNAL(valueChanged(int)),
            this, SLOT(cannyChanged(int)));

    cannySlider->setValue(317);

//    connect(renderArea, SLOT(),
//            this, SIGNAL(updateHint()));

    QGridLayout *mainLayout = new QGridLayout;

////    mainLayout->addWidget(topHintLabel, 0, 0, 1, 6);
//    mainLayout->addWidget(renderWidget, 0, 0, 6, 6);
////    mainLayout->addWidget(detectedEdgeArea, 0, 6, 6, 6);
////    mainLayout->addWidget(floodfillArea, 6, 0, 6, 6);
////    mainLayout->addWidget(contoursArea, 6, 6, 6, 6);

//    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *container = new QHBoxLayout;
    container->addWidget(renderWidget);
    container->addWidget(glWidget);
    container->addWidget(xSlider);
    container->addWidget(ySlider);
    container->addWidget(zSlider);

    QWidget *w = new QWidget;
    w->setLayout(container);
    mainLayout->addWidget(w,1,1,1,6);
    mainLayout->addWidget(ReadFileBtn, 2, 1, 1, 1);
    mainLayout->addWidget(WriteFileBtn, 2, 2, 1, 1);
    mainLayout->addWidget(generateBtn, 2, 3, 1, 1);
    mainLayout->addWidget(cannySlider, 2, 4, 1, 1);
    mainLayout->addWidget(cannyLabel, 2, 5, 1, 1);

    QHBoxLayout *container1 = new QHBoxLayout;
//    container1->addWidget(contoursArea);
    container1->addWidget(floodfillArea);
    container1->addWidget(detectedEdgeArea);

    QWidget *w1 = new QWidget;
    w1->setLayout(container1);
    mainLayout->addWidget(w1,3,1,1,10);

//    dockBtn = new QPushButton(tr("Undock"), this);
//    connect(dockBtn, &QPushButton::clicked, this, &Window::dockUndock);
//    mainLayout->addWidget(dockBtn);
    connect(ReadFileBtn, SIGNAL(clicked(bool)),
            renderWidget,SLOT(readFile()));
    connect(WriteFileBtn, SIGNAL(clicked(bool)),
            renderWidget,SLOT(writeFile()));

    connect(generateBtn,SIGNAL(clicked(bool)),
            this, SLOT(generatePoints()));
//    setLayout(mainLayout);

//    xSlider->setValue(15 * 16);
//    ySlider->setValue(345 * 16);
//    zSlider->setValue(0 * 16);

//    connect(renderWidget, SIGNAL(changed()),
//            floodfillArea, SLOT(update()));
//    connect(renderWidget, SIGNAL(changed()),
//            contoursArea, SLOT(update()));

//    mainLayout->addWidget(pickPoint, 2, 1, 1, 1);

//    mainLayout->setSpacing(25);//设置间距
    QWidget* widget = new QWidget(this);
    widget->setLayout(mainLayout);
    this->setCentralWidget(widget);
}

MainWindow::~MainWindow()
{
    delete renderWidget;
    delete generateBtn;
    delete passBtn;
    delete topHintLabel;
    delete cannyLabel;
    delete cannySlider;
}

void MainWindow::updateHint()
{
    topHintLabel->setText(hintStringArray[++stepN]);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    qDebug()<<"mainWindow"<<e->x()<<e->y();
//    qDebug()<<"position"<<renderArea->x()<<renderArea->y();
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    qDebug()<<"key = "<<e->key();
}

void MainWindow::cannyChanged(int v){
    cannyLabel->setText(QString::number(v, 10));
    renderWidget->setCannyPara(v);
    detectedEdgeArea->setCannyPara(v);
    emit renderWidget->changed();
}

QSlider *MainWindow::createSlider()
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(0, 360 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

void MainWindow::generatePoints()
{
//    float *points = renderWidget->calCoordnate();
//    glWidget->updateVBO();
//    glWidget->m_handframe.generatePoints();
}
