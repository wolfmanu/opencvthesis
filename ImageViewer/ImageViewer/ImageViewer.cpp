#include "stdafx.h"

#include "jpegFunctions.h"
#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>
#include "ImageViewer.h"

using namespace std;
using namespace cv;

string calibfile = DEF_CALIB_FILE;
long fileNumberToSave=0;
Network cameraControlNet;
HWND controlh;

int cam_w = DEF_CAM_W;
int cam_h = DEF_CAM_H;

class Calibrator{

	string inputImgPath;
	string inputImgName;
	string AICONCalibName;
	string ARTKPCalibName;

	string AICONcalib, ARTKPcalib;

	int threshold[MAX_NUM_THRESH];
	int threshCounter;
	int countNumberThresh;
	int markerIdfound;
	int numMrkFound;

	VECT6 Xestimation;
	TRANSFMATRIX Testimation;
	
	ARToolKitPlus::TrackerSingleMarker *tracker ;
	ARToolKitPlus::TrackerSingleMarker *trackerLite;
	ARToolKitPlus::ARMarkerInfo *MI ;

public:

Calibrator::Calibrator(string ARTKPCalibName_)
{
	ARTKPCalibName=ARTKPCalibName_;

	threshold[0] = 64;
	threshold[1] = 96;
	threshold[2] = 127;
	threshold[3] = 159;
	threshold[4] = 191;
	threshCounter = 0;
	countNumberThresh = 0;
	numMrkFound = -1;

	ARTKPcalib = inputImgPath + ARTKPCalibName;

	tracker = new ARToolKitPlus::TrackerSingleMarkerImpl<18,18,72, 1, N_PATTERN>(cam_w, cam_h);
	trackerLite = new ARToolKitPlus::TrackerSingleMarkerImpl<6,6,6, 1, N_PATTERN>(cam_w, cam_h);
	MI = new ARToolKitPlus::ARMarkerInfo[N_PATTERN];

	tracker->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_LUM);
	trackerLite->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_LUM);

	return;
}



Calibrator::~Calibrator(void)
{	
	tracker->cleanup();
	trackerLite->cleanup();
	delete tracker;
	delete trackerLite;
	delete MI;	
}

int Calibrator::Initialize()
{
	if(!tracker->init(ARTKPcalib.c_str(), 1.0f, 2500.0f))
	{
		ofstream tempfile;
		tempfile.open ("temp.cal");
		tempfile << "ARToolKitPlus_CamCal_Rev02"<<endl;
		tempfile << "704 480 380.45102 246.24552 846.63525 765.88469 -0.23620   0.36864   -0.00032   0.00024  0.00000 0.0 5"<<endl;
		tempfile.close();
		tracker->init("temp.cal", 1.0f, 2500.0f);
		return -1;
	}

	if(!trackerLite->init(ARTKPcalib.c_str(), 1.0f, 2500.0f))
	{
		ofstream tempfile;
		tempfile.open ("temp.cal");
		tempfile << "ARToolKitPlus_CamCal_Rev02"<<endl;
		tempfile << "704 480 380.45102 246.24552 846.63525 765.88469 -0.23620   0.36864   -0.00032   0.00024  0.00000 0.0 5"<<endl;
		tempfile.close();
		trackerLite->init("temp.cal", 1.0f, 2500.0f);
		return -1;
	}

	tracker->setPatternWidth(MARKER_SIZE);
	tracker->activateVignettingCompensation(true);	  
    tracker->setBorderWidth(MARKER_BORDER);
    tracker->setThreshold(INITIAL_THRESHOLD);
	tracker->setUndistortionMode(ARToolKitPlus::UNDIST_STD);//UNDIST_STD
	tracker->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);
    tracker->setMarkerMode(ARToolKitPlus::MARKER_ID_SIMPLE);
	tracker->setImageProcessingMode(ARToolKitPlus::IMAGE_FULL_RES);

	trackerLite->setPatternWidth(MARKER_SIZE);
	trackerLite->activateVignettingCompensation(true);	  
    trackerLite->setBorderWidth(MARKER_BORDER);
	trackerLite->setThreshold(INITIAL_THRESHOLD);
    trackerLite->setUndistortionMode(ARToolKitPlus::UNDIST_STD);//
	trackerLite->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_ORIGINAL_CONT);
    trackerLite->setMarkerMode(ARToolKitPlus::MARKER_ID_SIMPLE);
	trackerLite->setImageProcessingMode(ARToolKitPlus::IMAGE_HALF_RES);

	return 1;
}


Pose_Marker* Calibrator::FindPattern(Mat calcImg)
{
	Pose_Marker *pose = 0;
	bool stop=false;
	bool usedSq=false;
	
	while(!stop)
	{		
		/*imshow("p",calcImg);
		cvWaitKey();*/
		markerIdfound = trackerLite->calc((uchar*)calcImg.data, MARKER_ID, false, &MI, &numMrkFound);

		if(markerIdfound != -1 )
		{
			/////////////////////////////////THRESHOLDING/////////////////////////////////////////
			trackerLite->activateAutoThreshold(false);
			tracker->setThreshold(trackerLite->getThreshold());
			/////////////////////////////////THRESHOLDING/////////////////////////////////////////
			
		}

		//The estimator searches only for a specific marker (whose id is specified by the argument markerID)
		//or for any marker if markerID is equal to -1
		
		markerIdfound = tracker->calc((uchar*)calcImg.data, MARKER_ID, true, &MI);

		if (markerIdfound != -1)
		{
			WriteTMatrixFromFloat((float*)tracker->getModelViewMatrix(), &Testimation);

			Tmatrix2Vect(Testimation, &Xestimation);
			
			pose = new Pose_Marker(Xestimation.a[0]*RAD2DEG, Xestimation.a[1]*RAD2DEG, Xestimation.a[2]*RAD2DEG, 
							Xestimation.l.x/10.0, Xestimation.l.y/10.0, Xestimation.l.z/10.0,
							markerIdfound);
			stop = true;	
			Sleep(0);
		}
		else
		{
			///////////////////////////////THRESHOLDING/////////////////////////////////////////
			if(threshCounter >= MAX_NUM_THRESH)
			{
				countNumberThresh++;
				if(countNumberThresh >= 5)
						stop = true;
				
				trackerLite->activateAutoThreshold(true);
				tracker->setThreshold(trackerLite->getThreshold());
			}
			else
			{
				trackerLite->activateAutoThreshold(false);
				trackerLite->setThreshold(threshold[threshCounter]);
				tracker->activateAutoThreshold(false);
				tracker->setThreshold(trackerLite->getThreshold());
			}
			threshCounter++;
			///////////////////////////////THRESHOLDING/////////////////////////////////////////
		}
	}
	return pose; 
}
		

void Calibrator::WriteTMatrixFromFloat(float* matr, TRANSFMATRIX *Tmx)
{
	Tmx->mat[0] = (GREAL)(matr[0]);
	Tmx->mat[1] = (GREAL)(matr[1]);
	Tmx->mat[2] = (GREAL)(matr[2]);

	Tmx->mat[3] = (GREAL)(matr[4]);
	Tmx->mat[4] = (GREAL)(matr[5]);
	Tmx->mat[5] = (GREAL)(matr[6]);

	Tmx->mat[6] = (GREAL)(matr[8]);
	Tmx->mat[7] = (GREAL)(matr[9]);
	Tmx->mat[8] = (GREAL)(matr[10]);

	Tmx->mat[9] = (GREAL)(matr[12]);
	Tmx->mat[10] = (GREAL)(matr[13]);
	Tmx->mat[11] = (GREAL)(matr[14]);

	return;
}

void Calibrator::FindMaskCorners(ARToolKitPlus::ARMarkerInfo *MI, CvPoint* pt1, CvPoint* pt2)
{
	CvPoint P[4];
	int i;
	char delta = 25;

	P[0].x = (int)(MI->vertex[0][0]);
	P[0].y = (int)(MI->vertex[0][1]);
	P[1].x = (int)(MI->vertex[1][0]);
	P[1].y = (int)(MI->vertex[1][1]);
	P[2].x = (int)(MI->vertex[2][0]);
	P[2].y = (int)(MI->vertex[2][1]);
	P[3].x = (int)(MI->vertex[3][0]);
	P[3].y = (int)(MI->vertex[3][1]);

	pt1->x = cam_w;
	pt1->y = cam_h;
	pt2->x = 0;
	pt2->y = 0;


	for (i=0; i<4; i++)
	{
		if (P[i].x < pt1->x)	pt1->x = P[i].x;
		if (P[i].y < pt1->y)	pt1->y = P[i].y;
		if (P[i].x > pt2->x)	pt2->x = P[i].x;
		if (P[i].y > pt2->y)	pt2->y = P[i].y;
	}

	delta = (int)(0.3 * (pt2->x - pt1->x));

	pt1->x -=  delta;
	pt1->y -=  delta;
	pt2->x +=  delta;
	pt2->y +=  delta;

	return;
}

};

/************************************************************************************/
/************************************************************************************/
/******************************    Image Viewer   ***********************************/
/************************************************************************************/
/************************************************************************************/


// ImageViewer.cpp : Defines the entry point for the application.
//

#define __CAPTTHREAD__

using namespace Gdiplus;
using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

CImageData imagedata;
CriticalSection OptionsCS;
CameraOptions cameraOptions;
HWND hWndGlob;
bool toStop=false;
bool capturing=false;
CriticalSection fileNumbCS;
CriticalSection toStopCS;
LONG fileNumber=0;
queue<Mat> imageQueue;
CriticalSection imageQueueCS;
CriticalSection cameraNetCS;
Calibrator *calibrator;
int elaboratedFrame=0;
FTPSender *ftps;
int errstate=0;
double ZoomRatios[] = { 0.05, 0.1, 0.25, 0.5, 1, 2, 3, 4, 5};
//int BlurFilterType=-1;
double ThresholdFilterValue = -1; //means not active


// These variables are required for horizontal scrolling. 
static int xMinScroll;       // minimum horizontal scroll value 
static int xCurrentScroll;   // current horizontal scroll value 
static int xMaxScroll;       // maximum horizontal scroll value 
// These variables are required for vertical scrolling. 
static int yMinScroll;       // minimum vertical scroll value 
static int yCurrentScroll;   // current vertical scroll value 
static int yMaxScroll;       // maximum vertical scroll value 

static BOOL fScroll;         // TRUE if scrolling occurred 
static BOOL fSize;           // TRUE if fBlt & WM_SIZE 
static BOOL fBlt;            // TRUE if BitBlt occurred 

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Options(HWND , UINT , WPARAM , LPARAM );
INT_PTR CALLBACK	CameraControl(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	Threshold(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	Blur(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	CannyEdge(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	LinesDetection(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
//INT_PTR CALLBACK	LinesDetectionP(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI		ElaborateImage( LPVOID lpParam );

string getFileNameToLoad();
void SendViaFTP(Pose_Marker* pose);
void print(Graphics *gr);
string getUniqueFileName();
int CalibrateCamera();
int SaveFile(Mat image, Pose_Marker *pose);
void OnZoomPiu(void);
void OnZoomMeno(void);
void resetScrollBar(void);
void OnSave();
DWORD WINAPI CaptureImage( LPVOID lpParam );


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	
	GdiplusStartupInput gdiplusStartupInput; 
    ULONG_PTR gdiplusToken; 
	
	GdiplusStartup( &gdiplusToken, &gdiplusStartupInput, NULL); 

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGEVIEWER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IMAGEVIEWER));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	GdiplusShutdown(gdiplusToken);
	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IMAGEVIEWER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_IMAGEVIEWER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{


   hInst = hInstance; // Store instance handle in our global variable
   
   hWndGlob = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW |WS_HSCROLL |
                                    WS_VSCROLL ,
      CW_USEDEFAULT, 0, 800, 600, NULL, NULL, hInstance, NULL);

   if (!hWndGlob)
   {
      return FALSE;
   }
   ShowWindow(hWndGlob, nCmdShow);
   UpdateWindow(hWndGlob);

   return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//

DWORD WINAPI provadialogbox( LPVOID lpParam )
{
	DialogBox( hInst, MAKEINTRESOURCE(IDD_CAMERACTRL), 0, CameraControl);
	return 1;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	SCROLLINFO si; 

	switch (message)
	{
	case WM_CREATE:
		resetScrollBar();
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_OPEN:
			if(capturing)
			{
				Guard g(toStopCS);
				toStop=true;
			}
			OPENFILENAME ofn;       // common dialog box structure
			_TCHAR szFile[MAX_PATH];       // buffer for file name
			HANDLE hf;              // file handle

			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);

			ofn.lpstrFilter = _TEXT(FILEFORMAT);
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			// Display the Open dialog box. 

			if (GetOpenFileName(&ofn)==TRUE) 
			{
				USES_CONVERSION;
				Mat Image=imread(W2A(ofn.lpstrFile));
				
				if(!Image.empty() /*Image.is_valid()*/)
				{
					/*cvShowImage("e",Image);
					cvWaitKey();
					cvDestroyAllWindows();*/
					
					imagedata.setImage(Image);
					imagedata.isCameraImage=false;
				}
			}
			InvalidateRect(hWndGlob,NULL,TRUE);
			break;
		case ID_CAPTURE_START:
			if(!capturing)
			{
				//DialogBox( hInst, MAKEINTRESOURCE(IDD_CAMERACTRL), hWnd, CameraControl);
				if(cameraOptions.sendFtp)
				{
					ftps=new FTPSender(cameraOptions.ftpServer, cameraOptions.ftpPort );
					int err=ftps->FTPconnect(cameraOptions.ftpUser, cameraOptions.ftpPass);
					if(err==-1)
						errstate=FTP_CONNECTION_ERR;
					else if(err==-2)
						errstate=FTP_USER_ERR;
					else if(err==-3)
						errstate=FTP_PASSWOR_ERR;
				}
				calibrator = new Calibrator(calibfile);
				int ret=CalibrateCamera();
				if(ret==-1)//da specificare meglio tipo di errore
				{
					MessageBox(
								hWndGlob,
								(LPCWSTR)L"Error Initializing Tracker",
								(LPCWSTR)L"Warning",
								MB_ICONWARNING | MB_OK 
							);
					errstate=CALIB_FILE_READ_ERR;
					break;
				}
				
				/*test connettività camera */
			
				if(Network::testConnectivity(cameraOptions.cameraIp)==0)
				{
					MessageBox(
								hWndGlob,
								(LPCWSTR)L"Camera not reachable.",
								(LPCWSTR)L"Warning",
								MB_ICONWARNING | MB_OK 
							);
					errstate=CAM_NOT_CONNECTED;
					break;
				}
				   
				toStop=false;
				capturing=true;
			 
				QueueUserWorkItem(CaptureImage, 0,0);
				DWORD   dwThread;
				HANDLE  hThread; 

				hThread = CreateThread( 
					NULL,                   // default security attributes
					0,                      // use default stack size  
					provadialogbox,       // thread function name
					NULL,          // argument to thread function 
					0,                      // use default creation flags 
					&dwThread);   // returns the thread identifier 
				


			}
			break;
		case ID_CAPTURE_STOP:
			if(capturing)
			{
				Guard g(toStopCS);
				toStop=true;
				capturing=false;
				EndDialog(controlh,0);
			}
			break;
		case ID_VIEW_POSE:
			{
				Guard g(OptionsCS);
				cameraOptions.viewPose=!cameraOptions.viewPose;
				InvalidateRect(hWndGlob,NULL,TRUE);
			}
			break;
		case ID_CAPTURE_OPTION:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CAPTUREOPTION), hWnd, Options);
			break;
		case ID_VIEW_GRID:
			{
				Guard g(OptionsCS);
				//cameraOptions.viewgri=!cameraOptions.viewPose;
				InvalidateRect(hWndGlob,NULL,TRUE);
			}
			break;
		case ID_VIEW_MARKERCENTER:
			{
				Guard g(OptionsCS);
				cameraOptions.viewCenter=!cameraOptions.viewCenter;
				InvalidateRect(hWndGlob,NULL,TRUE);
			}
			break;

		case ID_VIEW_ZOOMPIU:
			if(!imagedata.immagine.empty())
				OnZoomPiu();
			break;
		case ID_VIEW_ZOOMMENO:
			if(!imagedata.immagine.empty())
				OnZoomMeno();
			break;
		case ID_VIEW_RESETZOOM:
			imagedata.CurrentZoomRatio=4;
			break;

		case ID_FILE_SAVE:
			OnSave();
			break;

		case ID_RESET_ALL:
			if(!imagedata.immagine.empty())
				imagedata.resetAllFilter();
			InvalidateRect(hWndGlob,NULL,TRUE);
			break;

		case ID_RESET_THRESHOLD:
			if(!imagedata.immagine.empty())
				imagedata.resetAllFilter();
			InvalidateRect(hWndGlob,NULL,TRUE);
			break;

		case ID_RESET_BLUR:
			if(!imagedata.immagine.empty())
				imagedata.resetAllFilter();
			InvalidateRect(hWndGlob,NULL,TRUE);
			break;

		case ID_DETECTION_EDGEDETECTION:
			if(!imagedata.immagine.empty())
				DialogBox(hInst, MAKEINTRESOURCE(IDD_CANNYEDGE), hWnd, CannyEdge);
			break;

		case ID_DETECTION_LINESDETECTION:
			if(!imagedata.immagine.empty())
				DialogBox(hInst, MAKEINTRESOURCE(IDD_LINESDET), hWnd, LinesDetection);
			break;

		case ID_DETECTION_LINESDETECTION2:
			{
			imagedata.setBackupHoughDetection();
			
			vector<Vec4i> lines;
			HoughLinesP( imagedata.backupFilter, lines, 1, CV_PI/180, 80, 30, 10 );
			for( size_t i = 0; i < lines.size(); i++ )
			{
				line( imagedata.immagine, cv::Point(lines[i][0], lines[i][1]),
					cv::Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 1, 8 );
			}

			InvalidateRect(hWndGlob,NULL,TRUE);
			break;
			}

		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_HSCROLL: 
    { 
        int xDelta;     // xDelta = new_pos - current_pos  
        int xNewPos;    // new position 
        int yDelta = 0; 

        switch (LOWORD(wParam)) 
        { 
            // User clicked the scroll bar shaft left of the scroll box. 
            case SB_PAGEUP: 
                xNewPos = xCurrentScroll - 50; 
                break; 

            // User clicked the scroll bar shaft right of the scroll box. 
            case SB_PAGEDOWN: 
                xNewPos = xCurrentScroll + 50; 
                break; 

            // User clicked the left arrow. 
            case SB_LINEUP: 
                xNewPos = xCurrentScroll - 5; 
                break; 

            // User clicked the right arrow. 
            case SB_LINEDOWN: 
                xNewPos = xCurrentScroll + 5; 
                break; 

            // User dragged the scroll box. 
            case SB_THUMBPOSITION: 
                xNewPos = HIWORD(wParam); 
                break; 

            default: 
                xNewPos = xCurrentScroll; 
        } 

        // New position must be between 0 and the screen width. 
        xNewPos = max(0, xNewPos); 
        xNewPos = min(xMaxScroll, xNewPos); 

        // If the current position does not change, do not scroll.
        if (xNewPos == xCurrentScroll) 
            break; 

        // Set the scroll flag to TRUE. 
        fScroll = TRUE; 

        // Determine the amount scrolled (in pixels). 
        xDelta = xNewPos - xCurrentScroll; 

        // Reset the current scroll position. 
        xCurrentScroll = xNewPos; 

        // Scroll the window. (The system repaints most of the 
        // client area when ScrollWindowEx is called; however, it is 
        // necessary to call UpdateWindow in order to repaint the 
        // rectangle of pixels that were invalidated.) 
        ScrollWindowEx(hWnd, -xDelta, -yDelta, (CONST RECT *) NULL, 
            (CONST RECT *) NULL, (HRGN) NULL, (PRECT) NULL, 
            SW_INVALIDATE); 
        UpdateWindow(hWnd); 


        // Reset the scroll bar. 
        si.cbSize = sizeof(si); 
        si.fMask  = SIF_POS; 
        si.nPos   = xCurrentScroll; 
        SetScrollInfo(hWnd, SB_HORZ, &si, TRUE); 
		
        break; 
    } 
	case WM_VSCROLL: 
    { 
        int xDelta = 0; 
        int yDelta;     // yDelta = new_pos - current_pos 
        int yNewPos;    // new position 

        switch (LOWORD(wParam)) 
        { 
            // User clicked the scroll bar shaft above the scroll box. 
            case SB_PAGEUP: 
                yNewPos = yCurrentScroll - 50; 
                break; 

            // User clicked the scroll bar shaft below the scroll box. 
            case SB_PAGEDOWN: 
                yNewPos = yCurrentScroll + 50; 
                break; 

            // User clicked the top arrow. 
            case SB_LINEUP: 
                yNewPos = yCurrentScroll - 5; 
                break; 

            // User clicked the bottom arrow. 
            case SB_LINEDOWN: 
                yNewPos = yCurrentScroll + 5; 
                break; 

            // User dragged the scroll box. 
            case SB_THUMBPOSITION: 
                yNewPos = HIWORD(wParam); 
                break; 

            default: 
                yNewPos = yCurrentScroll; 
        } 

        // New position must be between 0 and the screen height. 
        yNewPos = max(0, yNewPos); 
        yNewPos = min(yMaxScroll, yNewPos); 

        // If the current position does not change, do not scroll.
        if (yNewPos == yCurrentScroll) 
            break; 

        // Set the scroll flag to TRUE. 
        fScroll = TRUE; 

        // Determine the amount scrolled (in pixels). 
        yDelta = yNewPos - yCurrentScroll; 

        // Reset the current scroll position. 
        yCurrentScroll = yNewPos; 

        // Scroll the window. (The system repaints most of the 
        // client area when ScrollWindowEx is called; however, it is 
        // necessary to call UpdateWindow in order to repaint the 
        // rectangle of pixels that were invalidated.) 
        ScrollWindowEx(hWnd, -xDelta, -yDelta, (CONST RECT *) NULL, 
            (CONST RECT *) NULL, (HRGN) NULL, (PRECT) NULL, 
            SW_INVALIDATE); 
        UpdateWindow(hWnd); 
		
        // Reset the scroll bar. 
        si.cbSize = sizeof(si); 
        si.fMask  = SIF_POS; 
        si.nPos   = yCurrentScroll; 
        SetScrollInfo(hWnd, SB_VERT, &si, TRUE); 

        break; 
        } 

	case WM_SIZE: 
        { 
            int xNewSize; 
			int yNewSize;
			int imwidth=0, imheight=0;
			if(!imagedata.immagine.empty())
			{
				imwidth=imagedata.immagine.cols;
				imheight=imagedata.immagine.rows;
			}

			xNewSize = LOWORD(lParam); 
			yNewSize = HIWORD(lParam); 

			if (fBlt) 
				fSize = TRUE; 

			// The horizontal scrolling range is defined by 
			// (bitmap_width) - (client_width). The current horizontal 
			// scroll value remains within the horizontal scrolling range. 
			xMaxScroll = max(imwidth-xNewSize, 0); 
			xCurrentScroll = min(xCurrentScroll, xMaxScroll); 
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = xMinScroll; 
			si.nMax   = imwidth; 
			si.nPage  = xNewSize; 
			si.nPos   = xCurrentScroll; 
			SetScrollInfo(hWnd, SB_HORZ, &si, TRUE); 

			 // The vertical scrolling range is defined by 
			// (bitmap_height) - (client_height). The current vertical 
			// scroll value remains within the vertical scrolling range. 
			yMaxScroll = max(imheight - yNewSize, 0); 
			yCurrentScroll = min(yCurrentScroll, yMaxScroll); 
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = yMinScroll; 
			si.nMax   = imheight; 
			si.nPage  = yNewSize; 
			si.nPos   = yCurrentScroll; 
			SetScrollInfo(hWnd, SB_VERT, &si, TRUE); 
			
			break;
		}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		{
		Graphics g(hdc);
		print(&g);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		if(capturing)
		{
			Guard g(toStopCS);
			toStop=true;
			capturing=false;
		}	
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK Options(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	bool isChecked;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
		int code;
		code=cameraOptions.viewVideo ? BST_CHECKED : BST_UNCHECKED;
		Button_SetCheck(GetDlgItem(hDlg, IDC_DISPLAYVIDEO), code);
		code=cameraOptions.sendFtp ? BST_CHECKED : BST_UNCHECKED;
		Button_SetCheck(GetDlgItem(hDlg, IDC_USEFTP), code);
		code=cameraOptions.saveImg ? BST_CHECKED : BST_UNCHECKED;
		Button_SetCheck(GetDlgItem(hDlg, IDC_SAVEIMAGE), code);
		code=cameraOptions.elaborate ? BST_CHECKED : BST_UNCHECKED;
		Button_SetCheck(GetDlgItem(hDlg, IDC_ELABORATE), code);

		Edit_SetText(GetDlgItem(hDlg, IDC_EDITFTPPORT),cameraOptions.wftpPort.c_str());
		Edit_SetText(GetDlgItem(hDlg, IDC_EDITFTPSERVER),cameraOptions.wftpServer.c_str());
		Edit_SetText(GetDlgItem(hDlg, IDC_EDITUSER),cameraOptions.wftpUser.c_str());
		Edit_SetText(GetDlgItem(hDlg, IDC_EDITPWD),cameraOptions.wftpPass.c_str());
		Edit_SetText(GetDlgItem(hDlg, IDC_EDITFOLDER),cameraOptions.wsaveFold.c_str());
		Edit_SetText(GetDlgItem(hDlg, IDC_EDITNAME),cameraOptions.wsaveName.c_str());
		Edit_SetText(GetDlgItem(hDlg, IDC_EDITIP1),cameraOptions.wip1.c_str());
		Edit_SetText(GetDlgItem(hDlg, IDC_EDITIP2),cameraOptions.wip2.c_str());
		Edit_SetText(GetDlgItem(hDlg, IDC_EDITIP3),cameraOptions.wip3.c_str());
		Edit_SetText(GetDlgItem(hDlg, IDC_EDITIP4),cameraOptions.wip4.c_str());

		isChecked=(Button_GetCheck(GetDlgItem(hDlg,IDC_USEFTP))==BST_CHECKED);
		Edit_Enable(GetDlgItem(hDlg,IDC_STATICUSER),isChecked);
		Edit_Enable(GetDlgItem(hDlg,IDC_STATICPWD),isChecked);
		Edit_Enable(GetDlgItem(hDlg,IDC_STATICFTPPORT),isChecked);
		Edit_Enable(GetDlgItem(hDlg,IDC_STATICFTPSERVER),isChecked);
		Edit_Enable(GetDlgItem(hDlg,IDC_EDITUSER),isChecked);
		Edit_Enable(GetDlgItem(hDlg,IDC_EDITPWD),isChecked);
		Edit_Enable(GetDlgItem(hDlg,IDC_EDITFTPPORT),isChecked);
		Edit_Enable(GetDlgItem(hDlg,IDC_EDITFTPSERVER),isChecked);

		isChecked =(Button_GetCheck(GetDlgItem(hDlg,IDC_SAVEIMAGE))==BST_CHECKED);
		Edit_Enable(GetDlgItem(hDlg,IDC_STATICFOLDER),isChecked);
		Edit_Enable(GetDlgItem(hDlg,IDC_STATICNAME),isChecked);
		Edit_Enable(GetDlgItem(hDlg,IDC_EDITFOLDER),isChecked);
		Edit_Enable(GetDlgItem(hDlg,IDC_EDITNAME),isChecked);
		Button_Enable(GetDlgItem(hDlg,IDC_BUTTONSELECT),isChecked);
			
		return (INT_PTR)TRUE;
		}

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		_TCHAR szDir[MAX_PATH];
		switch (wmId)
		{
		case IDOK:
			_TCHAR buff[MAX_PATH],buff2[MAX_PATH],buff3[MAX_PATH],buff4[MAX_PATH];
			EndDialog(hDlg, LOWORD(wParam));
			{
				Guard g(OptionsCS);
				cameraOptions.viewVideo=(Button_GetCheck(GetDlgItem(hDlg,IDC_DISPLAYVIDEO))==BST_CHECKED);
				cameraOptions.sendFtp=(Button_GetCheck(GetDlgItem(hDlg,IDC_USEFTP))==BST_CHECKED);
				cameraOptions.saveImg=(Button_GetCheck(GetDlgItem(hDlg,IDC_SAVEIMAGE))==BST_CHECKED);
				cameraOptions.elaborate=(Button_GetCheck(GetDlgItem(hDlg,IDC_ELABORATE))==BST_CHECKED);
				if(cameraOptions.sendFtp)
				{
					Edit_GetText(GetDlgItem(hDlg, IDC_EDITFTPPORT),buff,MAX_PATH);
					cameraOptions.setPort(buff);
					Edit_GetText(GetDlgItem(hDlg, IDC_EDITFTPSERVER),buff,MAX_PATH);
					cameraOptions.setServer(buff);
					Edit_GetText(GetDlgItem(hDlg, IDC_EDITUSER),buff,MAX_PATH);
					cameraOptions.setUser(buff);
					Edit_GetText(GetDlgItem(hDlg, IDC_EDITPWD),buff,MAX_PATH);
					cameraOptions.setPass(buff);
				}
				if(cameraOptions.saveImg)
				{
					Edit_GetText(GetDlgItem(hDlg, IDC_EDITFOLDER),buff,MAX_PATH);
					cameraOptions.setFold(buff);
					Edit_GetText(GetDlgItem(hDlg, IDC_EDITNAME),buff,MAX_PATH);
					cameraOptions.setName(buff);
				}
				Edit_GetText(GetDlgItem(hDlg, IDC_EDITIP1),buff,MAX_PATH);
				Edit_GetText(GetDlgItem(hDlg, IDC_EDITIP2),buff2,MAX_PATH);
				Edit_GetText(GetDlgItem(hDlg, IDC_EDITIP3),buff3,MAX_PATH);
				Edit_GetText(GetDlgItem(hDlg, IDC_EDITIP4),buff4,MAX_PATH);
				cameraOptions.setCameraIp(buff, buff2, buff3, buff4);

			}
			return (INT_PTR)TRUE;

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;

		case IDC_BUTTONSELECT:
			//wstring wselectedDir;
			//wselectedDir=cameraOptions.saveFold;
			//_TCHAR wselectedDir[260];
			BROWSEINFO info; 
			info.hwndOwner = hDlg;
			info.pidlRoot = NULL;
			info.pszDisplayName = NULL;//wselectedDir;
			info.lpszTitle = _T("Choose the directory");
			info.lpfn = NULL;
			info.iImage = NULL;
			info.lParam = NULL;
			info.ulFlags = BIF_RETURNONLYFSDIRS;
			LPITEMIDLIST pidl;
			pidl= SHBrowseForFolder(&info);
			if (pidl)
				if (SHGetPathFromIDList(pidl, szDir))
					Edit_SetText(GetDlgItem(hDlg, IDC_EDITFOLDER),szDir);
			
			return (INT_PTR)TRUE;

		case IDC_USEFTP:
			isChecked=(Button_GetCheck(GetDlgItem(hDlg,IDC_USEFTP))==BST_CHECKED);
			Edit_Enable(GetDlgItem(hDlg,IDC_STATICUSER),isChecked);
			Edit_Enable(GetDlgItem(hDlg,IDC_STATICPWD),isChecked);
			Edit_Enable(GetDlgItem(hDlg,IDC_STATICFTPPORT),isChecked);
			Edit_Enable(GetDlgItem(hDlg,IDC_STATICFTPSERVER),isChecked);
			Edit_Enable(GetDlgItem(hDlg,IDC_EDITUSER),isChecked);
			Edit_Enable(GetDlgItem(hDlg,IDC_EDITPWD),isChecked);
			Edit_Enable(GetDlgItem(hDlg,IDC_EDITFTPPORT),isChecked);
			Edit_Enable(GetDlgItem(hDlg,IDC_EDITFTPSERVER),isChecked);
			return (INT_PTR)TRUE;

		case IDC_SAVEIMAGE:
			isChecked =(Button_GetCheck(GetDlgItem(hDlg,IDC_SAVEIMAGE))==BST_CHECKED);
			Edit_Enable(GetDlgItem(hDlg,IDC_STATICFOLDER),isChecked);
			Edit_Enable(GetDlgItem(hDlg,IDC_STATICNAME),isChecked);
			Edit_Enable(GetDlgItem(hDlg,IDC_EDITFOLDER),isChecked);
			Edit_Enable(GetDlgItem(hDlg,IDC_EDITNAME),isChecked);
			Button_Enable(GetDlgItem(hDlg,IDC_BUTTONSELECT),isChecked);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK CameraControl(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		controlh=hDlg;
		cameraControlNet.setNetwork(cameraOptions.cameraIp,"80");
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		
		switch (wmId)
		{
			//http://<servername>/cgi-bin/camctrl.cgi? [move=<value>][&focus=<value>]
			//[&iris=<value>][&speedpan=<value>][&speedtilt=<value>][&speedzoom=<value>]
			//[&speedapp=<value>][&auto=<value>][&zoom=<value>][&return=<return page>]
		case IDC_BUTTONUP:
			{
			char recvbuf[BUFLEN];
			std::string bu="GET /cgi-bin/camctrl.cgi?move=up HTTP/1.1\r\n";
						bu+="Host: ";
						bu+=cameraOptions.cameraIp;
						bu+="\r\n";
						bu+="Connection: Keep-Alive\r\n\r\n";
			{
				Guard g(cameraNetCS);

				cameraControlNet.NWConnect();
				cameraControlNet.NWsendData(bu.c_str(),bu.length());
				cameraControlNet.NWrcvData(recvbuf,BUFLEN);
				cameraControlNet.NWCloseConnection();
			}
			return (INT_PTR)TRUE;
			}
		case IDC_BUTTONDOWN:
			{
			char recvbuf[BUFLEN];
			std::string bu="GET /cgi-bin/camctrl.cgi?move=down HTTP/1.1\r\n";
						bu+="Host: ";
						bu+=cameraOptions.cameraIp;
						bu+="\r\n";
						bu+="Connection: Keep-Alive\r\n\r\n";
			{
				Guard g(cameraNetCS);

				cameraControlNet.NWConnect();
				cameraControlNet.NWsendData(bu.c_str(),bu.length());
				cameraControlNet.NWrcvData(recvbuf,BUFLEN);
				cameraControlNet.NWCloseConnection();
			}
			return (INT_PTR)TRUE;
			}
		case IDC_BUTTONLEFT:
			{
			char recvbuf[BUFLEN];
			std::string bu="GET /cgi-bin/camctrl.cgi?move=left HTTP/1.1\r\n";
						bu+="Host: ";
						bu+=cameraOptions.cameraIp;
						bu+="\r\n";
						bu+="Connection: Keep-Alive\r\n\r\n";
			{
				Guard g(cameraNetCS);

				cameraControlNet.NWConnect();
				cameraControlNet.NWsendData(bu.c_str(),bu.length());
				cameraControlNet.NWrcvData(recvbuf,BUFLEN);
				cameraControlNet.NWCloseConnection();
			}
			return (INT_PTR)TRUE;
			}
		case IDC_BUTTONRIGHT:
			{
			char recvbuf[BUFLEN];
			std::string bu="GET /cgi-bin/camctrl.cgi?move=right HTTP/1.1\r\n";
						bu+="Host: ";
						bu+=cameraOptions.cameraIp;
						bu+="\r\n";
						bu+="Connection: Keep-Alive\r\n\r\n";
			{
				Guard g(cameraNetCS);

				cameraControlNet.NWConnect();
				cameraControlNet.NWsendData(bu.c_str(),bu.length());
				cameraControlNet.NWrcvData(recvbuf,BUFLEN);
				cameraControlNet.NWCloseConnection();
			}
			return (INT_PTR)TRUE;
			}
		case IDC_BUTTONPAN:
			{
			char recvbuf[BUFLEN];
			std::string bu="GET /cgi-bin/camctrl.cgi?auto=pan HTTP/1.1\r\n";
						bu+="Host: ";
						bu+=cameraOptions.cameraIp;
						bu+="\r\n";
						bu+="Connection: Keep-Alive\r\n\r\n";
			{
				Guard g(cameraNetCS);

				cameraControlNet.NWConnect();
				cameraControlNet.NWsendData(bu.c_str(),bu.length());
				cameraControlNet.NWrcvData(recvbuf,BUFLEN);
				cameraControlNet.NWCloseConnection();
			}
			return (INT_PTR)TRUE;
			}
		case IDC_BUTTONSTOP:
			{
			char recvbuf[BUFLEN];
			std::string bu="GET /cgi-bin/camctrl.cgi?auto=stop HTTP/1.1\r\n";
						bu+="Host: ";
						bu+=cameraOptions.cameraIp;
						bu+="\r\n";
						bu+="Connection: Keep-Alive\r\n\r\n";
			{
				Guard g(cameraNetCS);

				cameraControlNet.NWConnect();
				cameraControlNet.NWsendData(bu.c_str(),bu.length());
				cameraControlNet.NWrcvData(recvbuf,BUFLEN);
				cameraControlNet.NWCloseConnection();
			}
			return (INT_PTR)TRUE;
			}
		case IDC_BUTTONHOME:
			{
			char recvbuf[BUFLEN];
			std::string bu="GET /cgi-bin/camctrl.cgi?move=home HTTP/1.1\r\n";
						bu+="Host: ";
						bu+=cameraOptions.cameraIp;
						bu+="\r\n";
						bu+="Connection: Keep-Alive\r\n\r\n";
			{
				Guard g(cameraNetCS);

				cameraControlNet.NWConnect();
				cameraControlNet.NWsendData(bu.c_str(),bu.length());
				cameraControlNet.NWrcvData(recvbuf,BUFLEN);
				cameraControlNet.NWCloseConnection();
			}
			return (INT_PTR)TRUE;
			}
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK	Blur(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		imagedata.setBackupFilter();
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		_TCHAR buff[MAX_PATH];
		int n;

		switch (wmId)
		{
		case IDCANCEL:
			imagedata.setBlurFilter(-1);
			InvalidateRect(hWndGlob,NULL,TRUE);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;

		case IDC_RADIO_BLUR:
			Edit_GetText(GetDlgItem(hDlg, IDC_EDITKSIZE), buff ,MAX_PATH);
			n=wcstod(buff, NULL);
			if(n==0)
				n=3;
			imagedata.testBlurFilter(IDC_RADIO_BLUR, n);
			InvalidateRect(hWndGlob,NULL,TRUE);
			return (INT_PTR)TRUE;

		case IDC_RADIO_GAUSSIANBLUR:
			Edit_GetText(GetDlgItem(hDlg, IDC_EDITKSIZE), buff ,MAX_PATH);
			n=wcstod(buff, NULL);
			if(n==0)
				n=3;
			if((n%2)!=1)		//deve essere dispari
				n++;
			imagedata.testBlurFilter(IDC_RADIO_GAUSSIANBLUR, n);
			InvalidateRect(hWndGlob,NULL,TRUE);
			return (INT_PTR)TRUE;

		case IDC_RADIO_MEDIANBLUR:
			Edit_GetText(GetDlgItem(hDlg, IDC_EDITKSIZE), buff ,MAX_PATH);
			n=wcstod(buff, NULL);
			if(n==0)
				n=3;
			imagedata.testBlurFilter(IDC_RADIO_MEDIANBLUR,n);
			InvalidateRect(hWndGlob,NULL,TRUE);
			return (INT_PTR)TRUE;


		case IDOK:		
			Edit_GetText(GetDlgItem(hDlg, IDC_EDITKSIZE), buff ,MAX_PATH);
			n=wcstod(buff, NULL);
			if(n==0)
				n=3;
			imagedata.setBlurFilter( n);
			InvalidateRect(hWndGlob,NULL,TRUE);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


INT_PTR CALLBACK LinesDetection(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	double rho, teta;
	DWORD dwPos;
	UNREFERENCED_PARAMETER(lParam);
	double thresh;

	_TCHAR buff[MAX_PATH];
	switch (message)
	{
	case WM_INITDIALOG:
		imagedata.setBackupHoughDetection();
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDOK:
		{
			HWND hwndTrack;
			hwndTrack=GetDlgItem(hDlg, IDC_SLIDER);
			dwPos = SendMessage(hwndTrack, TBM_GETPOS, 0, 0); //0 - 100
			thresh= dwPos*2.5;
			Edit_GetText(GetDlgItem(hDlg, IDC_EDIT_RHO),buff,MAX_PATH);
			rho=wcstod(buff, NULL);
			Edit_GetText(GetDlgItem(hDlg, IDC_EDIT_TETA),buff,MAX_PATH);
			teta=wcstod(buff, NULL);
			
			imagedata.setLineDetection(rho, teta, thresh);
			InvalidateRect(hWndGlob,NULL,TRUE);
			return (INT_PTR)TRUE;
		}

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			imagedata.setImage(imagedata.backup);
			InvalidateRect(hWndGlob,NULL,TRUE);
			return (INT_PTR)TRUE;


		case IDTEST:
		{
			HWND hwndTrack;
			hwndTrack=GetDlgItem(hDlg, IDC_SLIDER_THR);
			dwPos = SendMessage(hwndTrack, TBM_GETPOS, 0, 0); //0 - 100
			thresh= (dwPos*2.5);
			Edit_GetText(GetDlgItem(hDlg, IDC_EDIT_RHO),buff,MAX_PATH);
			rho=wcstod(buff, NULL);
			Edit_GetText(GetDlgItem(hDlg, IDC_EDIT_TETA),buff,MAX_PATH);
			teta=wcstod(buff, NULL);
			
			imagedata.testLineDetection(rho, teta, thresh);
			InvalidateRect(hWndGlob,NULL,TRUE);
			return (INT_PTR)TRUE;
		}
		break;
		}
	//case WM_HSCROLL :
	//	{
	//	HWND hwndTrack;
	//	hwndTrack=GetDlgItem(hDlg, IDC_SLIDER_THR);
	//	dwPos = SendMessage(hwndTrack, TBM_GETPOS, 0, 0); //0 - 100
	//	thresh= (dwPos*2.5);
	//	wstring ws;
	//	ws=thresh;
	//	Edit_SetText(GetDlgItem(hDlg, IDC_EDIT_THRESH),ws.c_str());
	//	return (INT_PTR)TRUE;
	//	}
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK Threshold(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	DWORD dwPos;
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
		imagedata.setBackupFilter();
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDOK:
		{
			HWND hwndTrack;
			hwndTrack=GetDlgItem(hDlg, IDC_SLIDER);
			dwPos = SendMessage(hwndTrack, TBM_GETPOS, 0, 0); //0 - 100
			ThresholdFilterValue=(255.0*dwPos)/100;
			imagedata.setThreshFilter(ThresholdFilterValue);
			EndDialog(hDlg, LOWORD(wParam));
			InvalidateRect(hWndGlob,NULL,TRUE);
			return (INT_PTR)TRUE;
		}

		case IDCANCEL:
			ThresholdFilterValue=-1;
			EndDialog(hDlg, LOWORD(wParam));
			imagedata.setThreshFilter(ThresholdFilterValue);
			InvalidateRect(hWndGlob,NULL,TRUE);
		}
		break;

	case WM_HSCROLL :
		{
		HWND hwndTrack;
		hwndTrack=GetDlgItem(hDlg, IDC_SLIDER);
		dwPos = SendMessage(hwndTrack, TBM_GETPOS, 0, 0); //0 - 100
		ThresholdFilterValue=(255.0*dwPos)/100;
		imagedata.testThreshFilter(ThresholdFilterValue);
		InvalidateRect(hWndGlob,NULL,TRUE);
		return (INT_PTR)TRUE;
		}
	}
	return (INT_PTR)FALSE;
}


INT_PTR CALLBACK CannyEdge(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	DWORD dwPos1, dwPos2;
	double thresh1, thresh2;
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
		imagedata.setBackupCannyDetection();
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDOK:
		{
			HWND hwndTrack1;
			hwndTrack1=GetDlgItem(hDlg, IDC_SLIDER1);
			dwPos1 = SendMessage(hwndTrack1, TBM_GETPOS, 0, 0); //0 - 100
			thresh1=(255.0*dwPos1)/100;
			HWND hwndTrack2;
			hwndTrack2=GetDlgItem(hDlg, IDC_SLIDER2);
			dwPos2 = SendMessage(hwndTrack2, TBM_GETPOS, 0, 0); //0 - 100
			thresh2=(255.0*dwPos2)/100;
			imagedata.setCannyFilter(thresh1, thresh2);
			InvalidateRect(hWndGlob,NULL,TRUE);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			imagedata.setImage(imagedata.backup);
			InvalidateRect(hWndGlob,NULL,TRUE);
		}
		break;

	case WM_HSCROLL :
		{
		HWND hwndTrack1;
		hwndTrack1=GetDlgItem(hDlg, IDC_SLIDER1);
		dwPos1 = SendMessage(hwndTrack1, TBM_GETPOS, 0, 0); //0 - 100
		thresh1=(255.0*dwPos1)/100;
		HWND hwndTrack2;
		hwndTrack2=GetDlgItem(hDlg, IDC_SLIDER2);
		dwPos2 = SendMessage(hwndTrack2, TBM_GETPOS, 0, 0); //0 - 100
		thresh2=(255.0*dwPos2)/100;
		imagedata.testCannyFilter(thresh1, thresh2);
		InvalidateRect(hWndGlob,NULL,TRUE);
		return (INT_PTR)TRUE;
		}
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void OnSave()
{
	if(capturing)
	{
		Guard g(toStopCS);
		toStop=true;
		capturing=false;
	}
	if(!imagedata.immagine.empty()/*is_valid()*/)
	{
		OPENFILENAME ofn;       // common dialog box structure
		_TCHAR szFile[MAX_PATH];       // buffer for file name
		HANDLE hf;              // file handle

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hWndGlob;
		ofn.lpstrFile = szFile;
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = _TEXT(FILEFORMAT);
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_CREATEPROMPT |OFN_NONETWORKBUTTON |OFN_OVERWRITEPROMPT;

		// Display the Open dialog box. 

		if (GetSaveFileName(&ofn)==TRUE) 
		{
			USES_CONVERSION; 
			//tosave.save(W2A(ofn.lpstrFile), "immagine salvata");
			imwrite(W2A(ofn.lpstrFile),imagedata.immagine);
		}
		InvalidateRect(hWndGlob,NULL,TRUE);
	}
}

void OnZoomPiu()
{
	Mat src = imagedata.backup; 
	double factor;
	if(imagedata.CurrentZoomRatio<_countof(ZoomRatios)-1)
	{
		imagedata.CurrentZoomRatio++;
		factor = ZoomRatios[imagedata.CurrentZoomRatio];
		imagedata.immagine.create( cvSize( src.cols*factor, src.rows*factor ), src.type() );
	}
	resize(src,imagedata.immagine,imagedata.immagine.size(),0,INTER_LANCZOS4);
	//src.convertTo(imagedata.immagine,imagedata.immagine.type());
	//cvResize(src, imagedata.immagine);
	InvalidateRect(hWndGlob,NULL,TRUE);
      
}

void OnZoomMeno()
{
	Mat src = imagedata.backup; 
	double factor;  
	if(imagedata.CurrentZoomRatio>1)
	{
		imagedata.CurrentZoomRatio--;
		factor = ZoomRatios[imagedata.CurrentZoomRatio];
		imagedata.immagine.create( cvSize( src.cols*factor, src.rows*factor ), src.type() );
	}
    
	resize(src,imagedata.immagine,imagedata.immagine.size(),0,INTER_LANCZOS4);
	//src.convertTo(imagedata.immagine,imagedata.immagine.type());
	//cvResize(src, imagedata.immagine);
	InvalidateRect(hWndGlob,NULL,TRUE);
      
}

void print(Graphics *gr)
{
	RectF winRt;
	int x,y, poseyoffset=0, posexoffset=0;
	double alpha=23;
	double b_tan_alpha, xc, yc;
	double xcentro=380.45102, ycentro=246.24552; //ATTENZIONE: DA CAMBIARE CON I DATI DEL FILE .CAL
	wstring pos;
	Pose_Marker pm=imagedata.getPoseMarker();;
	FontFamily fontFamily(L"Arial");
	Font font(&fontFamily, (REAL)8, FontStyleRegular, UnitPoint);
	HPALETTE hPalette = (HPALETTE)GetCurrentObject((HDC)hWndGlob, OBJ_PAL);
	gr->GetVisibleClipBounds(&winRt);
	SolidBrush whiteBrush(Color(255, 255, 255, 255));
	SolidBrush redBrush(Color(255, 255, 0, 0));
	SolidBrush solidBrush(Color(255, 0, 0, 0));
	SolidBrush *brush=&solidBrush;;
	gr->FillRectangle(&whiteBrush,winRt);

	if(!imagedata.immagine.empty() &&(!imagedata.isCameraImage || cameraOptions.viewVideo))
	{
		if(imagedata.isCameraImage && cameraOptions.viewPose)
		{
			RectF strRt;
		
			pos=pm.toLine();
			gr->MeasureString(pos.c_str(),pos.length(),
							&font,PointF(0,0),
							&strRt);
			poseyoffset=strRt.Height;
			posexoffset=strRt.Width;
			if(!cameraOptions.elaborate)
				brush=&redBrush;
		}
		bool sz=false;
		Bitmap bmp(imagedata.GetBitmap(),hPalette);
		if(winRt.Height>imagedata.immagine.rows+poseyoffset)
			y=(winRt.GetTop()+winRt.GetBottom()-(imagedata.immagine.rows+poseyoffset))/2;
		else
		{
			y=winRt.GetTop()- yCurrentScroll;
			sz=true;
		}
		if(winRt.Width>imagedata.immagine.cols)
			x=(winRt.GetLeft()+winRt.GetRight()-imagedata.immagine.cols)/2;
		else
		{
			x=winRt.GetLeft()- xCurrentScroll;
			sz=true;
		}

		if(cameraOptions.viewCenter)
		{
			b_tan_alpha= 352/(pm.distZ*tan(alpha*PI/180));
			xc=xcentro+(b_tan_alpha*pm.distX);
			yc=ycentro+(b_tan_alpha*pm.distY);
		}
		
		gr->DrawImage(&bmp,x,y+poseyoffset);
		if(imagedata.isCameraImage && cameraOptions.viewPose)
			gr->DrawString(pos.c_str(), pos.length(), &font, 
					PointF(x-(posexoffset-imagedata.immagine.cols)/2, y), brush);
		if(cameraOptions.viewCenter)
		{
			Pen p(&redBrush,2);
			gr->DrawLine(&p,(REAL)(x+xc), (REAL)(y+poseyoffset+yc-5),(REAL)(x+xc),(REAL)(y+poseyoffset+yc+5));
			gr->DrawLine(&p,(REAL)(x+xc-5), (REAL)(y+poseyoffset+yc),(REAL)(x+xc+5),(REAL)(y+poseyoffset+yc));
		}
		
		SendMessage(hWndGlob,WM_SIZE,SIZE_RESTORED,MAKELPARAM(winRt.Width,winRt.Height));

	}
	
	
}


DWORD WINAPI CaptureImage( LPVOID lpParam )
{
	Network cameraNet;
	std::string cameraIp = DEF_CAMERA_IP;
	int headerLen = HEADER_LEN;

	char recvbuf[BUFLEN];
	int iResult,o;
	
	std::string bu="GET /cgi-bin/video.jpg HTTP/1.1\r\n";
				bu+="Host: ";
				bu+=cameraOptions.cameraIp;
				bu+="\r\n";
				bu+="Connection: Keep-Alive\r\n\r\n";

	cameraNet.setNetwork(cameraOptions.cameraIp,"80");

	unsigned char* bufJpeg;
	std::string comment="";
	IplImage* auximg;
	//Mat imgJ;
	//TODO svuotare l'immagine (imagedata), rimepire con colore di sfondo
	while(!toStop)
	{
		if(cameraNet.NWConnect()==-1)
			errstate=CAM_NOT_CONNECTED;
		else
		{
			cameraNet.NWsendData(bu.c_str(),bu.length());
			iResult = cameraNet.NWrcvData(recvbuf,BUFLEN); //header
		
			comment="";
			
			bufJpeg=(unsigned char*)malloc((iResult-headerLen)*sizeof(unsigned char));

			if(iResult>headerLen)
				memcpy(bufJpeg,&(recvbuf[headerLen]),(iResult-headerLen));
				
			o=iResult-headerLen;
			while(iResult > 0)
			{
				iResult = cameraNet.NWrcvData(recvbuf, BUFLEN);	
				bufJpeg=(unsigned char*)realloc(bufJpeg,(o+iResult)*sizeof(unsigned char));
				memcpy(&(bufJpeg[o]), recvbuf,iResult);
				o+=iResult;
			}
			auximg=readJpegMem (bufJpeg,o,comment);
			
			Mat imgJ(auximg);
			
			free(bufJpeg);
			cameraNet.NWCloseConnection();
			
			{
				Guard g(toStopCS);
				if(!toStop)
				{
					imagedata.setImage(imgJ);
					imagedata.isCameraImage=true;
					{
						Guard g(OptionsCS);
						if(cameraOptions.viewVideo)
							InvalidateRect(hWndGlob,NULL,TRUE);
					}	
					{
						Guard g(OptionsCS);
						if(cameraOptions.elaborate)
						{
							Guard p(imageQueueCS);
							imageQueue.push(imgJ);
						}
					}
				
					{
						Guard g(OptionsCS);
						if(cameraOptions.elaborate)
						{
							if(elaboratedFrame==0){
								DWORD   dwThread;
								HANDLE  hThread; 

								hThread = CreateThread( 
									NULL,                   // default security attributes
									0,                      // use default stack size  
									ElaborateImage,       // thread function name
									NULL,          // argument to thread function 
									0,                      // use default creation flags 
									&dwThread);   // returns the thread identifier 


								//QueueUserWorkItem(ElaborateImage, 0,0);
							}
							elaboratedFrame++;
						}
					}
				}
			}
			if(auximg)
				cvReleaseImage(&auximg);
		}
	}
	return 0;
}

int SaveFile(Mat image, Pose_Marker *pose)
{
	string comment;
	string options="quality=90";
	string filename=getUniqueFileName();
	
	comment=pose->toString();
	IplImage iplim=image;
	CvImage* ccc=new CvImage(&iplim);
	writeJpegFile ((char*)(filename.c_str()), *ccc, options, comment);
	cout<<"Saved image "<<filename<<endl;
	
	return 1;
}

string getUniqueFileName()
{
	stringstream ss;
	{
	Guard l(fileNumbCS);
	ss<<cameraOptions.saveFold<<"//"<<cameraOptions.saveName<<fileNumberToSave<<DEF_EXTENSION;
	fileNumberToSave++;
	}
	
	return ss.str();
}

DWORD WINAPI ElaborateImage( LPVOID lpParam )
{
	int count=4,xcount=2, ycount=2;
	
	int recognized=0, n=1;
	
	Mat image(cvSize(cam_w,cam_h), IPL_DEPTH_8U, 1);
	Pose_Marker *pose, pose_old;
	
	Mat inImage;
	
	int frames=0;
	//endTask=false;
	
	while(true)
	{
		{
			//Guard p(imageQueueCS);
			inImage=imagedata.immagine;/**/
			/*if(!imageQueue.empty())
			{
				inImage=imageQueue.front();
				imageQueue.pop();
			}
			else */if (toStop)
			{
				//toStop=false;
				//endTask=true;
				break;
			}
		}
		if(!inImage.empty())
		{
			//inImage.copyTo(image);
			/*imshow("e",inImage);
			cvWaitKey();*/
			pose=calibrator->FindPattern(inImage); //devo ritornare anche la posa per metterla nel file jpeg
			
			if (pose)
			{
				errstate=0;
				if(*pose==pose_old)
				{
					pose->errorState=REPEATED_POSE;
					errstate=REPEATED_POSE;
				}
				else if(pose->errorState==REPEATED_POSE)
				{
					pose->errorState=0;	
					errstate=0;
				}
			}
			else 
			{
				pose=new Pose_Marker(0,0,0,0,0,0,0);
				pose->errorState=MARKER_NOT_FOUND;
				errstate=MARKER_NOT_FOUND;
			}
			imagedata.setPoseMarker(*pose);

			if(cameraOptions.saveImg)
				SaveFile(inImage, pose);
			inImage.release();

			if(cameraOptions.sendFtp)
				SendViaFTP(pose);

			pose_old=*pose;
			free(pose);
		}
	}
	return 1;
}

void SendViaFTP(Pose_Marker* pose)
{
	string msg=pose->toString();
	
	if(errstate!=FTP_CONNECTION_ERR	&& errstate!=FTP_USER_ERR && errstate!=FTP_PASSWOR_ERR)
		ftps->FTPsendData(msg);
}

int CalibrateCamera()
{
	if (!calibrator->Initialize()){
		throw "[CalibrateCamera] - Initialize() Failed\n";
		return -1;
	}
	return 0;
}

void resetScrollBar()
{
	xMinScroll = 0; 
	xCurrentScroll = 0; 
	xMaxScroll = 0; 

	yMinScroll = 0; 
	yCurrentScroll = 0; 
	yMaxScroll = 0; 

	fBlt = FALSE; 
	fScroll = FALSE; 
	fSize = FALSE; 
}