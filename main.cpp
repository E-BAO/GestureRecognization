#include "mainwindow.h"
#include <QApplication>
#include "training.h"
#include "handstructure2d.h"

#include <QString>

int main(int argc, char *argv[])
{

    int k = 18;
    for(int i = k;i <= k; i ++){
        string folder_path = "/Users/ebao/study/lab/Gesture/images/gesture" + to_string(i) + "/";

        qDebug()<<"image gesture ============== "<<i;
        vector<Point> out_para;
        Mat outimg;
        training(folder_path, out_para,outimg);

        imwrite("/Users/ebao/study/lab/Gesture/images/gestures_pre_clr/finger_img" + to_string(i) + ".png",outimg);
    }

    for(int i = k;i <= k; i ++){
        HandStructure2D *handStructure2d = new HandStructure2D();
        handStructure2d->indexofImg = i;
        handStructure2d->calGesture();
    }

    waitKey(0);


    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
