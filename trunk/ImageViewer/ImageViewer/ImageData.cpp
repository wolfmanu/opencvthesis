/****************************************************************************
ImageData.cpp : Implementation file for the CImageData class
written by PJ Arends
pja@telus.net

For updates check http://www.codeproject.com/tools/imageviewer.asp

-----------------------------------------------------------------------------
This code is provided as is, with no warranty as to it's suitability or usefulness
in any application in which it may be used.

This code may be used in any way you desire. This file may be redistributed by any
means as long as it is not sold for profit, and providing that this notice and the
author's name are included. Any modifications not made by the original author should
be clearly marked as such to remove any confusion between the original version and
any other versions.

If any bugs are found and fixed, a note to the author explaining the problem and
fix would be nice.
-----------------------------------------------------------------------------
****************************************************************************/

#include "stdafx.h"
#include "ImageData.h"
#include <shlwapi.h>

// default c'tor
CImageData::CImageData():  
immagine(),
backup(),
backupFilter(),
backupRestore(),
CurrentZoomRatio(4),
thresholdVal(-1),
blurSize(-1),
blurType(-1),
laplDepth(-1),
thresh1(-1), thresh2(-1)
{
}

CImageData::~CImageData()
{
	immagine.release();
	backup.release();
	backupRestore.release();
	backupFilter.release();
}

Pose_Marker CImageData::getPoseMarker()
{
	return posemarker;
}
void CImageData::setPoseMarker(Pose_Marker pm)
{
	posemarker=pm;
}

void CImageData::setImage(Mat imm)
{
	if(backupRestore.empty())
		backupRestore=imm.clone();

	extern CImageData imagedata;
	extern double ZoomRatios[];

	backup.release();
	backup = imm.clone();

	double factor;
	factor = ZoomRatios[imagedata.CurrentZoomRatio];
	
	immagine.release();
	immagine.create( cvSize( imm.cols*factor, imm.rows*factor ), imm.type());
	
	resize(backup,immagine,immagine.size(),0,INTER_LANCZOS4);
}

void CImageData::setBlurFilter(int ksize)
{
	cv::Mat imgFilter;

	if(ksize==-1)
		blurType=-1;

	blurSize=ksize;
	if(blurType!=-1)
	{
		Size s(blurSize, blurSize);
		switch (blurType)
		{
		case IDC_RADIO_BLUR:
			blur(backup, imgFilter, s);
			break;
		case IDC_RADIO_GAUSSIANBLUR:
			GaussianBlur(backup, imgFilter, s,0);
			break;
		case IDC_RADIO_MEDIANBLUR:
			medianBlur(backup, imgFilter, blurSize);
			break;
		}
	}
	setImage(imgFilter);
}

void CImageData::setBackupFilter()
{
	backupFilter=immagine.clone();
}

void CImageData::setBackupHoughDetection()
{
	if(immagine.channels()>1)
	{
		cvtColor(immagine, backupFilter, CV_RGB2GRAY);
		Canny( backupFilter, backupFilter, 50, 200, 3 );
	}
	else
		Canny( immagine, backupFilter, 50, 200, 3 );
}

void CImageData::setBackupCannyDetection()
{
	if(immagine.channels()>1)
		cvtColor(immagine, backupFilter, CV_RGB2GRAY);

}

void CImageData::testBlurFilter(int bt, int ksize)
{
	blurType=bt;
	Size s(ksize, ksize);
	switch (blurType)
	{
	case IDC_RADIO_BLUR:
		blur(backupFilter, immagine, s);
		break;
	case IDC_RADIO_GAUSSIANBLUR:
		GaussianBlur(backupFilter, immagine, s,0);
		break;
	case IDC_RADIO_MEDIANBLUR:
		medianBlur(backupFilter, immagine, ksize);
		break;
	}	
}

void CImageData::testThreshFilter(double thresh)
{
	threshold(backupFilter, immagine, thresh, 255, THRESH_BINARY);
}
void CImageData::setThreshFilter(double thresh)
{
	cv::Mat imgFilter;
	if(thresholdVal>=0)
	{
		threshold(backup, imgFilter, thresholdVal, 255, THRESH_BINARY);
		setImage(imgFilter);
	}
	else
		setImage(backup);
}

void CImageData::setBNFilter()
{
	if(backup.channels()>1)
	{
		Mat dst;
		cvtColor(backup, dst, CV_RGB2GRAY);
		setImage(dst);
	}
}

void CImageData::resetAllFilter()
{
	thresholdVal=-1;
	blurType=-1;
	laplDepth=-1;
	setImage(backupRestore);
}

void CImageData::setErodeFilter()
{
	Mat dst;
	erode(backup, dst, Mat());
	setImage(dst);
}
void CImageData::setDilateFilter()
{
	Mat dst;
	dilate(backup, dst, Mat());
	setImage(dst);
}
void CImageData::setLaplacianFilter()
{
	Mat dst;
	Laplacian(backup, dst, backup.depth());
	setImage(dst);
}

void CImageData::testCannyFilter(double t1,double t2)
{
	Canny(backupFilter, immagine, t1, t2);
}

void CImageData::setCannyFilter(double t1,double t2)
{
	Mat dst;
	Canny(backupFilter, dst, t1, t2);
	setImage(dst);
}

void CImageData::testLineDetection(double rho_,double teta_, double thresh_)
{
	vector<Vec2f> lines;
	setImage(backup);
	if(rho_>0 && teta_>0 && thresh_>0)
	{
		HoughLines( backupFilter, lines, rho_, (teta_*CV_PI)/180, thresh_ );

		for( size_t i = 0; i < lines.size(); i++ )
		{
			float rho = lines[i][0];
			float theta = lines[i][1];
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			cv::Point pt1(cvRound(x0 + 1000*(-b)),
					  cvRound(y0 + 1000*(a)));
			cv::Point pt2(cvRound(x0 - 1000*(-b)),
					  cvRound(y0 - 1000*(a)));
			line(immagine, pt1, pt2, Scalar(0,0,255), 1, 8 );
		}
	}
}

void CImageData::setLineDetection(double rho_,double teta_, double thresh_)
{
	vector<Vec2f> lines;
	HoughLines( backupFilter, lines, rho_, teta_, thresh_ );

	for( size_t i = 0; i < lines.size(); i++ )
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		cv::Point pt1(cvRound(x0 + 1000*(-b)),
				  cvRound(y0 + 1000*(a)));
		cv::Point pt2(cvRound(x0 - 1000*(-b)),
				  cvRound(y0 - 1000*(a)));
		line(immagine, pt1, pt2, Scalar(0,0,255), 1, 8 );
	}
	setImage(immagine);
}

HBITMAP CImageData::GetBitmap() 
{
	HBITMAP bm=NULL;
	bm=Utils::IplImage2DIB(immagine);
	return bm;
}

