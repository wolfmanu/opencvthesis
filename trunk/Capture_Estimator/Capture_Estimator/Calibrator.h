//#pragma once
//
//#include "stdafx2.h"
//	#define CAM_H 240
//	#define CAM_W 320
//
//using namespace std;
//
//class Calibrator
//{
//public:
//	Calibrator (void);
//	Calibrator(string path, string AICONCalibName_, string ARTKPCalibName_);
//	~Calibrator(void);
//	
//	int Initialize();
//	int Calibrate();
//	IplImage* LoadMyImage();
//	Pose_Marker* FindPattern(IplImage* image);
//
//private:
//	void WriteTMatrixFromFloat(float* matr, TRANSFMATRIX *Tmx);
//	void FindMaskCorners(ARToolKitPlus::ARMarkerInfo *MI, CvPoint* pt1, CvPoint* pt2);
//	void PrintStatus(bool *flags, int i);
//	bool File_Exists(const char *sPath);
//
//	void Calibrator_(void);
//
//	string inputImgPath;
//	string inputImgName;
//	string AICONCalibName;
//	string ARTKPCalibName;
//
//	string AICONcalib, ARTKPcalib, inputImage;
//
//	int threshold[MAX_NUM_THRESH];
//	int threshCounter;
//	int countNumberThresh;
//	int markerIdfound;
//	int numMrkFound;
//
//	VECT6 Xestimation;
//	TRANSFMATRIX Testimation;
//	//MyLogger logger;
//	cameraInfo *c;
//	IplImage *maskImg, *calcImg, *treshImg, *croppedImg;
//
//	ARToolKitPlus::TrackerSingleMarker *tracker ;
//	ARToolKitPlus::TrackerSingleMarker *trackerLite;
//	ARToolKitPlus::ARMarkerInfo *MI ;
//};
