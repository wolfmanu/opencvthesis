#include "stdafx.h"
#include "cameraInfo.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
//#include <cvaux.h>
#include <highgui.h>
#include <iostream>

using namespace std;

//public ctor
cameraInfo::cameraInfo():
rawimg(),
unimg()
{
	NameCamera = " void";
	Description = " void";
};


cameraInfo::cameraInfo(CvImage src)
{
	cameraInfo();
	rawimg = src.clone();
	unimg  = src.clone();


	size =  src.height() * src.width();
	tablex = new int[size];
	tabley = new int[size];
}



void cameraInfo::loadImage(CvImage src)
{
	//cameraInfo();
	rawimg = src.clone();
	unimg  = src.clone();


	size =  src.height() * src.width();
	tablex = new int[size];
	tabley = new int[size];
}




  //public dtor
cameraInfo::~cameraInfo()
{
	rawimg.release();
	unimg.release();
	delete []tablex;
	delete []tabley;
}
