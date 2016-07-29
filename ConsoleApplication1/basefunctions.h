//
//  basefunctions.h
//  opencv_test
//
//  Created by Li Fengqing on 8/18/15.
//  Copyright (c) 2015 Li Fengqing. All rights reserved.
//

#ifndef __opencv_test__basefunctions__
#define __opencv_test__basefunctions__
#include <iostream>
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include "macros.h"
#include <vector>
using namespace cv;
using namespace std;

class Object:public Rect
{
public:
    Object()
    {
        average = 0;
        myid = 0;
        newflag = 1;
        type = 0;
        validpoints = 0;
    }
    Object(Rect a):Rect(a)
    {
        
        average = -1;
        addupaverage = -1;
        myid = 0;
        newflag = 1;
        type = 0;
        validpoints = 0;
    }
    int average;
    int myid;
    int newflag;
    int addupaverage;
    int type;
    int validpoints;
};



class baseFuncClass
{
public:
    Point mValidPoints[VALIDPOINTMAX];
    int validpointscount ;
public:
    Point getUpPoint(Point* lines,int index);
    Point getDownPoint(Point* lines,int index);
    Point getUpPoint(Point* lines);
    Point getDownPoint(Point* lines);
    Point getPoint(Point pt1, Point pt2, int updown);
    Point getUpPoint(Point pt1, Point pt2);
    Point getDownPoint(Point pt1, Point pt2);
    int mycvCheckValidPoint(const Mat*img);
    int mycvGetValueOnLine(const Mat*img, Point pt1, Point pt2);
    float getHeight(Point pt1, Point pt2);
    float Getincline(Point pt1, Point pt2);
    float getWidth(Point pt1, Point pt2);
    float getLength(Point pt1, Point pt2);
    
    int CheckValidPercentage(const Mat*img);
    int  threshold( const Mat* srcimage, Mat*  dst, int valid_percentage ); // percentage is valid point from 1 to 100
    int get_averge_100(vector<int> &buf,int index);
    int get_averge(vector<int> &buf);
    int get_averge_inrange(vector<int> &buf,int index,int range);
    
    int get_size(int width, int height);
    int get_validpoint(Mat& image, Rect rect);
    float get_emptyrate(Mat& image, Rect rect);
    int averge9point(Mat&src, Mat&dst);
    int averge9point(Mat&src, Mat&dst, int times);
    int get_max(vector<int> &buf);
    int overlay(Rect a, Rect b, Rect &c);
    int drawrect(Mat& image, Rect rect);
    int get_average_inrect(Mat& image, Rect rect);
    int get_thirdthaverage(Mat& image);
    int get_average(Mat& image);
    int getoverlaprate(Rect a, Rect b);








};


std::string _strdate_alternative();


#endif /* defined(__opencv_test__basefunctions__) */

