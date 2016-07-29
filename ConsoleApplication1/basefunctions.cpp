//
//  basefunctions.cpp
//  opencv_test
//
//  Created by Li Fengqing on 8/18/15.
//  Copyright (c) 2015 Li Fengqing. All rights reserved.
//

#include "basefunctions.h"


Point baseFuncClass::getUpPoint(Point* lines)
{
    return getPoint(lines[0],lines[1], 1);
}

Point baseFuncClass::getDownPoint(Point* lines)
{
    return getPoint(lines[0],lines[1], 0);
}



Point baseFuncClass::getUpPoint(Point* lines,int index)
{
    return getPoint(lines[index*2],lines[index*2+1], 1);
}

Point baseFuncClass::getDownPoint(Point* lines, int index)
{
    return getPoint(lines[index*2],lines[index*2+1], 0);
}

Point baseFuncClass::getPoint(Point pt1, Point pt2, int updown)
{
    if(pt1.y > pt2.y)
    {
        if(updown)
            return pt1;
        else
            return pt2;
    }
    else
    {
        if(updown)
            return pt2;
        else
            return pt1;
    }
    
}


Point baseFuncClass::getUpPoint(Point pt1, Point pt2)
{
    return getPoint(pt1, pt2, 1);
    
}
Point baseFuncClass::getDownPoint(Point pt1, Point pt2)
{
    return getPoint(pt1,pt2,0);
}





float baseFuncClass::Getincline(Point pt1, Point pt2)
{
    float incline =  (float) (pt1.y-pt2.y)/(pt1.x-pt2.x);
    return incline;
}


#if 0
int baseFuncClass::mycvGetValueOnLine(const Mat*img, Point pt1, Point pt2)
{
    Mat srcimage(Size(img->widthStep,img->height), CV_8UC1, img->imageData);
    
    
    int value = 0;
    LineIterator it(srcimage, pt1, pt2, 8);
    //   LineIterator it2 = it;
    vector<uchar> buf(it.count);
    for(int i = 0; i < it.count; i++, ++it)
    {
        buf[i] = *(const uchar*)(* it);
        value += (buf[i]/255);
        
    }
    
    
    return value;
}
#else


int baseFuncClass::mycvGetValueOnLine(const Mat*img, Point pt1, Point pt2)
{
    const Mat &srcimage = *img;
    
    int rectlength = 10;
    int value = 0;
    int width = pt2.x - pt1.x;
    int height = pt2.y - pt1.y;
    if(fabs(width) > fabs(height))
    { //横向均分
        int rectcount = fabs(width)/rectlength; //可能有未整除区域
        float vertical_interval = ((float)height)/rectcount;
        
        for(int i=0;i<rectcount;i++)
        {
            //起始点
            int x = pt1.x+i*((float)width)/rectcount;
            int y = pt1.y+i*vertical_interval;
            for(int j=0;j<rectlength;j++)
            {
                for(int k=0;k<rectlength;k++)
                {
                    if(x+j<img->cols && y+k<img->rows)
                    {
                        
                        uchar ch = srcimage.at<uchar>(Point(x+j,y+k));
                        if(ch )
                            value++;
                        if(value > 2000)
                        {
                            //  printf("what ?\n");
                        }
                        
                    }
                }
            }
        }
        
    }
    else //纵向分
    {
        int rectcount = fabs(height)/rectlength; //可能有未整除区域
        float horizon_interval = (float)width/rectcount;
        
        for(int i=0;i<rectcount;i++)
        {
            //起始点
            
            int x = pt1.x+i*horizon_interval;
            int y = pt1.y+i*(float)height/rectcount;
            for(int j=0;j<rectlength;j++)
            {
                for(int k=0;k<rectlength;k++)
                {
                    if(x+j<img->cols && y+k<img->rows)
                    {
                        uchar ch = srcimage.at<uchar>(Point(x+j,y+k));
                        
                        if(ch )
                            value++;
                        
                        if(value > 2000)
                        {
                            //  printf("what ?\n");
                        }
                    }
                    
                }
            }
        }
    }
    return value;
}
#endif

int baseFuncClass::mycvCheckValidPoint(const Mat*img)
{
    
    
    for(int j=0;j<img->cols;j++)
    {
        for(int k=0;k<img->rows;k++)
        {
            if(img->at<uchar>(k,j))
            {
                
                mValidPoints[validpointscount++%VALIDPOINTMAX]= Point(j,k);
                
                
            }
        }
    }
    return validpointscount;
}

float baseFuncClass::getWidth(Point pt1, Point pt2)
{
    return fabs(pt1.x - pt2.x);

}

float baseFuncClass::getLength(Point pt1, Point pt2)
{
    return sqrt((float)(pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y));
}

float baseFuncClass::getHeight(Point pt1, Point pt2)
{
    return fabs(pt1.y - pt2.y);
}

int  baseFuncClass::threshold( const Mat* srcimage, Mat*  dst, int valid_percentage ) // percentage is valid point from 1 to 1000
{
    int left = 0;
    int right = 255;
    int percentage = 0;
    srcimage->copyTo(*dst);
    percentage = CheckValidPercentage(dst);
    if(percentage < valid_percentage)
    {
        ::threshold(*srcimage, *dst, 0, 255, CV_THRESH_TOZERO); //找出骨节
        printf("no need to crop, original image has less than %d percentage valid points\n", valid_percentage);
        return 0;
    }
    while(right-left > 1)
    {
        ::threshold(*srcimage, *dst, (right+left)/2, 255, CV_THRESH_TOZERO); //找出骨节
        percentage = CheckValidPercentage((const Mat*)dst);
        if(percentage < valid_percentage)
        {
            printf("threshold too large %d %d %d\n", (right+left)/2,percentage ,valid_percentage);
            right = (right +left)/2;
            
            
        }
        else if(percentage  > valid_percentage)
        {
            printf("threshold too small %d %d %d\n", (right+left)/2,percentage ,valid_percentage);
            left = (right +left)/2;
        }
        else //hit
        {
            printf("threshold hit %d %d %d\n", (right+left)/2,percentage ,valid_percentage);
            return (right+left)/2;
        }
        
    }
    ::threshold(*srcimage, *dst, right, 255, CV_THRESH_TOZERO); //取右侧 ，略多crop掉一点点，
    percentage =CheckValidPercentage((const Mat*)dst);
    printf("threshold aproch %d ,%d \n", right,percentage);
    return right;
    
}
int baseFuncClass::get_size(int width, int height)
{
    return width*height;
}
int baseFuncClass::get_validpoint(Mat& image, Rect rect)
{
    int validpoint = 0;
    for(int j=rect.x;j<rect.x+rect.width;j++)
    {
        for(int k=rect.y;k<rect.y+rect.height;k++)
        {
            if(image.at<uchar>(Point(j,k)))
            {
                
                validpoint++;
                
            }
        }
    }
    return validpoint;
}

float baseFuncClass::get_emptyrate(Mat& image, Rect rect)
{
    int validpoint = get_validpoint(image,rect);
    if(validpoint == 0)
        validpoint = 1;
    return (float)(rect.width*rect.height)/validpoint;
        
}


int baseFuncClass::CheckValidPercentage(const Mat*img)
{
    long long validpoint = 0;
    
    CvSize size = img->size();
    
    
    for(int j=0;j<size.width;j++)
    {
        for(int k=0;k<size.height;k++)
        {
            if(img->at<uchar>(Point(j,k)))
            {
                
                validpoint++;
                
            }
        }
    }
    return (validpoint)*1000/(size.width*size.height);
    
}

int baseFuncClass::get_averge_100(vector<int> &buf,int index)
{
    
    int total = 0;
    int count = 0;
    
    int size = buf.size();
    
    for(int i=-50;i<50;i++)
    {
        if(index + i >=0 && index+i<size)
        {
        total += buf[index+i] ;
            count++;
        }
    }
    
    return total/count;
    
}

int baseFuncClass::get_max(vector<int> &buf)
{
    int max = 0;
    for (int i=0; i<buf.size(); i++) {
        if (buf[i] > max) {
            max = buf[i];
        }
    }
    return max;
}

int baseFuncClass::get_averge_inrange(vector<int> &buf,int index,int range)
{
    
    int total = 0;
    int count = 0;
    
    int size = buf.size();
    
    for(int i=-range/2;i<range/2;i++)
    {
        if(index + i >=0 && index+i<size)
        {
            total += buf[index+i] ;
            count++;
        }
    }
    
    return total/count;
}

int baseFuncClass::get_averge(vector<int> &buf)
{
    int total = 0;
    for(int i=0;i<buf.size();i++)
    {
        total += buf[i];
    }
    return total/buf.size();
    
}

int baseFuncClass::averge9point(Mat&src, Mat&dst)
{

     src.copyTo(dst);
    for (int i=1; i<src.cols-1; i++) { //ignore first colum and last column
        for (int j=1; j<src.rows-1; j++) { //ignore first row and last row
            dst.at<uchar>(Point(i,j)) = ((int)src.at<uchar>(Point(i,j))  + src.at<uchar>(Point(i-1,j))  + src.at<uchar>(Point(i+1,j)) +src.at<uchar>(Point(i,j-1))+src.at<uchar>(Point(i,j+1))  + src.at<uchar>(Point(i-1,j-1))  + src.at<uchar>(Point(i+1,j+1)) +src.at<uchar>(Point(i+1,j-1))+src.at<uchar>(Point(i-1,j+1)))/9;
        }
    }
    return 0;
}

int baseFuncClass::averge9point(Mat&src, Mat&dst, int times)
{
    Mat temp ;
    src.copyTo(temp);
    for(int i=0;i<times;i++)
    {
        averge9point(src,dst);
        dst.copyTo(temp);
    }
    return 0;

}

int baseFuncClass::get_average_inrect(Mat& image, Rect rect)
{
    
    long long avg = 0;
    int count = 0;
    for(int j=rect.x;j<rect.x+rect.width;j++)
    {
        for(int k=rect.y;k<rect.y+rect.height;k++)
        {
            int value = image.at<uchar>(Point(j,k));
            if(value)
            {
            avg +=value;
            }
            count++;
            
            
        }
    }
    return avg/count;
    
}
int baseFuncClass::get_average(Mat& image)
{
    return get_average_inrect(image,Rect(0,0,image.cols,image.rows));
}
int baseFuncClass::get_thirdthaverage(Mat& image)
{
    return get_average_inrect(image,Rect(0,0,image.cols,image.rows/3));
}


int baseFuncClass::overlay(Rect a, Rect b, Rect &c)
{
    //计算两个rect的交集
    //
    Rect d;
    d.x = MAX(a.x, b.x);
    d.y = MAX(a.y, b.y);
    d.width = MIN((a.x+a.width-1), (b.x+b.width-1)) - d.x +1;
    d.height = MIN((a.y+a.height-1), (b.y+b.height-1)) - d.y +1;
    if(d.width >0 && d.height>0)
    {
        
        c.x = MIN(a.x, b.x);
        c.y = MIN(a.y, b.y);
        c.width = MAX((a.x+a.width), (b.x+b.width)) - c.x;
        c.height = MAX((a.y+a.height), (b.y+b.height)) - c.y;
        return 1;
        
    }
    return 0;
    
}
int baseFuncClass::getoverlaprate(Rect a, Rect b)
{
    //计算两个rect的交集
    //
    Rect d;
    d.x = MAX(a.x, b.x);
    d.y = MAX(a.y, b.y);
    d.width = MIN((a.x+a.width-1), (b.x+b.width-1)) - d.x +1;
    d.height = MIN((a.y+a.height-1), (b.y+b.height-1)) - d.y +1;
    if(d.width >0 && d.height>0)
    {
        
        int size = MAX(a.width*a.height, b.width*b.height);
        return ( d.width*d.height*100) / size;
        
    }
    return 0;
    
}

int baseFuncClass::drawrect(cv::Mat &image, Rect rect)
{
#define LINEW 2
    int startx = rect.x;
    int starty = rect.y;
    int endx = rect.x + rect.width -1;
    int endy = rect.y + rect.height -1;
    CvScalar color  = CV_RGB(100,100,0);  //yellow
    
    line(image,Point(startx,starty),Point(startx,endy),color , LINEW, 8 );
    line(image,Point(startx,starty),Point(endx,starty),color , LINEW, 8 );
    line(image,Point(endx,endy),Point(startx,endy),color , LINEW, 8 );
    line(image,Point(endx,endy),Point(endx,starty),color , LINEW, 8 );
    return 0;
}



std::string _strdate_alternative()
{
	char cptime[50];
	time_t now = time(NULL);
	strftime(cptime, 50, "%d/%b/%Y %H:%M:%S", localtime(&now)); //short month name
	return std::string(cptime);
}
