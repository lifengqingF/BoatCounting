
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

#define TIME_SERIES_RANGE 16

int BoatCountEngine::potential_objects_detection(Mat& last_info, Mat& add, Mat&sub, Mat& info)
{
	Mat add_threshhold;
	Scalar mean_scalar =  mean(add);
	int thresh = mean_scalar[0] + 10;
	::threshold(add, add_threshhold, thresh, 255, CV_THRESH_TOZERO); //找出骨节

	info = last_info + add_threshhold;
	info = info - sub;
	 mean_scalar = mean(info);
	 thresh = mean_scalar[0] + 10;
	::threshold(info, info, thresh, 255, CV_THRESH_TOZERO); //找出骨节


#if 0
	for (int i = 0; i < info.rows; i++)
	{
		for (int j = 0; j < info.cols; j++)
		{
			if (last_info.at<uchar>(Point(j, i)) > 0 && diff_threshhold.at<uchar>(Point(j, i)) > 0)
			{
				info.at<uchar>(Point(j, i)) = 0;
			}
			else if (last_info.at<uchar>(Point(j, i)) == 0 && diff_threshhold.at<uchar>(Point(j, i)) == 0)
			{
				info.at<uchar>(Point(j, i)) = 0;
			}
			else if (last_info.at<uchar>(Point(j, i)) > 0 && diff_threshhold.at<uchar>(Point(j, i)) == 0)
			{
				info.at<uchar>(Point(j, i)) = 255;
			}
			else if (last_info.at<uchar>(Point(j, i)) == 0 && diff_threshhold.at<uchar>(Point(j, i)) > 0)
			{
				info.at<uchar>(Point(j, i)) = 255;
			}
		}
	}
#endif 
	return 0;
}


int BoatCountEngine::calculate(vector<Mat> &input, Mat&info)
{
	for (int i = 0; i < info.rows; i++)
	{
		for (int j = 0; j < info.cols; j++)
		{
			int mean = 0;
			Vector<int> distance;
			for (auto& element : input)
				mean += element.at<uchar>(Point(j, i));
			mean /= input.size();
			int distance_mean = 0;
			for (auto& element : input)
			{
				int distance_value = std::abs(element.at<uchar>(Point(j, i)) - mean);
				distance_mean += distance_value;
				distance.push_back(distance_value);
			}
			distance_mean /= distance.size();
			int last_distance = distance.back();
			if (last_distance  - distance_mean > 30)
			{
				info.at<uchar>(Point(j, i)) = 255;
			}
			else
				info.at<uchar>(Point(j, i)) = 0;

		}
	}
	return 0;
}
static int index = 0;
int BoatCountEngine::process(Mat & image)
{
#if 0
	int i = 7;
	Mat blur = image.clone();
	GaussianBlur(image, blur, Size(i, i), 0, 0);

	// Create a kernel that we will use for accuting/sharpening our image
#if 1
	Mat kernel = (Mat_<float>(5, 5) <<
		1,1, 1, 1,1,
		1,1, 1, 1,1,
		1,1,-24, 1, 1,
		1,1, 1, 1, 1,
		1,1, 1, 1, 1
		);
#else
	Mat kernel = (Mat_<float>(3, 3) <<
		1, 1, 1,
		1, -8, 1, 
		1, 1, 1
		);
#endif
	// an approximation of second derivative, a quite strong kernel
				  // do the laplacian filtering as it is
				  // well, we need to convert everything in something more deeper then CV_8U
				  // because the kernel has some negative values,
				  // and we can expect in general to have a Laplacian image with negative values
				  // BUT a 8bits unsigned int (the one we are working with) can contain values from 0 to 255
				  // so the possible negative number will be truncated
	Mat imgLaplacian;
	Mat sharp = blur; // copy source image to another temporary one
	filter2D(sharp, imgLaplacian, CV_32F, kernel);
	blur.convertTo(sharp, CV_32F);
	Mat imgResult = sharp - imgLaplacian;
	// convert back to 8bits gray scale
	imgResult.convertTo(imgResult, CV_8UC3);
	imgLaplacian.convertTo(imgLaplacian, CV_8UC3);
	Mat img_Strong = imgResult + imgLaplacian;

	imshow( "Laplace Filtered Image", imgLaplacian );
	imshow("New Sharped Image", imgResult);
	imshow(" strong ", img_Strong);

	Mat img_Strong_threshhold;
	Scalar mean_scalar = mean(img_Strong);
	int thresh = mean_scalar[0] + 10;
	::threshold(img_Strong, img_Strong_threshhold, thresh, 255, CV_THRESH_TOZERO); //找出骨节

	imshow(" img_Strong_threshhold ", img_Strong_threshhold);

	Mat imgLaplacian_threshhold;
	Scalar mean_scalar_imgLaplacian = mean(imgLaplacian);
	int thresh_imgLaplacian = mean_scalar_imgLaplacian[0] + 10;
	::threshold(imgLaplacian, imgLaplacian_threshhold, thresh_imgLaplacian, 255, CV_THRESH_TOZERO); //找出骨节

	imshow("imgLaplacian_threshhold ", imgLaplacian_threshhold);


	imshow(" original ", image);
	waitKey(0);

	return 0;
#endif 

#if 0

	while (input_sequence.size() < TIME_SERIES_RANGE)
	{
		input_sequence.push_back(image);
	}
	Mat absdiff_;
	Mat add;
	Mat remove;
	absdiff(input_sequence.back(), image, absdiff_);
	add = image - input_sequence.back();
	remove = input_sequence.back() - image;


	input_sequence.erase(input_sequence.begin());
	input_sequence.push_back(image);


	Mat info;
	info = Mat::zeros(image.rows, image.cols, image.type());


	while (output_info_sequence.size() < TIME_SERIES_RANGE)
	{
		output_info_sequence.push_back(info);
	}
	calculate(input_sequence, info);
	potential_objects_detection(output_info_sequence.back(), add, remove, info);

	output_info_sequence.erase(output_info_sequence.begin());
	output_info_sequence.push_back(info);

	int x = 0, y = 0;
#define SCREEN_W 1500
#define TITLE_H 0

	for (int i = 0; i < TIME_SERIES_RANGE; i++)
	{
		namedWindow("source " + to_string(i + index));
		imshow("source " + to_string(i + index), input_sequence[i]);
		moveWindow("source " + to_string(i + index),x%SCREEN_W, (x/ SCREEN_W)*(input_sequence[i].rows+ TITLE_H)*2 + y );
		x += input_sequence[i].cols;
	}

	x = 0; y = input_sequence[0].rows+ TITLE_H;
	for (int i = 0; i < TIME_SERIES_RANGE; i++)
	{
		namedWindow("out " + to_string(i + index));
	imshow("out " + to_string(i + index), output_info_sequence[i]);
	moveWindow("out " + to_string(i + index), x % SCREEN_W, (x/ SCREEN_W)*(output_info_sequence[i].rows+ TITLE_H) * 2 + y);
	x += input_sequence[i].cols;

	}
	namedWindow("diff");
	imshow("diff", absdiff_);
	namedWindow("add");
	imshow("add", add);
	namedWindow("remove");
	imshow("remove", remove);

	waitKey(0);
	//index++;
	return 0 ;
#endif


#if 0
	if (A.empty())
	{

		A = Mat::zeros(image.rows,image.cols, image.type());
	}
	if (D.empty())
	{
		image.copyTo(D);
		D = Mat::zeros(image.rows, image.cols, image.type());
	}
	if (K.empty())
	{
		image.copyTo(K);
		K = Mat::zeros(image.rows, image.cols, image.type());

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

	rectangle(lastview, Rect(lastview.cols / 2, lastview.rows / 2, 3, 3), Scalar(255, 0, 0));
	rectangle(lastview, Rect(lastview.cols / 2, lastview.rows / 2, 5, 5), Scalar(0, 255, 0));
	rectangle(lastview, Rect(lastview.cols / 2, lastview.rows / 2, 7, 7), Scalar(0, 0, 255));
	rectangle(lastview, Rect(lastview.cols / 2, lastview.rows / 2, 9, 9), Scalar(255, 0, 0));
	rectangle(lastview, Rect(lastview.cols / 2, lastview.rows / 2, 11, 11), Scalar(0, 255, 0));
	rectangle(lastview, Rect(lastview.cols / 2, lastview.rows / 2, 13, 13), Scalar(0, 0, 255));


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
        ObjectDetection* detect_engine = new ObjectDetection(waterline,visualize);
    if (last !=NULL) {
        Mat image1gray;
        Mat image2gray;
        cvtColor(*last, image1gray, CV_RGB2GRAY);
        cvtColor(*current, image2gray, CV_RGB2GRAY);
        detect_engine->setfilename(mPath, mFilename);
        detect_engine->detect_waterline(image1gray,image2gray); //find out waterline first  // find water line test
        detect_engine->DetectObjects(image1gray, image2gray);
        frames.push_back(detect_engine);
        sort_addup_objects(image1gray);
              if (visualize) {
            int haveitems =  detect_engine->drawit(show, boatcount);
            putText(show, "frame C:" + to_string(framecount) + " boat C:"+ to_string(boatcount) + " B: " + to_string(average_brightness) , Point(25, 25), FONT_HERSHEY_SIMPLEX, 1., Scalar(255, 100, 0), 2);
            imshow("current", show);
       //     imshow("last", *last);
            if(haveitems)
            {
                
               char ch = waitKey(1);
                if(ch == ' ')
                    waitKey();
            } 
            else
            {
                waitKey(1);
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



int BoatCountEngine::sort_addup_objects(Mat& image)
{
    int framecount = frames.size();
    if (framecount >=2) {
        ObjectDetection* currentframe = frames[framecount-1];
        ObjectDetection* lastframe = frames[framecount-2];
        vector<Object>* objects_last = &(lastframe->positive_objects);
        vector<Object>* objects_current = &(currentframe->positive_objects);
        vector<Object> objects_overlap;

		Rect rect = lastframe->waterlineRect(image);
		average_brightness = currentframe->average_brightness;
        for( vector<Object>::iterator i=(currentframe->positive_objects).begin(); i !=(currentframe->positive_objects).end() && (currentframe->positive_objects).size()>0 ; ++i )
        {
                    
			Rect tempRC;
			if (overlay(rect, *i, tempRC))
			{

				vector<Object>::iterator temp = checkthis(i, lastframe->positive_objects, currentframe->positive_objects, objects_overlap, rect);
				if (temp != i)
				{ //overlap found
					i = temp;
				}
				else
				{

				}
			}
        }
    }
    return 0;
}
vector<Object>::iterator  BoatCountEngine::checkthis( vector<Object>::iterator current_obj_itr, vector<Object>& objects_last, vector<Object>& objects_current, vector<Object>& objects_overlap , Rect& rect)
{
    
    
    for( vector<Object>::iterator last_obj_itr=objects_last.begin(); last_obj_itr != objects_last.end() && objects_current.size()>0 && objects_last.size()>0; ++last_obj_itr)
    {
        
        //find if two rect overlapped
        //if it is overlapd.
        //delete it from objects_sorted
        //delete it from objects
        //append the merge one on the end of objects
        //break; no need to compare with the rest, for the new one will take it place
		Rect tempRC;
        int getcomfirmed = 0;
        Object newone;
        if (overlay(*current_obj_itr,*last_obj_itr,newone) 	&&  overlay(rect, *last_obj_itr, tempRC)) {
            //there is with overlap found i
            if ((*last_obj_itr).myid == 0 && (*current_obj_itr).myid == 0) { //apparently this one is new
                //判断overlap rate， 静止物体的判断规则是至少overlap 70%
                if(average_brightness <=NIGHTBRIGHTNESSSTHRESHHOLD)
                {
                    (*current_obj_itr).myid = boatcount++;
                }
                else if(getoverlaprate(*last_obj_itr, *current_obj_itr) >40 /*&&  (*current_obj_itr).type && (*current_obj_itr).addupaverage - (*current_obj_itr).average <BRIGHTNESSDIFF  &&  (*current_obj_itr).average - (*current_obj_itr). addupaverage < BRIGHTNESSDIFF*/)
                {
                    (*current_obj_itr).myid = boatcount++;
                }

                
                //否则并不认为是overlap，不会判断为新物体出现。
                
            }
            else //但是一旦匹配过一次之后，随后就宽松了，只要大致重合，就认为还是原来的。
            {
                (*current_obj_itr).myid =(*last_obj_itr).myid; //继承旧的ID。
                (*current_obj_itr).newflag = 0;   //设置new flag 为 0.
                 //already have an id
            }
        }
    }
#if 0
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
#endif 

    return current_obj_itr;
    
}

int BoatCountEngine::print_statistics(string day)
{
    cout<<"#Test day:"<<day <<" Boat Count:"<<boatcount<<  " complete at " << _strdate_alternative() << endl;
    return 0;
}

