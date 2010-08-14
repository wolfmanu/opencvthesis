/****************************************************************************
ImageData.h : Header file for the CImageData class
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

#pragma once

//#include "..\Common Files\tstl.h"
//#include "BitmapInfoHeader.h"
#include "Pose_Marker.h"

class CImageData
{
public:
	CvImage backup;
	CvImage immagine;
	bool isCameraImage;
	int CurrentZoomRatio;
private:               
	Pose_Marker posemarker;

public:
    CImageData();                                   // default c'tor
    ~CImageData();                                  // d'tor - calls DeleteData()

	Pose_Marker getPoseMarker();
	void setPoseMarker(Pose_Marker);
	void setImage(CvImage);
	
	HBITMAP GetBitmap();
};
