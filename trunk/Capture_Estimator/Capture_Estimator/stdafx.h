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

#include "targetver.h"

#include <time.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <conio.h>
#include <string>
#include <queue>
#include <set>

#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
#include <windows.h>

#include <cv.h>
#include <cvaux.h>
#include <highgui.h>
#include <ffopencv.h>

#include "CartesianLib.h" //Matrix manipulation
#include "Calibration.h"
#include "Pose_Marker.h"
#include "Network.h"
#include "FTPSender.h"


#include "CriticalSection.h"
#include "Guard.h"

#include "squares.h"
#include "Square_Area.h"

// TODO: reference additional headers your program requires here
