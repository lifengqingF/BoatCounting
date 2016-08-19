//
//  frame_detect.h
//  frame_detect
//
//  Created by Li Fengqing on 8/20/15.
//  Copyright (c) 2015 Li Fengqing. All rights reserved.
//

#ifndef __fishingboat__fishingboatff__
#define __fishingboat__fishingboatff__
#include <iostream>
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include "inputoutput.h"
#include "basefunctions.h"
#include "frame_detect.h"
#include <vector>
using namespace std;
using namespace cv;
std::string _strdate_alternative();



class BoatCountEngine: public baseFuncClass
{

	vector <Mat> input_sequence;
	vector <Mat> output_info_sequence;

	Mat D;
	Mat K;
	Mat A;
	Mat C;

	Mat lastbackground;
	Mat background;
	Mat currentview;
	Mat lastview;
	Mat changeswithLast;
	Mat currentchangeswithBackground;


    Mat* last;
    Mat* current;
    int average_brightness;
    int framecount;
    int boatcount;
    int visualize;
    
    string mPath;
    string mFilename;

public:
    vector<frame_detect*> frames;
    int waterline ;
    
    BoatCountEngine(int visual)
    {
        last = NULL;
        current = NULL;
        waterline = 0;
        boatcount = 0;
        framecount = 0;
        average_brightness = 0;
        visualize = visual;
    };
	int calculate(vector<Mat> &intput, Mat&info);
	int potential_objects_detection(Mat& last_info, Mat& add, Mat&sub, Mat& info);

    int setfilename(string path, string name);
    int process(Mat & image);
    int sort_addup_objects(Mat & image);
    vector<Object>::iterator checkthis( vector<Object>::iterator i, vector<Object>& objects_last, vector<Object>& objects_current, vector<Object>& objects_overlap, Rect& rect);
    int print_statistics(string day);
	int maintainBackground();
	int findObjectCandidate(Mat& image);
};


#endif /* defined(__fishingboat__fishingboatff__) */
