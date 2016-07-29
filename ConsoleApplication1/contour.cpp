//
//  contour.cpp
//  fishingboat
//
//  Created by Li Fengqing on 8/22/15.
//  Copyright (c) 2015 Li Fengqing. All rights reserved.
//

#include "contour.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <iostream>
#include "macros.h"

using namespace cv;
using namespace std;

const int mMAX_KERNEL_LENGTH = 31;

int i = 0;

static void help()
{
    cout
    << "\nThis program illustrates the use of findContours and drawContours\n"
    << "The original image is put up along with the image of drawn contours\n"
    << "Usage:\n"
    << "./contours2\n"
    << "\nA trackbar is put up which controls the contour level from -3 to 3\n"
    << endl;
}

const int w = 500;
int levels = 3;

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;

static void on_trackbar(int, void*)
{
    Mat cnt_img = Mat::zeros(w, w, CV_8UC3);
    int _levels = levels - 3;
    drawContours( cnt_img, contours, _levels <= 0 ? 3 : -1, Scalar(128,255,255),
                 3, CV_AA, hierarchy, std::abs(_levels) );
    
    imshow("contours", cnt_img);
}

int mmmmain( int argcccc, char**)
{
    Mat img = Mat::zeros(w, w, CV_8UC1);
#if 0
    if(argc > 1)
    {
        help();
        return -1;
    }
#endif
    //Draw 6 faces
    for( int i = 0; i < 6; i++ )
    {
        int dx = (i%2)*250 - 30;
        int dy = (i/2)*150;
        const Scalar white = Scalar(255);
        const Scalar black = Scalar(0);
        
        if( i == 0 )
        {
            for( int j = 0; j <= 10; j++ )
            {
                double angle = (j+5)*CV_PI/21;
                line(img, Point(cvRound(dx+100+j*10-80*cos(angle)),
                                cvRound(dy+100-90*sin(angle))),
                     Point(cvRound(dx+100+j*10-30*cos(angle)),
                           cvRound(dy+100-30*sin(angle))), white, 1, 8, 0);
            }
        }
        
        ellipse( img, Point(dx+150, dy+100), Size(100,70), 0, 0, 360, white, -1, 8, 0 );
        ellipse( img, Point(dx+115, dy+70), Size(30,20), 0, 0, 360, black, -1, 8, 0 );
        ellipse( img, Point(dx+185, dy+70), Size(30,20), 0, 0, 360, black, -1, 8, 0 );
        ellipse( img, Point(dx+115, dy+70), Size(15,15), 0, 0, 360, white, -1, 8, 0 );
        ellipse( img, Point(dx+185, dy+70), Size(15,15), 0, 0, 360, white, -1, 8, 0 );
        ellipse( img, Point(dx+115, dy+70), Size(5,5), 0, 0, 360, black, -1, 8, 0 );
        ellipse( img, Point(dx+185, dy+70), Size(5,5), 0, 0, 360, black, -1, 8, 0 );
        ellipse( img, Point(dx+150, dy+100), Size(10,5), 0, 0, 360, black, -1, 8, 0 );
        ellipse( img, Point(dx+150, dy+150), Size(40,10), 0, 0, 360, black, -1, 8, 0 );
        ellipse( img, Point(dx+27, dy+100), Size(20,35), 0, 0, 360, white, -1, 8, 0 );
        ellipse( img, Point(dx+273, dy+100), Size(20,35), 0, 0, 360, white, -1, 8, 0 );
    }
    //show the faces
    namedWindow( "image", 1 );
    imshow( "image", img );
    //Extract the contours so that
    vector<vector<Point> > contours0;
    findContours( img, contours0, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    
    contours.resize(contours0.size());
    for( size_t k = 0; k < contours0.size(); k++ )
        approxPolyDP(Mat(contours0[k]), contours[k], 3, true);
    
    namedWindow( "contours", 1 );
    createTrackbar( "levels+3", "contours", &levels, 7, on_trackbar );
    
    on_trackbar(0,0);
    waitKey();
    
    return 0;
}




#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

Mat src_gray;
int thresh = 10;
int max_thresh = 255;
RNG rng(12345);

/// Function header
void thresh_callback(int, void* );

/** @function main */
int testcontours( Mat& image ,int color)
{
    
    /// Convert image to gray and blur it
    if(color)
    {
    cvtColor( image, src_gray, CV_BGR2GRAY );
    }
    else
    {
        image.copyTo(src_gray);
        
    }
    blur( src_gray, src_gray, Size(3,3) );
    
    /// Create Window
    string source_window = "Source";
    namedWindow( source_window, CV_WINDOW_AUTOSIZE );
    imshow( source_window, image );
    
    createTrackbar( " Canny thresh:", "Source", &thresh, max_thresh, thresh_callback );
    thresh_callback( 0, 0 );
    return(0);
}

/** @function thresh_callback */


void thresh_callback(int, void* )
{
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    Mat grayblur;
    Mat graythreshhold;
  
    //src_gray.copyTo(grayblur);
   // bilateralFilter ( src_gray, grayblur, MAX_KERNEL_LENGTH, MAX_KERNEL_LENGTH*2, MAX_KERNEL_LENGTH/2 );
    int i = mMAX_KERNEL_LENGTH;
    blur( src_gray, grayblur, Size( i, i ), Point(-1,-1) );
    /// Detect edges using canny
    
      ::threshold(grayblur, graythreshhold, thresh*2, 255, CV_THRESH_TOZERO); //找出骨节
       Canny( graythreshhold, canny_output, thresh*2, thresh*2, 3 );
    
    // Canny( grayblur, canny_output, thresh*2, thresh*2, 3 );
    imshow( "canny_incontourtest", canny_output );

    /// Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
    /// Draw contours
    Mat drawing = Mat::zeros( src_gray.size(), CV_8UC3 );
    
#if 1
    //contours.resize(contours0.size());
    for( size_t k = 0; k < contours.size(); k++ )
        //approxPolyDP(Mat(contours[k]), contours[k], 3, true);
        convexHull(Mat(contours[k]), contours[k], true, true);
#endif
    
#if 1
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = CV_RGB(255, 255, 255);//Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
  
        drawContours( drawing, contours, i, color, CV_FILLED, 8, hierarchy, CV_RETR_EXTERNAL, Point() );
 //       drawContours( drawing, contours, i, color, 0, 8, hierarchy, CV_RETR_EXTERNAL, Point() );

#if 0
        Rect r= boundingRect(contours[i]);
        if(hierarchy[i][2]<0) //Check if there is a child contour
            rectangle(drawing,Point(r.x-10,r.y-10), Point(r.x+r.width+10,r.y+r.height+10), Scalar(0,0,255),2,8,0); //Opened contour
        else
            rectangle(drawing,Point(r.x-10,r.y-10), Point(r.x+r.width+10,r.y+r.height+10), Scalar(0,255,0),2,8,0); //closed contour
#endif
        
        
    }
#endif
    //   drawContours(drawing, contours, 0, Scalar(255), CV_FILLED);
    
    
    /// Show in a window
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    putText(drawing, "thresh:"+to_string(thresh), Point(10,19), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(100, 255, 0), 1);

    imshow( "Contours", drawing );
    waitKey();
}
/*
 cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(40, 40));
 cv::morphologyEx( inputImage, outputImage, cv::MORPH_CLOSE, structuringElement );
 */
