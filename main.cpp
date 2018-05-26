#include "mainwindow.h"
#include <QApplication>
#include "training.h"
#include <QString>

int main(int argc, char *argv[])
{

    for(int i = 0;i <= 13; i ++){
        string folder_path = "/Users/ebao/study/lab/Gesture/images/gesture" + to_string(i) + "/";

        qDebug()<<"image gesture ============== "<<i;
        vector<Point> out_para;
        Mat outimg;
        training(folder_path, out_para,outimg);

        imwrite("/Users/ebao/study/lab/Gesture/images/gestures_pre_clr/finger_img" + to_string(i) + ".png",outimg);
    }
    waitKey(0);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();



    return a.exec();
}
