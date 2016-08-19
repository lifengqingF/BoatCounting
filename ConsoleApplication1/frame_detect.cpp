//
//  frame_detect.cpp
//  fishingboat
//
//  Created by Li Fengqing on 8/21/15.
//  Copyright (c) 2015 Li Fengqing. All rights reserved.
//

#include "frame_detect.h"
//
//  frame_detect.h
//  frame_detect.cpp
//
//  Created by Li Fengqing on 8/20/15.
//  Copyright (c) 2015 Li Fengqing. All rights reserved.
//

#include "fishingboat.h"

#include <iostream>
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include "inputoutput.h"
#include "basefunctions.h"
#include "macros.h"
using namespace std;
using namespace cv;
int frame_detect::detect_waterline(Mat& image1, Mat& image2)
{
    
    average_brightness = get_thirdthaverage(image1);
    Mat image1_2 = image1 - image2;
    Mat image2_1 = image2 - image1;
    Mat addup = image1_2 + image2_1;
    Mat addup_threshhold_zero_inverted;
    Mat addup_threshhold_zero_inverted_color;
    ::threshold(addup, addup_threshhold_zero_inverted,20,20 ,CV_THRESH_TOZERO_INV);
    Mat addup_threshhold_zero_inverted_canny;
    Mat addup_threshhold_zero_inverted_blur;
    int i = 8;
    bilateralFilter ( addup_threshhold_zero_inverted, addup_threshhold_zero_inverted_blur, i, i*2, i/2 );
    Canny(addup_threshhold_zero_inverted_blur, addup_threshhold_zero_inverted_canny, 10, 10);
    detectwaterline(addup_threshhold_zero_inverted_canny);
    if(visualize)
    {
    cvtColor(addup_threshhold_zero_inverted_canny, addup_threshhold_zero_inverted_color, CV_GRAY2BGR);
    line(addup_threshhold_zero_inverted_color, Point(0,waterline), Point(addup_threshhold_zero_inverted_color.cols-1, waterline), CV_RGB(255, 0, 0));
    }
    return 0;
}

int frame_detect::detectwaterline(Mat& image)
{

    int mywaterline = 0;
    vector<int> vertical_value;
    for(int i = 0; i < image.rows; i++)
    {
        int value = 0;
        for(int j = 0; j <image.cols; j++)
        {
            value += image.at<uchar>(Point(j,i)); // j first
            
        }
        vertical_value.push_back(value);
    }
    
    int average = get_averge(vertical_value);
    int maxpeak = 0;
    int maxpeakvalue =0;
    for(int i = image.rows/4; i < image.rows; i++)
    {
        int average100 = get_averge_100(vertical_value, i);
        int average20 = get_averge_inrange(vertical_value,i,20);
        if (average20 -average100 > maxpeak) {
            mywaterline = i;
            maxpeak = average20 -average100 ;
        }
    }
    
    if (mywaterline > image.rows*2/5 && mywaterline < image.rows*3/4 ) { //valid water line
        if( waterline > image.rows/3)
        {
            waterline = (waterline*9 + mywaterline)/10; //10次平均
        }
        else
        {
            waterline = mywaterline;
        }
    }
    else
    {  //fail to get valid information
      //  waterline = 0;
    
    }
    return 0;

}


extern int testcontours( Mat& image,int );



int frame_detect::erode(Mat & image , Mat& dst)
{
    int erosion_size = 5;  //max 21
    
    int erosion_elem = 0;
    int erosion_type = 0;
    if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
    else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
    else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
    Mat element = getStructuringElement( erosion_type,
                                        Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                        Point( erosion_size, erosion_size ) );
    /// Apply the erosion operation
    ::erode( image, dst, element );
    //imshow( "Erosion Demo", dst );
    return 0;
}
int frame_detect::dilate(Mat & image , Mat& dst)
{
    int dilate_size = 5;  //max 21
    
    int dilate_elem = 0;
    int dilate_type = 0;
    if( dilate_elem == 0 ){ dilate_type = MORPH_RECT; }
    else if( dilate_elem == 1 ){ dilate_type = MORPH_CROSS; }
    else if( dilate_elem == 2) { dilate_type = MORPH_ELLIPSE; }
    Mat element = getStructuringElement( dilate_type,
                                        Size( 2*dilate_size + 1, 2*dilate_size+1 ),
                                        Point( dilate_size, dilate_size ) );
    /// Apply the erosion operation
    ::dilate( image, dst, element );
    //   imshow( "Erosion Demo", dst );
    return 0;
}


int frame_detect::cropcontour(Mat& src_image, Mat& src_gray, Mat&dst_poly, Mat&dst_hull, int pn)
{
        Mat canny_output;
        vector<vector<Point> > contours;
    vector<vector<Point> > contourspoly;
    vector<vector<Point> > contourshull;
        vector<Vec4i> hierarchy;
        
        Mat grayblur;
        Mat graythreshhold;
    int thresh = 9;
        
        //src_gray.copyTo(grayblur);
        // bilateralFilter ( src_gray, grayblur, MAX_KERNEL_LENGTH, MAX_KERNEL_LENGTH*2, MAX_KERNEL_LENGTH/2 );
        int i =MAX_KERNEL_LENGTH;
        blur( src_gray, grayblur, Size( i, i ), Point(-1,-1) );
        /// Detect edges using canny
        
        ::threshold(grayblur, graythreshhold, thresh*2, 255, CV_THRESH_TOZERO); //找出骨节
        Canny( graythreshhold, canny_output, thresh*2, thresh*2, 3 );
        
        // Canny( grayblur, canny_output, thresh*2, thresh*2, 3 );
    if (visualize) {
                imshow( "canny_"+to_string(pn), canny_output );
    }

        
        /// Find contours
        findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
        
        /// Draw contours
        Mat drawingpoly = Mat::zeros( src_gray.size(), CV_8UC1 );
        Mat drawinghull = Mat::zeros( src_gray.size(), CV_8UC1 );
    
#if 1
        contourspoly.resize(contours.size());
        contourshull.resize(contours.size());
    for( size_t k = 0; k < contours.size(); k++ )
    {
            approxPolyDP(Mat(contours[k]), contourspoly[k], 3, true);
            convexHull(Mat(contours[k]), contourshull[k], 3, true);
    }
#endif
        
#if 1
        for( int i = 0; i< contours.size(); i++ )
        {
            Scalar color = CV_RGB(255, 255, 255);//Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
              Rect r= boundingRect(contours[i]);
            if (r.width*2 < r.height*3 && contourArea(contourspoly[i]) > 200) {
                string object_name = get_object_name(positive_objects.size());
            //    Mat crop(src_image,r);
            //    imwrite(object_name, crop);
                positive_objects.push_back(Object(r));
                drawContours( drawingpoly, contourspoly, i, color, /*CV_FILLED*/ 1, 8, hierarchy, CV_RETR_EXTERNAL, Point() );
                drawContours( drawinghull, contourshull, i, color, /*CV_FILLED*/1, 8, hierarchy, CV_RETR_EXTERNAL, Point() );
            }
            //       drawContours( drawing, contours, i, color, 0, 8, hierarchy, CV_RETR_EXTERNAL, Point() );
            
#if 0
            if(hierarchy[i][2]<0) //Check if there is a child contour
                rectangle(drawing,Point(r.x-10,r.y-10), Point(r.x+r.width+10,r.y+r.height+10), Scalar(0,0,255),2,8,0); //Opened contour
            else
                rectangle(drawing,Point(r.x-10,r.y-10), Point(r.x+r.width+10,r.y+r.height+10), Scalar(0,255,0),2,8,0); //closed contour
#endif
            
            
        }
#endif
        //   drawContours(drawing, contours, 0, Scalar(255), CV_FILLED);
        
    drawingpoly.copyTo(dst_poly);
    drawinghull.copyTo(dst_hull);    
#if 0
    if (visualize) {
        /// Show in a window
        namedWindow( "Contours"+to_string(pn), CV_WINDOW_AUTOSIZE );
        putText(drawingpoly, "thresh:"+to_string(thresh), Point(10,19), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(100, 255, 0), 1);
        imshow("Contours poly"+to_string(pn),drawingpoly );
        putText(drawingpoly, "thresh:"+to_string(thresh), Point(10,19), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(100, 255, 0), 1);
        imshow("Contours hull"+to_string(pn),drawinghull );
    }
#endif
 
    return 0;
}




int frame_detect::detect_objects_in_diff( Mat& image1, Mat& image2)
{
  
        Mat addup = image2 - image1;
        Mat deduce = image1 - image2;
    
      Mat addup_threshold;
        ::threshold(addup, addup_threshold, 30, 30, CV_THRESH_TOZERO); //
        Mat crop_addup_contour_poly;
        Mat crop_addup_contour_hull;
        cropcontour(image2,addup_threshold, crop_addup_contour_poly,crop_addup_contour_hull, 1);
    
    if (visualize) {
        imshow("addup_threshold", addup_threshold);
        imshow("crop_addup_contour_poly", crop_addup_contour_poly);
    }
#if 1
    Mat deduce_threshold;
    ::threshold(deduce, deduce_threshold, 30, 30, CV_THRESH_TOZERO); //
    Mat crop_deduce_contour_poly;
    Mat crop_deduce_contour_hull;
    cropcontour(image1,deduce_threshold, crop_deduce_contour_poly,crop_deduce_contour_hull, 0);
    if (visualize) {
           imshow("crop_deduce_contour_poly", crop_deduce_contour_poly);
    }
#endif
 
//TODO    sort_objects(); //check if objects rectangle overlap with each other. // not a good implementation
    return 0;

    
}
int frame_detect::sort_objects()//if we saved countour ,no need to check if rectangles will overlapped
{
    //    check if two overlap with each other. // nxn比较， 1 与任意数比较之后，可能范围扩大，扩大后的范围可能包含已经比较过的区域。哇，难道复杂度是 n!
    
    
    for( vector<Object>::iterator i=positive_objects.begin(); i != positive_objects.end(); ++i )
    {
        i = checkthis(i);
    }
    
    return 0;
}
int frame_detect::drawit(Mat& showme , int boatcount)
{
#define LINEW 1

    Mat input_copy;
    showme.copyTo(input_copy);
    int returnvalue = 0;
    line(showme,Point(0,waterline),Point(showme.cols-1,waterline),CV_RGB(255, 0, 0) , LINEW, 8 );

    Rect rect = waterlineRect(showme);
    Rect temp;
    for( vector<Object>::iterator j=positive_objects.begin(); j != positive_objects.end(); ++j)
    {
        if (overlay(rect, *j,temp ))
        {
        int startx = (*j).x;
        int starty = (*j).y;
        int endx = (*j).x + (*j).width -1;
        int endy = (*j).y + (*j).height -1;
            int boatid = (*j).myid;
            int addupaverage = (*j).addupaverage;
            int validpoints = (*j).validpoints;
            int average = (*j).average;
            int type = (*j).type;
            Scalar colorb = CV_RGB(255, 255, 0); //yellow ,not decided what this is
            if ((*j).newflag && (*j).myid != 0 ) //new and have id //
            {
                 colorb = CV_RGB(255, 0, 0);
                if (returnvalue <3) {
                             returnvalue = 3;
                }
       
            }else if ((*j).myid != 0) // have id but old green
            {
                colorb = CV_RGB(0, 255, 0);
                if (returnvalue < 2) {
                    returnvalue = 2;
                }
            
            }
            else
            {
                if (returnvalue <1) {
                         returnvalue = 1;
                }
            
            }
            
        line(showme,Point(startx,starty),Point(startx,endy),colorb , LINEW, 8 );
        line(showme,Point(startx,starty),Point(endx,starty),colorb , LINEW, 8 );
        line(showme,Point(endx,endy),Point(startx,endy),colorb , LINEW, 8 );
        line(showme,Point(endx,endy),Point(endx,starty),colorb , LINEW, 8 );
        line(showme,Point(startx,starty),Point(endx,endy),colorb , LINEW, 8 );
        line(showme,Point(endx,starty),Point(startx,endy),colorb , LINEW, 8 );
            putText(showme, "A:" + to_string(addupaverage) , Point(startx, starty-40), FONT_HERSHEY_SIMPLEX, 0.5,colorb, 1);
            putText(showme, "C:" + to_string(average) , Point(startx, starty-60), FONT_HERSHEY_SIMPLEX, 0.5, colorb, 1);
            putText(showme, "T:" + to_string(type) , Point(startx, starty-80), FONT_HERSHEY_SIMPLEX, 0.5,colorb, 1);
            putText(showme, "V:" + to_string(validpoints) , Point(startx, starty-100), FONT_HERSHEY_SIMPLEX, 0.5,colorb, 1);
            if ((*j).newflag && (*j).myid != 0 ) //new and have id //
            {
                putText(showme, "ID:" + to_string(boatid) , Point(startx, starty), FONT_HERSHEY_SIMPLEX, 0.5,colorb, 1);
                putText(showme, "NEW", Point(startx, starty-20), FONT_HERSHEY_SIMPLEX, 0.5, colorb, 1);
            }else if ((*j).myid != 0) // have id but old green
            {
                putText(showme, "ID:" + to_string(boatid) , Point(startx, starty), FONT_HERSHEY_SIMPLEX, 0.5, colorb, 1);
                putText(showme, "OLD", Point(startx, starty-20), FONT_HERSHEY_SIMPLEX, 0.5, colorb, 1);
                
            }
            else
            {
                putText(showme, "ND", Point(startx, starty-20), FONT_HERSHEY_SIMPLEX, 0.5, colorb, 1);
            }
                
        }
    }
    
    return returnvalue;
    
}

 vector<Object>::iterator frame_detect::checkthis( vector<Object>::iterator i)
{
    
    for( vector<Object>::iterator j=positive_objects.begin(); j != positive_objects.end(); ++j)
    {
        //find if two rect overlapped
        //if it is overlapd.
        //delete it from objects_sorted
        //delete it from objects
        //append the merge one on the end of objects
        //break; no need to compare with the rest, for the new one will take it place
        Object newone;
        if (overlay(*i,*j,newone)) {
            if ((*i).type ) { // keep the type info
                newone.type = (*i).type;
                newone.average = (*i).average;
                newone.addupaverage = (*i).addupaverage;
                newone.validpoints = (*i).validpoints;
            }
            else if ((*j).type ) {
                newone.type = (*j).type;
                newone.average = (*j).average;
                newone.addupaverage = (*j).addupaverage;
                newone.validpoints = (*j).validpoints;
            }
            else
            {
                newone.type = (*j).type;
                newone.average = (*j).average;
                newone.addupaverage = (*j).addupaverage;
                newone.validpoints = (*j).validpoints;
                printf("verlap but no keep still area wired\n");
            }
            
            i = positive_objects.erase(i);
            j = positive_objects.erase(j);
            positive_objects.push_back(newone);
            //后面这个还没检查过，所以退回一个。
            i = checkthis(i);
            return i;
        }
    }
    // if nothing matched.
    positive_objects.push_back(*i);
    return i;
    
}


