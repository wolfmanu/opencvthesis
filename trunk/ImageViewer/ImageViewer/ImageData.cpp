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
CurrentZoomRatio(4)
{
}

CImageData::~CImageData()
{
	immagine.release();
	backup.release();
}

Pose_Marker CImageData::getPoseMarker()
{
	return posemarker;
}
void CImageData::setPoseMarker(Pose_Marker pm)
{
	posemarker=pm;
}

void CImageData::setImage(CvImage imm)
{
	extern CImageData imagedata;
	extern double ZoomRatios[];
	backup = imm.clone();
	double factor;
	factor = ZoomRatios[imagedata.CurrentZoomRatio];
	//if(!immagine.is_valid())
	{
		immagine.create( cvSize( imm.width()*factor, imm.height()*factor ),
						imm.depth(), imm.channels() );
		//immagine->origin=imm->origin;
	}
	
	cvResize(backup, immagine);
}

HBITMAP CImageData::GetBitmap() 
{
	HBITMAP bm=NULL;
	bm=Utils::IplImage2DIB(immagine);
	return bm;
}

