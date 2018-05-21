#include "renderwidget.h"
#include <QGridLayout>
#include <QPainter>
#include <QFile>
#include <QMessageBox>
#include <QString>
#include <QList>
#include <QDebug>
#include <QApplication>

using namespace std;

typedef struct color Color;

struct color{
    float b,g,r;
    color(float blue,float green, float red):b(blue),g(green),r(red){}
};

//BGR
Color c_black(0, 0, 0);
Color c_red(0,0,1.0);
Color c_yellow(0,1.0,1.0);
Color c_green(0,1.0,0);
Color c_cyan(1.0,1.0,0);
Color c_blue(1,0,0);

Color paintColor[6] = {
    c_black,c_red,c_yellow,c_green,c_cyan,c_blue
};

/***************/


RenderWidget::RenderWidget(QWidget *parent) : QWidget(parent)
{
    edgeThresh = 1;
    lowThreshold = 0;
    ratio = 3;
    kernel_size = 3;

    step = 0;
    for(int i = 0;i < JOINTS_NUMBER;i++){
        pickPointArray[i] = new PickPoint(this);
//        connect(pickPointArray[i],SIGNAL(moved()) ,
//                this, SLOT(update()));
        pickPointArray[i]->setVisible(false);
    }
    handCtPoint = new PickPoint(this);
    handCenter = new Point(174,238);
    handCtPoint->setGeometry(handCenter->x - handCtPoint->width()/2,handCenter->y-handCtPoint->height()/2,handCtPoint->width(),handCtPoint->height());
}

QSize RenderWidget::minimumSizeHint() const
{
    return QSize(400, 400);
}

QSize RenderWidget::sizeHint() const
{
    return QSize(400, 400);
}

void RenderWidget::mousePressEvent(QMouseEvent *e){
    if(step >= JOINTS_NUMBER)return;
    PickPoint *point = handCtPoint;//pickPointArray[step];
    point->setVisible(true);
    point->setGeometry(e->x() - point->width()/2,e->y()-point->height()/2,point->width(),point->height());
    step++;
}

void RenderWidget::mouseMoveEvent(QMouseEvent *ev) {
    qDebug()<<"mouse move at"<<ev->x()<<ev->y();
}

void RenderWidget::keyPressEvent(QKeyEvent *e){
    qDebug()<<"key = ";
    QWidget::keyPressEvent(e);
}



//QStringList arguments = QCoreApplication::arguments();
////QString str = arguments.at(0);

// + to_string(indexofImg) + "/";

void RenderWidget::paintEvent(QPaintEvent * /* event */)
{
    int indexofImg = 1;

    string folder_path = "/Users/ebao/study/lab/Gesture/images/gesture" + to_string(indexofImg) + "/";

//    qDebug()<<"image gesture"<<indexofImg;
//    vector<Point> out_para;
//    Mat outimg;
//    training(folder_path, out_para,outimg);

//    imwrite("/Users/ebao/study/lab/Gesture/images/gestures_pre/finger_img" + to_string(indexofImg) + ".png",outimg);


    Mat src = imread(folder_path+"gesture.jpg");
    Mat src_gray;
    cvtColor(src, src_gray,CV_RGB2GRAY);

    QImage img = cvMat2QImage(src_gray);

    QPainter painter(this);
    painter.drawImage(QRect(0, 0, width(), height()), img);
    painter.end();


//    cv::waitKey(0);

//    emit changed();
}

void RenderWidget::setCannyPara(int v){
    lowThreshold = v;
    update();
}


void RenderWidget::readFile()
{
    /********
    QFile f("test.txt");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Open failed.";
        return;
    }

    QTextStream txtInput(&f);
    QString lineStr;
    lineStr = txtInput.readLine();
    QList<QString> lst;
    lst=lineStr.split('\t');
    float x = lst[0].toInt();
    float y = lst[1].toInt();

    PickPoint *p0 = pickPointArray[0];
    p0->setGeometry(x,y,p0->width(),p0->height());
    p0->setVisible(true);

    int i = 0;
    while(!txtInput.atEnd())
    {
        lineStr = txtInput.readLine();
        lst=lineStr.split('\t');
        for(int j = 0;j < 4;j ++){
            PickPoint *p = pickPointArray[1+i * 4 + j];
            int x = lst[j * 2].toInt();
            int y = lst[j * 2 + 1].toInt();
            p->setGeometry(x,y,p->width(),p->height());
            p->setVisible(true);
        }
        i++;
    }
    step = JOINTS_NUMBER;

    f.close();
    *********/
}

void RenderWidget::writeFile()
{
    if(step >= JOINTS_NUMBER){
        QFile f("test.txt");
        if(!f.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Open failed.";
            return;
        }
        QTextStream txtOutput(&f);

        PickPoint *p0 = pickPointArray[0];
        float x = p0->x();
        float y = p0->y();
        txtOutput << x << "\t" << y << "\t\n";

        for(int i = 0;i < 5;i ++){
            for(int j = 0;j < 4;j ++){
                PickPoint *p = pickPointArray[1+i * 4 + j];
                int x = p->x();
                int y = p->y();
                txtOutput << x << "\t" << y << "\t";
            }
            txtOutput <<"\n";
        }
        f.close();
    }else{
        QString string = "Joints not enough:" + QString::number(step);
        QMessageBox::about(NULL, "Write Error", string);
    }
}



/*********************/

/*********** line thinning  ************/

static int Get_gray(Mat* im, int x, int y) {
  return static_cast<int>(im->at<uchar>(Point(x, y)));

//  int c = cv gdImageGetPixel(ptr, x, y);
//  int r = gdImageRed(ptr, c);
//  int g = gdImageGreen(ptr, c);
//  int b = gdImageBlue(ptr, c);
//  return GRAY(r, g, b);
}

////四周细化算法[/backcolor]
//void Refine(Mat& image)
//{
//        int p[8];
//        int top=1, down=1, right=1, left=1;
//        vector<Point> del;
//        int grayvalue = 0;
//        int height = image.rows;   //获取图像高度
//        int width = image.cols;           //获取图像宽度
//        Mat *im = reinterpret_cast<Mat*>((void*)&image);    //获取像素点信息
//        //上下收缩
//        for (int i = 1; i < height-1; i++)
//        {
//                for (int j = 1; j < width-1; j++)
//                {
//                        grayvalue = Get_gray(im, j, i);  //获取指定点灰度值
//                        if (grayvalue != 0)   //判断中心点是否为前景
//                        {
//                                p[0] = (Get_gray(im, j + 1, i) == 0) ? 0 : 1;
//                                p[1] = (Get_gray(im, j + 1, i - 1) == 0) ? 0 : 1;
//                                p[2] = (Get_gray(im, j, i - 1) == 0) ? 0 : 1;
//                                p[3] = (Get_gray(im, j - 1, i - 1) == 0) ? 0 : 1;
//                                p[4] = (Get_gray(im, j - 1, i) == 0) ? 0 : 1;
//                                p[5] = (Get_gray(im, j - 1, i + 1) == 0) ? 0 : 1;
//                                p[6] = (Get_gray(im, j, i + 1) == 0) ? 0 : 1;
//                                p[7] = (Get_gray(im, j + 1, i + 1) == 0) ? 0 : 1;
//                                if (i < height - 2)
//                                        down = (Get_gray(im, j, i + 2) == 0) ? 0 : 1;
//                                else
//                                        down = 1;
//                                //  横向直线
//                                if (p[6] && (p[5] || p[7] || p[0] || p[4]) && !(p[1] || p[3]) && p[2] == 0 && down)
//                                {
//                                        del.push_back(Point(j, i));
//                                }
//                                if (p[2] && (p[1] || p[3] || p[0] || p[4]) && !( p[5] || p[7]) && p[6] == 0)
//                                {
//                                        del.push_back(Point(j, i));
//                                }
//                        }
//                }
//        }

//        for (int i = 1; i < height - 2; i++)
//        {
//                grayvalue = Get_gray(im, 0, i);
//                if (grayvalue != 0)
//                {
//                        if ( Get_gray(im, 0, i - 1) && Get_gray(im, 1, i - 1) && Get_gray(im, 0, i + 1)==0 && Get_gray(im, 1, i)==0) //上2，上1，右上1，下1=0，右1=0
//                        {
//                                del.push_back(Point(0, i));
//                        }
//                        if (Get_gray(im, 0, i - 1) == 0 && Get_gray(im, 1, i + 1) && Get_gray(im, 1, i) == 0 && Get_gray(im, 0, i+2))//上1=0，下1，右下1，右1=0，下2
//                        {
//                                del.push_back(Point(0, i));
//                        }
//                }
//                if (grayvalue != 0)
//                {
//                        if (Get_gray(im, width - 1, i - 1) && Get_gray(im, width - 2, i - 1) && Get_gray(im, width - 1, i + 1) == 0 && Get_gray(im, width - 2, i) == 0) //上2，上1，左上1，下1=0，左1=0
//                        {
//                                del.push_back(Point(width - 1, i));
//                        }
//                        if (Get_gray(im, width - 1, i - 1) == 0 && Get_gray(im, width - 2, i + 1) && Get_gray(im, width - 2, i) == 0 && Get_gray(im, width - 1, i + 2))//上1=0，下1，左下1，左1=0，下2
//                        {
//                                del.push_back(Point(width - 1, i));
//                        }
//                }
//        }
//        for (int i = 0; i < del.size();i++)
//        {
//                uchar* data = image.ptr<uchar>(del[i].y);
//                data[del[i].x]=0;
//        }

//        //左右收缩
//        for (int i = 1; i < height - 1; i++)
//        {
//                for (int j = 1; j < width - 1; j++)
//                {
//                        grayvalue = Get_gray(im, j, i);  //获取指定点灰度值
//                        if (grayvalue != 0)   //判断中心点是否为前景
//                        {
//                                p[0] = (Get_gray(im, j + 1, i) == 0) ? 0 : 1;
//                                p[1] = (Get_gray(im, j + 1, i - 1) == 0) ? 0 : 1;
//                                p[2] = (Get_gray(im, j, i - 1) == 0) ? 0 : 1;
//                                p[3] = (Get_gray(im, j - 1, i - 1) == 0) ? 0 : 1;
//                                p[4] = (Get_gray(im, j - 1, i) == 0) ? 0 : 1;
//                                p[5] = (Get_gray(im, j - 1, i + 1) == 0) ? 0 : 1;
//                                p[6] = (Get_gray(im, j, i + 1) == 0) ? 0 : 1;
//                                p[7] = (Get_gray(im, j + 1, i + 1) == 0) ? 0 : 1;
//                                if (j < width - 2)
//                                        right = (Get_gray(im, j + 2, i) == 0) ? 0 : 1;
//                                else
//                                        right = 1;


//                                //竖直线
//                                if (p[0] && (p[1] || p[7] || p[2] || p[6]) && !(p[3] || p[5]) && p[4] == 0 && right)
//                                {
//                                        del.push_back(Point(j, i));
//                                }
//                                if (p[4] && (p[3] || p[5] || p[2] || p[6]) && !(p[1] || p[7]) && p[0] == 0)
//                                {
//                                        del.push_back(Point(j, i));
//                                }

//                        }
//                }
//        }

//        for (int j = 1; j < width - 2; j++)
//        {
//                grayvalue = Get_gray(im, j, 0);
//                if (grayvalue != 0)
//                {
//                        if (Get_gray(im, j - 1, 0) == 0 && Get_gray(im, j + 1, 0) && Get_gray(im, j + 2, 0) && Get_gray(im, j, 1) == 0 && Get_gray(im, j+1, 1)) //左1=0，右1，右2，下1=0，右下1
//                        {
//                                del.push_back(Point(j, 0));
//                        }
//                        if (Get_gray(im, j - 1, 0) && Get_gray(im, j+1, 0)==0 && Get_gray(im, j, 1) == 0 && Get_gray(im, j-1, 1))//左1，右1=0，下1=0，左下1
//                        {
//                                del.push_back(Point(j, 0));
//                        }
//                }
//        }
//        for (int j = 1; j < width - 2; j++)
//        {
//                grayvalue = Get_gray(im, j, height-1);
//                if (grayvalue != 0)
//                {
//                        if (Get_gray(im, j - 1, height - 1) == 0 && Get_gray(im, j + 1, height - 1) && Get_gray(im, j + 2, height - 1) && Get_gray(im, j, height - 2) == 0 && Get_gray(im, j + 1, height - 2)) //左1=0，右1，右2，下1=0，右下1
//                        {
//                                del.push_back(Point(j, height - 1));
//                        }
//                        if (Get_gray(im, j - 1, height - 1) && Get_gray(im, j + 1, height - 1) == 0 && Get_gray(im, j, height - 2) == 0 && Get_gray(im, j - 1, height - 2))//左1，右1=0，下1=0，左下1
//                        {
//                                del.push_back(Point(j, height - 1));
//                        }
//                }
//        }

//        for (int i = 0; i < del.size(); i++)
//        {
//                uchar* data = image.ptr<uchar>(del[i].y);
//                data[del[i].x] = 0;
//        }
//}
///**********************************/


/************** T junction detection ********************/



//void splitFingerLine(vector<Point> &fingerLine,  vector<vector<Point>> &output_fingerLines, vector<Point> &jointPoints,\
//                     int size){

//}
