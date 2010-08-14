// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define N_PATTERN 2
#define MARKER_SIZE 164.6//99.6//164.6//dimensione totale del pattern con bordo
#define INITIAL_THRESHOLD 127
#define MARKER_BORDER 0.250f
#define RAD2DEG	(57.29577951)
#define MARKER_ID -1	//-1 to search for all
#define MAX_NUM_THRESH 5
#define CAM_H 240//480
#define CAM_W 320//704

#define FILEFORMAT "All\0*.bmp;*.BMP;*.dib;*.DIB;*.jpeg;*.jpg;*.JPEG;*.JPG;*.png;*.PNG;*.PBM;*.pbm;*.pgm;*.PGM;*.ppm;*.PPM;*.sr;*.SR;*.ras;*.RAS;*.tif;*.TIF;*.TIFF;*.tiff\0BMP - Windows Bitmaps\0*.bmp;*.BMP;*.dib;*.DIB\0JPG - JPG/JPEG Format\0*.jpeg;*.jpg;*.JPEG;*.JPG\0PNG - Portable Network Graphics\0*.png;*.PNG\0PBM - Portable Bitmap\0*.PBM;*.pbm\0PGM - Portable Graymap\0*.pgm;*.PGM\0PPM - Portable Pixelmap\0*.ppm;*.PPM\0SR - Sun Raster\0*.sr;*.SR;*.ras;*.RAS\0TIF - Tagged Image File Format\0*.tif;*.TIF;*.TIFF;*.tiff\0\0"

#include "targetver.h"

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#ifndef _AFXDLL
#define _AFXDLL
#endif
#include <afxwin.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <Objbase.h>
#include <shlobj.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <queue>
#include <tchar.h>


// TODO: reference additional headers your program requires here
#include <cv.h>
#include <cvaux.h>
#include <highgui.h>
#include <ffopencv.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <ws2tcpip.h>
#include <atlbase.h> //for lpwstr to char* conversion W2A
#include <sstream>


#include "FTPSender.h"
#include "CartesianLib.h" //Matrix manipulation
#include "Calibration.h"
#include "CriticalSection.h"
#include "Guard.h"
#include "Utils.h"
#include "ImageData.h"
#include "CameraOptions.h"
#include "Network.h"
#include "Pose_Marker.h"
#include "cameraInfo.h"



#endif