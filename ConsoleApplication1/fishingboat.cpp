
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



int BoatCountEngine::setfilename(string path, string name)
{
    mPath = path;
    mFilename = name;
    return 0;
}

int BoatCountEngine::maintainBackground()
{

	Mat mask_keep;
	Mat mask_update;
	::threshold(changeswithLast, mask_keep, 1, 255, THRESH_BINARY);
	::threshold(changeswithLast, mask_update, 1, 255, THRESH_BINARY_INV);
	namedWindow("mask_keep");
	imshow("mask_keep", mask_keep);
	namedWindow("mask_update");
	imshow("mask_update", mask_update);
	Mat background_no_update;
	lastbackground.copyTo(background_no_update, mask_keep);
	//namedWindow("background_no_update");
	//imshow("background_no_update", background_no_update);
	Mat background_update;
	currentview.copyTo(background_update, mask_update);
	//namedWindow("background_update");
	//imshow("background_update", background_update);
	background = background_no_update + background_update;
	//namedWindow("lastbackground");
	namedWindow("background");
	imshow("background", background);

	return 0;
}


int BoatCountEngine::process(Mat & image)
{

#if 1
	if (A.empty())
	{

		A = Mat::zeros(image.rows,image.cols, CV_8UC3);
	}
	if (D.empty())
	{
		image.copyTo(D);
		D = Mat::zeros(image.rows, image.cols, CV_8UC3);
	}
	if (K.empty())
	{
		image.copyTo(K);
		K = Mat::zeros(image.rows, image.cols, CV_8UC3);

	}


	if (background.empty())
	{
		image.copyTo(background);  //initial background
	}
	background.copyTo(lastbackground); //got lastbackground
	//we need update background later
	if (currentview.empty())
	{
		image.copyTo(currentview);// initial currentview
	}
	currentview.copyTo(lastview); //got last view
	image.copyTo(currentview);   //got current view


	changeswithLast = (currentview - lastview) + (lastview - currentview);  //	changeswithLast // includes object show up or disappear .
	namedWindow("changeswithLast");
	imshow("changeswithLast", changeswithLast);
	currentchangeswithBackground = (currentview - background) + (background - currentview); // 		currentchangeswithBackground  //include object show up and keep still
	//namedWindow("currentchangeswithBackground");
	//imshow("currentchangeswithBackground", currentchangeswithBackground);
																							//	lastchangeswithBackground //include object disapear in current view and keep still in currentview;
	Mat last_A;
	Mat last_K; 
	Mat last_D;
	A.copyTo(last_A);
	K.copyTo(last_K);
	D.copyTo(last_D);
	A = changeswithLast - (last_A + last_K);
	D = changeswithLast - A;
	Mat K_temp;
	Mat K_temp2;
	K = (last_A + last_K) - D;
	
	//int average = baseFuncClass::get_average(K_temp);
	//::threshold(K_temp, K_temp2, 10, 255, CV_THRESH_TOZERO);
	//blur(K_temp2, K, Size(10, 10), Point(-1, -1));
	C = A + K;



	namedWindow("currentview");
	namedWindow("lastview");
	namedWindow("D");
	namedWindow("K");
	namedWindow("A");
	//imshow("lastbackground", lastbackground);
	imshow("currentview",currentview);
	imshow("lastview",lastview);
	imshow("D",D);
	imshow("K",K);
	imshow("A",A);
//	bitwise_and(drawing1, drawing2, res);     imshow("AND", res);
//	bitwise_or(drawing1, drawing2, res);      imshow("OR", res);
//	bitwise_xor(drawing1, drawing2, res);     imshow("XOR", res);
//	bitwise_not(drawing1, res);              imshow("NOT", res);

//background. initial background， 前后10个frame，去除极大值。如何排除。
	waitKey(0);

	return 0;
#endif


    Mat show;
    
    if (visualize) {
        image.copyTo(show);
    }
    if (framecount == 0) {
    }
         current = new Mat;
        image.copyTo(*current);
        frame_detect* detect_engine = new frame_detect(waterline,visualize);
    if (last !=NULL) {
        Mat image1gray;
        Mat image2gray;
        cvtColor(*last, image1gray, CV_RGB2GRAY);
        cvtColor(*current, image2gray, CV_RGB2GRAY);
        detect_engine->setfilename(mPath, mFilename);
        detect_engine->detect_waterline(image1gray,image2gray); //find out waterline first  // find water line test
        detect_engine->detect_objects_in_diff(image1gray, image2gray);
        frames.push_back(detect_engine);
        sort_addup_objects();
              if (visualize) {
            int haveitems =  detect_engine->drawit(show, boatcount);
            putText(show, "frame C:" + to_string(framecount) + " boat C:"+ to_string(boatcount) + " B: " + to_string(average_brightness) , Point(25, 25), FONT_HERSHEY_SIMPLEX, 1., Scalar(255, 100, 0), 2);
            imshow("current", show);
       //     imshow("last", *last);
            if(haveitems)
            {
                
               char ch = waitKey(30);
                if(ch == ' ')
                    waitKey();
            }
                  
            else
            {
               // waitKey();
            }
        }
        else
        {
           // cout << "Frame C: " << framecount << " Boat: " << boatcount << " BRT:" << average_brightness <<endl;
        }
        delete last;
    }
        last = current;
        waterline = detect_engine->waterline;
        
  
    return 0;
}



int BoatCountEngine::sort_addup_objects()
{
    int framecount = frames.size();
    if (framecount >=2) {
        frame_detect* currentframe = frames[framecount-1];
        frame_detect* lastframe = frames[framecount-2];
        vector<Object>* objects_last = &(lastframe->positive_objects);
        vector<Object>* objects_current = &(currentframe->positive_objects);
        vector<Object> objects_overlap;
        
        for( vector<Object>::iterator i=(currentframe->positive_objects).begin(); i !=(currentframe->positive_objects).end() && (currentframe->positive_objects).size()>0 ; ++i )
        {
                    
           vector<Object>::iterator temp = checkthis(i,lastframe->positive_objects,currentframe->positive_objects,objects_overlap);
            if(temp != i)
            { //overlap found
                i = temp;
            }
            else
            {
                
            }
        }
    }
    return 0;
}
vector<Object>::iterator  BoatCountEngine::checkthis( vector<Object>::iterator i, vector<Object>& objects_last, vector<Object>& objects_current, vector<Object>& objects_overlap)
{
    
    
    for( vector<Object>::iterator j=objects_last.begin(); j != objects_last.end() && objects_current.size()>0 && objects_last.size()>0; ++j)
    {
        
        //find if two rect overlapped
        //if it is overlapd.
        //delete it from objects_sorted
        //delete it from objects
        //append the merge one on the end of objects
        //break; no need to compare with the rest, for the new one will take it place
        int getcomfirmed = 0;
        Object newone;
        if (overlay(*i,*j,newone)) {
            //there is with overlap found i
            if ((*j).myid == 0) { //apparently this one is new
                //判断overlap rate， 静止物体的判断规则是至少overlap 70%
                if(average_brightness <=NIGHTBRIGHTNESSSTHRESHHOLD)
                {
                    (*i).myid = boatcount++;
                }
                else if(getoverlaprate(*i, *j) >80 &&  (*i).type && (*i).addupaverage - (*i).average <BRIGHTNESSDIFF  &&  (*i).average - (*i). addupaverage < BRIGHTNESSDIFF)
                {
                    (*i).myid = boatcount++;
                }

                
                //否则并不认为是overlap，不会判断为新物体出现。
                
            }
            else //但是一旦匹配过一次之后，随后就宽松了，只要大致重合，就认为还是原来的。
            {
                (*i).myid =(*j).myid; //继承旧的ID。
                (*i).newflag = 0;   //设置new flag 为 0.
                 //already have an id
            }
        }
    }
  
        if((*i).myid == 0)
        {
         //   printf("interesting, keep still type\n");
        //there is no overlap but i type shows overlap found ,wired
            if(average_brightness <=NIGHTBRIGHTNESSSTHRESHHOLD)
            {
                (*i).myid = boatcount++;
                //printf("at night but not matched overlap %d\n",boatcount);
            }
            else if(  (*i).type && (*i).addupaverage - (*i).average <BRIGHTNESSDIFF  &&  (*i).average - (*i). addupaverage < BRIGHTNESSDIFF)
            {
              //  printf("daylight but not matched overlap %d\n",boatcount);
                (*i).myid = boatcount++;
            }
            else
            {
            //printf("not qualified %d\n",(*i).addupaverage - (*i).average );
            }
        }

    return i;
    
}

int BoatCountEngine::print_statistics(string day)
{
    cout<<"#Test day:"<<day <<" Boat Count:"<<boatcount<<  " complete at " << _strdate_alternative() << endl;
    return 0;
}

