
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
cameraInfo::cameraInfo()
{
NameCamera = " void";
Description = " void";

rawimg = NULL;
unimg = NULL;

};


cameraInfo::cameraInfo(IplImage* src)
{
cameraInfo();
rawimg = cvCloneImage(src);
unimg  = cvCloneImage(src);


size =  src->height * src->width;
tablex = new int[size];
tabley = new int[size];


}



void cameraInfo::loadImage(IplImage* src)
{
//cameraInfo();
rawimg = cvCloneImage(src);
unimg  = cvCloneImage(src);


size =  src->height * src->width;
tablex = new int[size];
tabley = new int[size];


}




  //public dtor
cameraInfo::~cameraInfo()
{
cvReleaseImage(&rawimg);
cvReleaseImage(&unimg);
delete []tablex;
delete []tabley;
}
