//
//  frame_detect.h
//  fishingboat
//
//  Created by Li Fengqing on 8/21/15.
//  Copyright (c) 2015 Li Fengqing. All rights reserved.
//

#ifndef __fishingboat__frame_detect__
#define __fishingboat__frame_detect__

#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include "inputoutput.h"
#include "basefunctions.h"

using namespace std;
using namespace cv;

class ObjectDetection: public baseFuncClass
{
public:
 
    vector<Object> positive_objects;
    vector<Object> negtive_objects;
    int waterline;
    int visualize;
    int average_brightness;
    string mPath;
    string mFilename;
    int setfilename(string path, string name)
    {
        mPath = path;
        mFilename = name;
        return 0;
    }
    string get_object_name(int index)
    {
        
        size_t pos = mFilename.find(".");
        string name = mFilename.substr(0,pos);
        string objectname = mPath + name + to_string(index) + ".jpg";
        return objectname;
    }
    
    size_t get_objects_count()
    {
        return positive_objects.size() +  negtive_objects.size();
    };
    
    ObjectDetection(int average_waterline,int visual)
    {
        waterline = average_waterline;
        average_brightness = 0;
        visualize = visual;
    };
    ~ObjectDetection()
    {
    }
    Rect  waterlineRect(Mat& image)
    {
    #define MONITORAREA 100
        Rect rect(0,0,image.cols, image.rows);
        rect.y =  waterline - (MONITORAREA/10)*(waterline - MONITORAREA/10)/(image.rows);
        int endy =  waterline + (MONITORAREA*9/10)*(waterline+ MONITORAREA*9/10)/(image.rows);
        rect.height = endy - rect.y;
        return rect;
    }
    int detectwaterline(Mat& image);
    int detect_waterline(Mat& image1, Mat& image2);
    int sort_objects();
    vector<Object>::iterator checkthis(vector<Object>::iterator i);
    int drawit(Mat& show, int count);
    int dilate(Mat & image , Mat& dst);
    int erode(Mat & image , Mat& dst);
  //  int cropcontour(Mat& src_gray, Mat&dst);
    int cropcontour(Mat& src_img,Mat& src_gray, Mat&dst_poly, Mat&dst_hull, int pn);
    int DetectObjects( Mat& image1, Mat& image2);
	int DetectObjects(Mat& image);

  
  


    
    
};

#endif /* defined(__fishingboat__frame_detect__) */
