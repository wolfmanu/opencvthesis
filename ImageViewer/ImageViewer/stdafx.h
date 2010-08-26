// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

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

#include "Network.h"
#include "FTPSender.h"
#include "CartesianLib.h" //Matrix manipulation
#include "Calibration.h"
#include "CriticalSection.h"
#include "Guard.h"
#include "Utils.h"
#include "ImageData.h"
#include "CameraOptions.h"
#include "CameraControlDialog.h"

#include "Pose_Marker.h"
#include "cameraInfo.h"

#include "ErrorDef.h"

#endif