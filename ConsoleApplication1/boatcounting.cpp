//
//  main.cpp
//  vehicleDetection
//
//  Created by Li Fengqing on 9/3/15.
//  Copyright (c) 2015 Li Fengqing. All rights reserved.
//


#include "fishingboat.h"

using namespace std;
using namespace cv;

int test_on_one_day_images(string dir_path);


int main(int argc, char** argv)
{
	cout << " ----- start test ----- " << _strdate_alternative() << endl;
	//batch test
	if (argc < 2)
	{
		cout << "Usage: boatcounting.exe  [imagefold] <batch excute>" << endl;
		waitKey();
		return 0;
	}
	if (argc == 2)
	{
		cout << "test on a single day data %s" << argv[1] << endl;
		test_on_one_day_images(argv[1]);
	}
	else
	{
		string dir_path = argv[1];
		Directory dir;
		vector<string> folderNames = dir.GetListFolders(dir_path, "", false);
		for (int i = 0; i < folderNames.size(); i++) {
			string foldName = folderNames[i];
			string foldFullName = dir_path + foldName + "/";
			cout << "fold name:" << foldName << endl;
			cout << "file path:" << foldFullName << endl;
			test_on_one_day_images(foldFullName);
		}
	}
	cout << " ----- end test ----- " << _strdate_alternative() << endl;
	waitKey(0);
	return 0;
}

#define VISUAL 1
int test_on_one_day_images(string dir_path)
{
	OneDayBoatImageDirectory dir;
	vector<string> image_filename_lists = dir.GetListFiles(dir_path, ".jpg", false);
	Mat currentimage;
	class BoatCountEngine boat_counting_engine(VISUAL);
	for (int i = 0; i < image_filename_lists.size(); i++)
	{
		string fileName = image_filename_lists[i];
		string fileFullName = dir_path + "/"+ fileName;
		//  cout<<"file name:"<<fileName<<endl;
		currentimage = imread(fileFullName, 1);
		Mat src_gray;
		cvtColor(currentimage, src_gray, CV_BGR2GRAY);
		boat_counting_engine.setfilename(dir_path, fileName);
		Mat dst(currentimage.rows/3, currentimage.cols/3, src_gray.type());
		resize(src_gray, dst, dst.size(), 0, 0, INTER_AREA);

		Mat signed_dst;
		dst.convertTo(signed_dst, CV_32SC1);

		if (src_gray.rows > 0)
		{
			boat_counting_engine.process(currentimage);

		}
		else
		{
			cout << "fail to read file:" << fileName << endl;
		}
	}
	boat_counting_engine.print_statistics(dir_path);
	return 0;
}


