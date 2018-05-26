#include "imgprocessing.h"


void InitMat(Mat& m,float* num)
{
    for(int i=0;i<m.rows;i++)
        for(int j=0;j<m.cols;j++)
            m.at<float>(i,j)=*(num+i*m.rows+j);
}

/********* find far point ***************/

int findFarPoint(vector<Point> &_neighbor_points, Mat &_image, Point _inpoint, Point& _outpoint, int &count){

    count = 0;
    int maxdist = 0;
    _outpoint = _inpoint;

    _image.at<uchar>(_inpoint) = 0;


    for(int i = 0; i < _neighbor_points.size();i ++){
        Point tmppoint  = _inpoint + _neighbor_points[i];
        if (tmppoint.x > 0 && tmppoint.y > 0 && tmppoint.x < _image.cols&&tmppoint.y < _image.rows){
            int tmpdist = 0;
            Point tmpoutPoint;
            if(_image.at<uchar>(tmppoint) != 0){
                _image.at<uchar>(tmppoint) = 0;
                int _count;
                tmpdist = 1 + findFarPoint(_neighbor_points,_image,tmppoint,tmpoutPoint,_count);
//                count += _count;
                if(tmpdist > maxdist){
                    maxdist = tmpdist;
                    _outpoint = tmpoutPoint;
                    count ++;
                }
            }
        }
        if(maxdist > 200)
            break;
    }

    return maxdist;
}


/********* find curves ***************/
bool findNextPoint(vector<Point> &_neighbor_points, Mat &_image, Point _inpoint, int flag, Point& _outpoint, int &_outflag)
{
    int i = flag;
    int count = 1;
    bool success = false;

    while (count <= 8)  //////////////// 7
    {
        Point tmppoint = _inpoint + _neighbor_points[i];
        if (tmppoint.x > 0 && tmppoint.y > 0 && tmppoint.x < _image.cols&&tmppoint.y < _image.rows)
        {
            if (_image.at<uchar>(tmppoint) == 255)
            {
                _outpoint = tmppoint;
                _outflag = i;
                success = true;
                _image.at<uchar>(tmppoint) = 0;  ///////////  0
                break;
            }
        }
        if (count % 2)
        {
            i += count;
            if (i > 7)
            {
                i -= 8;
            }
        }
        else
        {
            i += -count;
            if (i < 0)
            {
                i += 8;
            }
        }
        count++;
    }
    return success;
}
//寻找图像上的第一个点
bool findFirstPoint(Mat &_inputimg, Point &_outputpoint)
{
    bool success = false;
    for (int i = 0; i < _inputimg.rows; i++)
    {
        uchar* data = _inputimg.ptr<uchar>(i);
        for (int j = 0; j < _inputimg.cols; j++)
        {
            if (data[j] == 255)
            {
                success = true;
                _outputpoint.x = j;
                _outputpoint.y = i;
                data[j] = 0;
                break;
            }
        }
        if (success)
            break;
    }
    return success;
}

bool isLine(Point p, double k, Point P0,double eps){
    double y = k * (p.x - P0.x) + P0.y;
    return abs(y - p.y) < eps;
}

//寻找曲线
void findLines(Mat &inputimg, vector<deque<Point>> &_outputlines)
{
    Mat _inputimg = inputimg.clone();
    vector<Point> neighbor_points = { Point(-1,-1),Point(0,-1),Point(1,-1),Point(1,0),Point(1,1),Point(0,1),Point(-1,1),Point(-1,0) };
    Point first_point;
    while (findFirstPoint(_inputimg, first_point))
    {
        vector<Point> vecLine;
        deque<Point> line;
        line.push_back(first_point);
        vecLine.push_back(first_point);
        //由于第一个点不一定是线段的起始位置，双向找
        Point this_point = first_point;
        int this_flag = 0;
        Point next_point;
        int next_flag;
        while (findNextPoint(neighbor_points, _inputimg, this_point, this_flag, next_point, next_flag))
        {
            vecLine.push_back(next_point);
            line.push_back(next_point);
            this_point = next_point;
            this_flag = next_flag;
        }
        //找另一边
        this_point = first_point;
        this_flag = 0;
        //cout << "flag:" << this_flag << endl;
        while (findNextPoint(neighbor_points, _inputimg, this_point, this_flag, next_point, next_flag))
        {
            line.push_front(next_point);
//            vecLine.push_front(next_point);
            vecLine.insert(vecLine.begin(),next_point);

            this_point = next_point;
            this_flag = next_flag;
        }
        cv::Vec4f line_para;
        cv::fitLine(vecLine, line_para, cv::DIST_L2, 0, 1e-2, 1e-2);
        Point point0;
        point0.x = line_para[2];
        point0.y = line_para[3];
        double k = line_para[1] / line_para[0];

        float eps = 1e-2;
        if (line.size() > 10)  //adjusted
        {
            Point p_end = line.at(line.size() - 1);
            Point p_start = line.at(0);
            int range = 3;
            if(p_start.x < range||p_start.y < range||p_end.x < range|| p_end.y < range){
                continue;
            }
            if(isLine(line.at(0),k,point0,eps) \
                    && isLine(line.at(line.size() - 1),k,point0,eps)\
                    && isLine(line.at(line.size()/3),k,point0,eps) \
                    && isLine(line.at(int(line.size() * 0.6)),k,point0,eps))
                continue;
            else
                _outputlines.push_back(line);
        }
    }
}


void findEnd(Mat &img, Point centerPoint, Point2f &vecDir){

    int maxPixelLen = 25; ///////adjusted here
    int igPixelLen = 5;

    int r = 10; // range larger than 10
    vector<Point> neighbor_points = { Point(-1,-1),Point(0,-1),Point(1,-1),Point(1,0),Point(1,1),Point(0,1),Point(-1,1),Point(-1,0) };
    int divide = 2;
    float tmpAngle = 0;
    int minPixelLen = img.cols + img.rows;

    Point2f minVecDir = vecDir;

//    int compareArray[3] = {0,0,0};

    float dAngle = CV_PI/4.0/divide;
    for(int i = -divide/2; i < divide/2; i ++){
        tmpAngle = dAngle * i;
        float m0[]={ cos(tmpAngle),sin(tmpAngle),-sin(tmpAngle),cos(tmpAngle)};
//        Mat M0(2,2,CV_32F);
//        InitMat(M0,m0);
        Point2f tmpVecDir(m0[0] * vecDir.x+ m0[1] * vecDir.y, m0[2] * vecDir.x + m0[3] * vecDir.y);

        if(fabs(tmpVecDir.ddot(tmpVecDir) - 1) > 1e-2)
            qDebug("error: the vector is not normalized");

        float absX = fabs(tmpVecDir.x);
        float absY = fabs(tmpVecDir.y);

        int startNbIdx;

        if(absX > absY){
            tmpVecDir /= absX;
            if(tmpVecDir.x > 0){
                startNbIdx = 1;
            }else{
                startNbIdx = 5;
            }
        }
        else{
            tmpVecDir /= absY;
            if(tmpVecDir.y > 0)
                startNbIdx = 3;
            else
                startNbIdx = 7;
        }

        bool foundEnd = false;
        int tmpPixelLen = maxPixelLen;
        for(int j = igPixelLen; j < maxPixelLen; j++){
            Point tmpPoint = centerPoint + (Point)(tmpVecDir * j);
            for(int k = 0; k < 5;k ++){
                int idx = (startNbIdx + k)%8;
                Point tmpNbPoint = tmpPoint + neighbor_points[idx];
                if(tmpNbPoint.x > img.cols - 2 || tmpNbPoint.y > img.rows - 2)
                    continue;
//                qDebug("coor = %d,%d",tmpNbPoint.x,tmpNbPoint.y);
                if (img.at<uchar>(tmpNbPoint) != 0){
                    foundEnd = true;
                    break;
                }
            }
            if(foundEnd){
                tmpPixelLen = j;
                break;
            }
        }
        if(minPixelLen > tmpPixelLen){
            minPixelLen = tmpPixelLen;
            minVecDir = tmpVecDir;
        }
    }
    vecDir = minVecDir/sqrt(minVecDir.ddot(minVecDir));
}



//CheckMode: 0代表去除黑区域，1代表去除白区域; NeihborMode：0代表4邻域，1代表8邻域;
void RemoveSmallRegion(Mat& Src, Mat& Dst, int AreaLimit, int CheckMode, int NeihborMode)
{
   int RemoveCount=0;       //记录除去的个数
   //记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查
   Mat Pointlabel = Mat::zeros( Src.size(), CV_8UC1 );

   if(CheckMode==1)
   {
//       cout<<"Mode: 去除小区域. ";
       for(int i = 0; i < Src.rows; ++i)
       {
           uchar* iData = Src.ptr<uchar>(i);
           uchar* iLabel = Pointlabel.ptr<uchar>(i);
           for(int j = 0; j < Src.cols; ++j)
           {
               if (iData[j] < 10)
               {
                   iLabel[j] = 3;
               }
           }
       }
   }
   else
   {
//       cout<<"Mode: 去除孔洞. ";
       for(int i = 0; i < Src.rows; ++i)
       {
           uchar* iData = Src.ptr<uchar>(i);
           uchar* iLabel = Pointlabel.ptr<uchar>(i);
           for(int j = 0; j < Src.cols; ++j)
           {
               if (iData[j] > 10)
               {
                   iLabel[j] = 3;
               }
           }
       }
   }

   vector<Point2i> NeihborPos;  //记录邻域点位置
   NeihborPos.push_back(Point2i(-1, 0));
   NeihborPos.push_back(Point2i(1, 0));
   NeihborPos.push_back(Point2i(0, -1));
   NeihborPos.push_back(Point2i(0, 1));
   if (NeihborMode==1)
   {
//       cout<<"Neighbor mode: 8邻域."<<endl;
       NeihborPos.push_back(Point2i(-1, -1));
       NeihborPos.push_back(Point2i(-1, 1));
       NeihborPos.push_back(Point2i(1, -1));
       NeihborPos.push_back(Point2i(1, 1));
   }
//   else cout<<"Neighbor mode: 4邻域."<<endl;
   int NeihborCount=4+4*NeihborMode;
   int CurrX=0, CurrY=0;
   //开始检测
   for(int i = 0; i < Src.rows; ++i)
   {
       uchar* iLabel = Pointlabel.ptr<uchar>(i);
       for(int j = 0; j < Src.cols; ++j)
       {
           if (iLabel[j] == 0)
           {
               //********开始该点处的检查**********
               vector<Point2i> GrowBuffer;                                      //堆栈，用于存储生长点
               GrowBuffer.push_back( Point2i(j, i) );
               Pointlabel.at<uchar>(i, j)=1;
               int CheckResult=0;                                               //用于判断结果（是否超出大小），0为未超出，1为超出

               for ( int z=0; z<GrowBuffer.size(); z++ )
               {

                   for (int q=0; q<NeihborCount; q++)                                      //检查四个邻域点
                   {
                       CurrX=GrowBuffer.at(z).x+NeihborPos.at(q).x;
                       CurrY=GrowBuffer.at(z).y+NeihborPos.at(q).y;
                       if (CurrX>=0&&CurrX<Src.cols&&CurrY>=0&&CurrY<Src.rows)  //防止越界
                       {
                           if ( Pointlabel.at<uchar>(CurrY, CurrX)==0 )
                           {
                               GrowBuffer.push_back( Point2i(CurrX, CurrY) );  //邻域点加入buffer
                               Pointlabel.at<uchar>(CurrY, CurrX)=1;           //更新邻域点的检查标签，避免重复检查
                           }
                       }
                   }

               }
               if (GrowBuffer.size()>AreaLimit) CheckResult=2;                 //判断结果（是否超出限定的大小），1为未超出，2为超出
               else {CheckResult=1;   RemoveCount++;}
               for (int z=0; z<GrowBuffer.size(); z++)                         //更新Label记录
               {
                   CurrX=GrowBuffer.at(z).x;
                   CurrY=GrowBuffer.at(z).y;
                   Pointlabel.at<uchar>(CurrY, CurrX) += CheckResult;
               }
               //********结束该点处的检查**********


           }
       }
   }

   CheckMode=255*(1-CheckMode);
   //开始反转面积过小的区域
   for(int i = 0; i < Src.rows; ++i)
   {
       uchar* iData = Src.ptr<uchar>(i);
       uchar* iDstData = Dst.ptr<uchar>(i);
       uchar* iLabel = Pointlabel.ptr<uchar>(i);
       for(int j = 0; j < Src.cols; ++j)
       {
           if (iLabel[j] == 2)
           {
               iDstData[j] = CheckMode;
           }
           else if(iLabel[j] == 3)
           {
               iDstData[j] = iData[j];
           }
       }
   }

//   cout<<RemoveCount<<" objects removed."<<endl;
}
