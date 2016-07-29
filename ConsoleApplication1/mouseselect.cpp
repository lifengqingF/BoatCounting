//
//  mouseselect.cpp
//  vehicleDetection
//
//  Created by Li Fengqing on 9/20/15.
//  Copyright (c) 2015 Li Fengqing. All rights reserved.
//

#include "mouseselect.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include "inputoutput.h"


using namespace cv;
using namespace std;

bool leftDown=false,leftup=false;
Point cor1,cor2;
Rect box;


Rect boat(0,0,100,100);
Rect car(100,0,100,100);
Rect others(200,0,100,100);


class images
{
public:
    vector<string> fileNames;
    int current;
    string m_dir_path;
    ofstream myfile;
    images()
    {
        current = 0;
    }
    int shownext()
    {
        Mat currentimage;
        if(current < fileNames.size())
        {
            string fileName = fileNames[current];
            string fileFullName = m_dir_path + fileName;
            cout<<to_string(current) <<endl;
            currentimage = imread(fileFullName, 1 );
            imshow("object", currentimage);
            current++;
        }
        else{
            closefile();
            cout << "finished all test"<< endl;
        }

        return 1;
    }
    int recordtype(int type)
    {
        myfile << fileNames[current]+"\t"+ to_string(type) + "\n" ;
        return 0;
    }

    
    int setdir(string dir_path)
    {
        OneDayBoatImageDirectory dir;
        m_dir_path = dir_path;
        fileNames = dir.GetListFiles(dir_path, ".jpg", false);
        
      
        myfile.open (dir_path + "label.txt");
      //  myfile.close();
        return 0;
        
    }
    int closefile()
    {
         myfile.close();
        return 0;
    }
};


images myimages;

void mouse_call(int event,int x,int y,int,void*)
{
    if(event==EVENT_LBUTTONDOWN)
    {
        cor1.x=x;
        cor1.y=y;
        cout <<"Corner 1: "<<cor1<<endl;
        
    }
    if(event==EVENT_LBUTTONUP|| event == EVENT_MOUSEMOVE)
    {
        if (boat.contains(Point(x,y)) ) {
            cout<< "boat" <<endl;
            myimages.recordtype(2);
        }
        if (car.contains(Point(x,y)) ) {
            cout<< "car" <<endl;
            myimages.recordtype(1);

        }
        if (others.contains(Point(x,y)) ) {
            cout<< "others" <<endl;
            myimages.recordtype(0);

        }
         myimages.shownext();
    }
    
}

int testcasemouse(string dir_path);

int mouse_main(int argc, char** argv)
{
    string dir_path = "/Users/lifengqing/Desktop/image/2010_02_07/objects/";
    myimages.setdir(dir_path);
    testcasemouse(dir_path);
    myimages.shownext();
    while(char(waitKey(1)!='q')) //waiting for the 'q' key to finish the execution
    {
        
    }
    return 0;
}

Scalar colorboat(0,0,255);
Scalar colorcar(0,255,0);
Scalar colorothers(255,0,0);

int testcasemouse(string dir_path)
{

    Mat Button = Mat::zeros(100, 300, CV_8UC3);
    rectangle(Button,boat, colorboat,2,8,0); //
    rectangle(Button,car, colorcar,2,8,0); //
    rectangle(Button,others, colorothers,2,8,0); //
    putText(Button, "boat", boat.tl() + Point(0,50), FONT_HERSHEY_SIMPLEX, 1,colorboat, 2);
    putText(Button, "car", car.tl() + Point(0,50), FONT_HERSHEY_SIMPLEX, 1,colorcar, 2);
    putText(Button, "others", others.tl() + Point(0,50), FONT_HERSHEY_SIMPLEX, 1,colorothers, 2);
    imshow("button vote", Button);
    setMouseCallback("button vote",mouse_call); //setting the mouse callback for selecting the region with mouse

return 0;
}
