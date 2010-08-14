// Capture_Estimator.cpp : Defines the entry point for the console application.
//

#define HTTP_IMAGE
//#define CAPT_IMAGE
//#define LOAD_IMAGE
//#define DISPLAY_FRAME
#define PLAY_VIDEO
//#define USE_SQUARE
//#define GRAB_RETRIEVE
#define DEBUG
//#define USEFTP

/*#ifdef LOAD_IMAGE
	#define CAM_H 768
	#define CAM_W 1024
#else*/
	#define CAM_H 480
	#define CAM_W 704
//#endif

#include "stdafx.h"


#include "jpegFunctions.h"
#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>

using namespace std;

string getFileNameToLoad();

class Calibrator{

	string inputImgPath;
	string inputImgName;
	string AICONCalibName;
	string ARTKPCalibName;

	string AICONcalib, ARTKPcalib, inputImage;

	int poseN;
	int threshold[MAX_NUM_THRESH];
	int threshCounter;
	int countNumberThresh;
	int markerIdfound;
	int numMrkFound;

	VECT6 Xestimation;
	TRANSFMATRIX Testimation;
	//MyLogger logger;
	cameraInfo *c;
	CvImage maskImg,/* *calcImg,*/ treshImg, croppedImg;

	ARToolKitPlus::TrackerSingleMarker *tracker ;
	ARToolKitPlus::TrackerSingleMarker *trackerLite;
	ARToolKitPlus::ARMarkerInfo *MI ;

public:
	Calibrator::Calibrator(string path, string AICONCalibName_, string ARTKPCalibName_)
{
	if(path[path.length()-1]!='\\')
		path.append("\\");

	inputImgPath=path;
	AICONCalibName=AICONCalibName_;
	ARTKPCalibName=ARTKPCalibName_;

	Calibrator_();
}

Calibrator::Calibrator(void)
{
	//inputImgName="frame4.png";
	inputImgPath="D:\\Documenti\\Politecnico\\anno5\\Tesi\\Capture_Estimator_svn\\Capture_Estimator\\Capture_Estimator\\";

//#ifdef LOAD_IMAGE
//	AICONCalibName="CAM_RIGHT_AIR_CALIB.ini";
//	ARTKPCalibName="EurobotCamAICONUndistortion2.cal";
//#else
	AICONCalibName="cameraAICON2.ini";
	ARTKPCalibName="cameraTOOLBOX.cal";
//#endif

	Calibrator_();
}

void Calibrator::Calibrator_(void)
{
	poseN=1;

	threshold[0] = 64;
	threshold[1] = 96;
	threshold[2] = 127;
	threshold[3] = 159;
	threshold[4] = 191;
	threshCounter = 0;
	countNumberThresh = 0;
	numMrkFound = -1;


	//inputImage = inputImgPath + inputImgName;
	AICONcalib = inputImgPath + AICONCalibName;
	ARTKPcalib = inputImgPath + ARTKPCalibName;

	tracker = new ARToolKitPlus::TrackerSingleMarkerImpl<18,18,72, 1, N_PATTERN>(CAM_W,CAM_H);
	trackerLite = new ARToolKitPlus::TrackerSingleMarkerImpl<6,6,6, 1, N_PATTERN>(CAM_W,CAM_H);
	MI = new ARToolKitPlus::ARMarkerInfo[N_PATTERN];
	c = new cameraInfo();
	
	//tracker->setLogger(&logger);
	tracker->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_LUM);
	trackerLite->setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_LUM);

	return;
}

Calibrator::~Calibrator(void)
{	
	c->rawimg.release();
	c->unimg.release();
	maskImg.release();

	tracker->cleanup();
	trackerLite->cleanup();
	delete tracker;
	delete trackerLite;
	delete MI;
	delete c;
	
}

int Calibrator::Initialize()
{
	printf("Loading tracker init files...");
	if(!tracker->init(ARTKPcalib.c_str(), 1.0f, 2500.0f))
	{
		printf("ERROR: tracker init() failed. Press any key to exit\n");
		delete tracker;
		_getch();
		return -1;
	}
	printf("Loading trackerLite init files...");
	if(!trackerLite->init(ARTKPcalib.c_str(), 1.0f, 2500.0f))
	{
		printf("ERROR: trackerLite init() failed. Press any key to exit\n");
		delete trackerLite;
		_getch();
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

	printf("DONE!\n");
	return 1;
}

int Calibrator::Calibrate()
{
	printf("Loading calibration LUT...");
	
	if(File_Exists(AICONcalib.c_str()))
		ReadCalibrationCamera(AICONcalib, c);
	else
	{
		printf("\nAICON Calibration File does not exist. Press any key to exit\n");
		_getch();
		return -1;
	}
	UndistortedImageTable(c, true);
	c->rawimg = cvCreateImage(cvSize(CAM_W, CAM_H), IPL_DEPTH_8U, 1);
	c->unimg = cvCreateImage(cvSize(CAM_W, CAM_H), IPL_DEPTH_8U, 1);

	maskImg = cvCreateImage(cvSize(CAM_W, CAM_H), IPL_DEPTH_8U, 1);
//	calcImg = cvCreateImage(cvSize(CAM_W,CAM_H), IPL_DEPTH_8U, 1);
	treshImg = cvCreateImage(cvSize(CAM_W,CAM_H), IPL_DEPTH_8U, 1);
	croppedImg = cvCreateImage(cvSize(CAM_W,CAM_H), IPL_DEPTH_8U, 1);

	printf("DONE!\n");
	return 1;
}

//CvImage Calibrator::LoadMyImage()
//{
//	inputImage=getFileNameToLoad();
//	
//	CvImage im(inputImage.c_str(), 0,CV_LOAD_IMAGE_GRAYSCALE);
//	if(!im)
//	{
//		printf("Image file does not exist. Press any key to exit.\n");
//		_getch();
//		return 0;
//	}
//	/*else
//	{
//		im = cvCreateImage( cvSize( calcImg->width, calcImg->height ),
//                               calcImg->depth, calcImg->nChannels );
//		cvCopyImage( calcImg, im);
//	}*/
//
//	if( (im.width() == 0) || (im.height() == 0) )
//	{
//		printf("NULL image dimension\n");
//	}
//	//else
//	//	printf("Img size %dx%d\n", im->width, im->height);
//	return im;
//}



Pose_Marker* Calibrator::FindPattern(CvImage calcImg)
{
	Pose_Marker *pose = 0;
	bool stop=false;
	bool usedSq=false;
	//Squares sq;
	//IplImage* backupImg=cvCloneImage(calcImg);
	//set<Square_Area*,Square_Area::classcomp>::iterator it;

	while(!stop)
	{
		cvCopyImage(calcImg, c->rawimg);
		
		//UndistortedImageAICON(c, false, CV_RGB(255,0,0));

		//memcpy((uchar*)calcImg->imageData, (uchar*)(c->unimg->imageData), CAM_W*CAM_H);
		
		/*#ifdef USE_SQUARE
			cvShowImage("squared",calcImg);
			cvWaitKey();
			cvDestroyWindow("squared");
		#endif*/

		cvThreshold(calcImg, treshImg, tracker->getThreshold(), 255, CV_THRESH_BINARY);

		
		markerIdfound = trackerLite->calc((uchar*)calcImg.data(), MARKER_ID, false, &MI, &numMrkFound);

#ifndef USE_SQUARE
		if(markerIdfound != -1 )
		{
			/////////////////////////////////THRESHOLDING/////////////////////////////////////////
			trackerLite->activateAutoThreshold(false);
			tracker->setThreshold(trackerLite->getThreshold());
			/////////////////////////////////THRESHOLDING/////////////////////////////////////////
			
		}
#endif
		//The estimator searches only for a specific marker (whose id is specified by the argument markerID)
		//or for any marker if markerID is equal to -1
		
		markerIdfound = tracker->calc((uchar*)calcImg.data(), MARKER_ID, true, &MI);

		if (markerIdfound != -1)
		{
			WriteTMatrixFromFloat((float*)tracker->getModelViewMatrix(), &Testimation);

			/*double teta_x=atan2(Testimation.rev.r.col.j.a[2],Testimation.rev.r.col.k.a[2]);
			double teta_y=atan2((-cos(teta_x)*Testimation.rev.r.col.j.a[0]+sin(teta_x)*Testimation.rev.r.col.k.a[0]),
								(cos(teta_x)*Testimation.rev.r.col.j.a[1]-sin(teta_x)*Testimation.rev.r.col.k.a[1]));
			double teta_z=atan2(-(Testimation.rev.r.col.i.a[2]),
								(sin(teta_x)*Testimation.rev.r.col.j.a[2]+cos(teta_x)*Testimation.rev.r.col.k.a[2]));
*/
			Tmatrix2Vect(Testimation, &Xestimation);
			
			printf( "Found Marker %d\n\n", markerIdfound);
			/*printf( "Estimation [ROLL PITCH YAW X Y Z] deg/cm\n\n");
			printf( "%2.3f\t %2.3f\t %2.3f\t %2.3f\t %2.3f\t %2.3f \r", Xestimation.a[0]*RAD2DEG, Xestimation.a[1]*RAD2DEG, Xestimation.a[2]*RAD2DEG, Xestimation.l.x/10.0, Xestimation.l.y/10.0, Xestimation.l.z/10.0);
			printf( "\n%2.3f\t %2.3f\t %2.3f\t %2.3f\t %2.3f\t %2.3f \r", teta_x*RAD2DEG, teta_y*RAD2DEG, teta_z*RAD2DEG, Xestimation.l.x/10.0, Xestimation.l.y/10.0, Xestimation.l.z/10.0);
*/
			pose = new Pose_Marker(Xestimation.a[0]*RAD2DEG, Xestimation.a[1]*RAD2DEG, Xestimation.a[2]*RAD2DEG, 
							Xestimation.l.x/10.0, Xestimation.l.y/10.0, Xestimation.l.z/10.0,
							markerIdfound,poseN++);

			stop = true;	
			Sleep(0);
		}
		else
		{
#ifdef USE_SQUARE
			if(!usedSq)
			{
				sq.findSquare(calcImg);
				usedSq=true;
			}
			if(!sq.SqAreaSet.empty())
			{
				it=sq.SqAreaSet.begin();
				
				cvCopyImage(backupImg,calcImg);
				sq.MydrawSquares(calcImg, (*it)->corners);
				cout<<"Using squares:"<<(*it)->corners[0].x<<" "<<(*it)->corners[0].y<<" "<<(*it)->corners[2].x<<" "<<(*it)->corners[2].y<<"\n";
				
				cvShowImage("sq",calcImg);
				cvWaitKey();
				cvDestroyWindow("sq");
				
				sq.SqAreaSet.erase(it);
				
			}
			else
			{
				stop = true;
				printf( "Marker not detected\n");
			}
#else
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
			printf( "Marker not detected with threshold %d\n", tracker->getThreshold());
#endif
			
		}
	}
	//cvReleaseImage(&calcImg);
	/*cvReleaseImage(&c->rawimg);
	cvReleaseImage(&c->unimg);
	cvReleaseImage(&maskImg);*/
	//cvCopyImage(c->rawimg,calcImg);
#ifdef USE_SQUARE
	cvReleaseImage(&backupImg);
#endif
	return pose; //devo ritornare anche la posa per metterla nel file jpeg
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

void Calibrator::PrintStatus(bool *flags, int i)
{
	if ((flags[0] == true) && (i==0)) printf("\nEstimator Inactive.\n");
	if ((flags[1] == true) && (i==1)) printf("\nMarker Detected.\n");
	if ((flags[2] == true) && (i==2)) printf("\nMarker Not Detected.\n");

	if (flags[i] == true)  flags[i] = false;

	for (int j=0; j<3; j++)
		if (j!=i) flags[j] = true;
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

	pt1->x = CAM_W;
	pt1->y = CAM_H;
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


bool Calibrator::File_Exists(const char *sPath)
{
	ifstream in(sPath, ios::_Nocreate);
	if(!in)
	{
		in.close();
		return false;
	}
	else
	{
		in.close();
		return true;
	}
}
};

/************************************************************************************/
/************************************************************************************/
/****************************** Capture_Estimator ***********************************/
/************************************************************************************/
/************************************************************************************/


string cameraIp = "192.168.0.199";
string rtspProto = "http://";
string imgNameBase="Capture__2906r_", imgName, ext=".jpg";
string folder="analized";
string imgNameBaseToLoad="analized/FCapture__2906r_";
Calibrator *calibrator;
CriticalSection fileNumbCS;
volatile LONG fileNumber=943;
LONG fileNumberToLoad=603;
int numFrameToElab=15;
bool toStop=false;
bool endTask=true;
queue<CvImage> imageQueue;
CriticalSection imageQueueCS;
FTPSender *ftps;
time_t testTime;
string testDescription=" - Throughtput test - HTTP request - ";


long lTimeout;
HANDLE hThread;
DWORD WINAPI WaitThread ( LPVOID );
void setAlarm();
void stopAlarm();



int CalibrateCamera(); //DWORD WINAPI CalibrateCamera( LPVOID lpParam );
int SaveFile(CvImage image, Pose_Marker *pose);
/*int ElaborateImage(IplImage* inImage); */ DWORD WINAPI ElaborateImage( LPVOID lpParam );
string getTimeFileName();
//DWORD testConnectivity(string ip);
string getUniqueFileName();
//void SendViaFTP(Pose_Marker* pose);

void changeFileNameToPose(int n);

ofstream fcapt("TestHTTPCapt.txt", ios::app);

//globali x cercare una pseudo-sincronizzazione cn l'elaborazione
int elaboratedFrame=0;
clock_t aux_t, end_e;

int _tmain(int argc, _TCHAR* argv[])
{		
	/**/
	//changeFileNameToPose(140);
	//Squares sq;
	//sq.vvv();
	//return 0;
	/**/

	clock_t start_e;
	double tot=0,diff=0;
	CvImage frame;
	int key=0, key2=0;
	string cameraRtspIp=rtspProto;
	cameraRtspIp.append(cameraIp);
	CvCapture *cap;
	
	#ifdef USEFTP
		ftps=new FTPSender(ftpServer,ftpPort);
	#endif

	calibrator = new Calibrator();
	ofstream f("TestHTTP_CaptureEstimator.txt", ios::app);
	
	if(!f) {
        cout<<"Errore nell'apertura del file!";
        return -1;
    }
	time(&testTime);
	f<<"Test start"<<testDescription<<testTime<<endl;

	cout<<endl<< "Capturing file " << cameraIp << endl;
		
		//QueueUserWorkItem(CalibrateCamera,0,0);
		if(CalibrateCamera()==-1)
			return -1;
		/*test connettività camera */
		
#ifdef HTTP_IMAGE
		if(Network::testConnectivity(cameraIp)==0){
			cout<<"camera not connected at ip:"<<cameraIp<<endl;
			cout<<"aborting"<<endl;
			system("pause");
			return 0;
		}   
#endif
#ifdef CAPT_IMAGE
		 
		clock_t startCapture,endCapture;
		//for(int u=0;u<20;u++)
		//{
			//cout<<"capture number "<<u<<endl;
			//CvCapture *cap;
			//setAlarm();
		
			startCapture=clock();	
			cap=cvCreateFileCapture_FFMPEG(cameraRtspIp.c_str());
			endCapture=clock();
			//stopAlarm();
			fcapt<<"cvCreateFileCapture_FFMPEG took "<<double(endCapture-startCapture)<<" milliseconds = "<<double(endCapture-startCapture)/1000<<" sec"<<endl;
			//cvReleaseCapture_FFMPEG(&cap);
		//}
		fcapt<<endl;
		fcapt.close();
		// return 0;
		
#endif
	

#ifdef CAPT_IMAGE
	if(!cap){
		cerr<<"Error opening file " << cameraIp<< endl;
		return 0;
	}
	//cvSetCaptureProperty_FFMPEG(cap, CV_CAP_PROP_FPS, 60);
#endif

/*#ifdef DISPLAY_FRAME
	cvNamedWindow("frame", CV_WINDOW_AUTOSIZE );
	std::cout<<"Displaying frame "<< n << std::endl;
#ifdef GRAB_RETRIEVE	
	if(!cvGrabFrame(cap)){              // capture a frame 
		printf("Could not grab a frame\n");
		exit(0);
	}
	frame =cvRetrieveFrame(cap); 
#else
	//cvSetCaptureProperty( cap, CV_CAP_PROP_POS_AVI_RATIO, 0); // go to % 
	//se diverso da 0 mi da errore con tutti i file

	//cvSetCaptureProperty( cap, CV_CAP_PROP_POS_FRAMES,0); 
	frame = cvQueryFrame( cap );//cattura frame corretto
#endif
	cvShowImage( "frame", frame );
	key = cvWaitKey();
	cvDestroyWindow( "frame" );	
#else*/
#ifdef PLAY_VIDEO

#ifdef HTTP_IMAGE
	clock_t to;
	//IplImage *image;
	char recvbuf[BUFLEN];
	FILE* imageFile; 
	int iResult;
	int headerLen = 207;
	CvImage imgJ;
	string bu="GET /cgi-bin/video.jpg HTTP/1.1\r\n";
			bu+="Accept: */*\r\n";
			bu+="Referer: http://192.168.0.199/snapshot.vspx\r\n";
			bu+="Accept-Language: it\r\n";
			bu+="Accept-Encoding: gzip, deflate\r\n";
			bu+="User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727; .NET CLR 3.0.4506.2152)\r\n";
			bu+="Host: 192.168.0.199\r\n";
			bu+="Connection: Keep-Alive\r\n\r\n";


	Network cameraNet(cameraIp,"80");

	CvImage image;
	CvPoint pt[1], *rect = pt;
	int count=1;
	pt[0].x=367;
	pt[0].y=245;
	int o;
	int toElab=1000;
	
	unsigned char* bufJpeg, *auxbufJpeg;
	int jpegSize = 47720;
	string comment="";


	while( key != 'q' /*&& toElab>elaboratedFrame*/) 
	{
		cameraNet.NWConnect();
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
			/*auxbufJpeg=&(bufJpeg[o]);
			auxbufJpeg=(unsigned char*)malloc((o+iResult)*sizeof(unsigned char));*/
			bufJpeg=(unsigned char*)realloc(bufJpeg,(o+iResult)*sizeof(unsigned char));
			//free(bufJpeg);
			//bufJpeg=auxbufJpeg;
			memcpy(&(bufJpeg[o]), recvbuf,iResult);
			o+=iResult;
		}

		imgJ =readJpegMem (bufJpeg,o,comment);
		//cvPolyLine(imgJ, (&rect), &count, 1, 1, CV_RGB(255,255,255), 3, CV_AA, 0 );
		//if(key2!='e') 
		//{//se nn sto elaborando o se voglio stoppare elaborazione
		//	key2=key;
			cvShowImage( "wnd", imgJ );
		//}

		
		cameraNet.NWCloseConnection();
		if(key=='e')
		{
			to=clock();
			image.create(imgJ.size(), imgJ.depth(), 1 );
			cvCopyImage(imgJ, image);
			{
				Guard p(imageQueueCS);
				imageQueue.push(image);
			}
			if(elaboratedFrame==0){
				start_e=to;
				QueueUserWorkItem(ElaborateImage, 0,0);
			}
			key=0;
			elaboratedFrame++;
		}
		//free(bufJpeg);
		key = cvWaitKey(1);
		
	}
	toStop=true;
	cvDestroyAllWindows();
	end_e=clock();
	f<<"[Capture_Estimator] - End elaborating "<<end_e<<endl;
	f<<"[Capture_Estimator] - Total elaboration time: "<<double(end_e-start_e)<<" milliseconds - "<<elaboratedFrame<<" frames"<<endl;
	f<<"[Capture_Estimator] - Avarage elaboration time: "<<double(end_e-start_e)/elaboratedFrame<<" milliseconds"<<endl;
	f<<"[Capture_Estimator] - Rate: "<<elaboratedFrame/(double(end_e-start_e))*1000<<" frames/sec"<<endl;
	f<<"Test end"<<endl<<endl;
#endif
#ifdef CAPT_IMAGE

	int found;
	string wndName = "video _ 's'ave, 'e'laborate, 'q'uit";
	cvNamedWindow(wndName.c_str(), CV_WINDOW_AUTOSIZE );
	cout<< "Playing video" << endl;
	CvImage image;

	int toElab[1]={10};//,1000,1000,1000,1000,1000,1000};
	int elabIndex=0,rateIndex=-1,rateLimit=3;

	while( key != 'q' /*&& elaboratedFrame<numFrameToElab */) 
	{
		frame = cvQueryFrame( cap );
		if( !frame ) break;

		if(elaboratedFrame==toElab[elabIndex])
		{
			key='s';
			//elabIndex++;
		}
		if(elabIndex>0 && elaboratedFrame==0 && endTask)
			key='e';

		if(key2!='e' || key=='s') //se nn sto elaborando o se voglio stoppare elaborazione
			key2=key;

		if(!toStop || endTask)
		{//se nn sto aspettando che termini elaborazione nell'altro thread
			switch (key2)
			{
			case 'e':
				rateIndex=((rateIndex+1)%rateLimit);
				if(rateIndex==0)
				{
					aux_t=clock();
					image = cvCreateImage( cvSize( frame->width, frame->height ),
									   frame->depth, frame->nChannels );
					cvCopyImage( frame, image);
					{
						Guard p(imageQueueCS);
						imageQueue.push(image);
					}
					
					//found=ElaborateImage(image);
					//t2=clock();
					//diff=t2-t1;
					//f<<"Elaborating took for " << diff << " milliseconds - pattern found "<<found<<endl;
					//tot+=diff;
					if(elaboratedFrame==0)
					{
						QueueUserWorkItem(ElaborateImage, 0,0); //faccio partire elavorazione la prima volta
						start_e=aux_t;
						f<<"[Capture_Estimator] - Start elaborating "<<start_e<<endl;
					}
					elaboratedFrame++; 
					//rateLimit==3 ? rateLimit=2 : rateLimit=3 ;
				}

				break;

			case 's':
				toStop=true;		//serve per segnalare all'altro thread che non saranno piu inseriti altri frame
				end_e=clock();
				f<<"[Capture_Estimator] - End elaborating "<<end_e<<endl;
				f<<"[Capture_Estimator] - Total elaboration time: "<<double(end_e-start_e)<<" milliseconds - "<<elaboratedFrame<<" frames"<<endl;
				f<<"[Capture_Estimator] - Avarage elaboration time: "<<double(end_e-start_e)/elaboratedFrame<<" milliseconds"<<endl;
				f<<"[Capture_Estimator] - Rate: "<<(elaboratedFrame/double(end_e-start_e))*1000<<" frames/sec"<<endl<<endl;
				
				elaboratedFrame=0;
			}
		}
	
		key=' ';
	
		//if(key2!='e')
			cvShowImage( wndName.c_str(), frame );

		key = cvWaitKey(1);
		/*if(elabIndex==1)
			key='q';*/
	}
	//toStop=true;		//serve per segnalare all'altro thread che non saranno piu inseriti altri frame
	//end_e=clock();
	//f<<"[Capture_Estimator] - End elaborating "<<end_e<<endl;
	//f<<"[Capture_Estimator] - Total elaboration time: "<<double(end_e-start_e)<<" milliseconds - "<<elaboratedFrame<<" frames"<<endl;
	//f<<"[Capture_Estimator] - Avarage elaboration time: "<<double(end_e-start_e)/elaboratedFrame<<" milliseconds"<<endl;
	//f<<"[Capture_Estimator] - Rate: "<<double((elaboratedFrame/(end_e-start_e))*1000))<<" frames/sec"<<endl;
	//f<<"Test end"<<endl<<endl;
	f.close();
	cvDestroyWindow( wndName.c_str() );
#else
#ifdef LOAD_IMAGE
	while(elaboratedFrame<=numFrameToElab)
	{
		frame=calibrator->LoadMyImage();
		if(frame)
		{
			IplImage *image = cvCreateImage( cvSize( frame->width, frame->height ),
								   frame->depth, frame->nChannels );
			cvCopyImage( frame, image);
			{
				Guard p(imageQueueCS);
				imageQueue.push(image);
			}
		}
		//ElaborateImage(image);
		if(elaboratedFrame==1)
			QueueUserWorkItem(ElaborateImage, 0,0); //faccio partire elavorazione la prima volta
		elaboratedFrame++; 

	//QueueUserWorkItem(ElaborateImage, calibrator->LoadMyImage(),0);
	//ElaborateImage(calibrator->LoadMyImage());
	}
#endif
#endif
#endif
////#endif
#ifdef CAPT_IMAGE
	cvReleaseCapture(&cap);
#endif	
	//cout<<"elaborated "<<elaboratedFrame<<" frames"<<endl;
	system("pause");
	return 0;
}

//DWORD WINAPI CalibrateCamera( LPVOID lpParam )
int CalibrateCamera()
{
	if (!calibrator->Initialize()){
		throw "[CalibrateCamera] - Initialize() Failed\n";
		return -1;
	}
	if (!calibrator->Calibrate()){
		throw "[CalibrateCamera] - Calibrate() Failed\n";
		return -1;
	}
	return 0;
}


DWORD WINAPI WaitThread ( LPVOID ) 
{
	Sleep (lTimeout);
	fcapt<< "cvCreateFileCapture_FFMPEG is lasting too long...Terminated."<<endl;
	cout<<'\a';
	ExitProcess(0);
	return 0;
}


void setAlarm()
{	
	DWORD dwTID;
	lTimeout=10000; //10sec
	hThread = CreateThread( NULL,0, WaitThread, 0, 0, &dwTID);
}

void stopAlarm()
{
	TerminateThread(hThread,0); // cancel thread
}

//DWORD testConnectivity(string ip)
//{
//	HANDLE hIcmpFile;
//    unsigned long ipaddr = INADDR_NONE;
//    DWORD dwRetVal = 0;
//    char SendData[] = "Data Buffer";
//    LPVOID ReplyBuffer = NULL;
//    DWORD ReplySize = 0;
//
//	ipaddr = inet_addr(ip.c_str());
//    
//    hIcmpFile = IcmpCreateFile();
//    if (hIcmpFile == INVALID_HANDLE_VALUE) {
//        printf("\tUnable to open handle.\n");
//        printf("IcmpCreatefile returned error: %ld\n", GetLastError() );
//        return 0;
//    }    
//
//    ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
//    ReplyBuffer = (VOID*) malloc(ReplySize);
//    if (ReplyBuffer == NULL) {
//        printf("\tUnable to allocate memory\n");
//        return 0;
//    }    
//	
//	return IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), 
//						NULL, ReplyBuffer, ReplySize, 1000);
//
//}



int SaveFile(CvImage image, Pose_Marker *pose)
{
	
	CvImage cvimage(image);
	string comment;
	string options="quality=90";
	string filename=getUniqueFileName();
	
	if(!pose)
		comment="Pattern not found";
	else{
		comment=pose->toString();
		filename[9]='F';			//per riconoscere se la posa è stata trovata dal nome del file
	}
	writeJpegFile ((char*)(filename.c_str()), cvimage, options, comment);
	cout<<"Saved image "<<filename<<endl;
	//string comment="";
	// Now I try to read the file and allocate it into a CvImage class
	//CvImage img = readJpegFile("outputTest.jpg",comment);
	// I visualize the results obtained

	//string strWin ="test read image ";
	//strWin += options;
	//cvNamedWindow( strWin.c_str(), CV_WINDOW_AUTOSIZE );
	//img.show(strWin.c_str());
	//cout<<"Comment:" << comment.c_str()<<endl;
	return 1;

}

DWORD WINAPI ElaborateImage( LPVOID lpParam )
//int ElaborateImage(IplImage* inImage)
{
	
	int count=4,xcount=2, ycount=2;
	ofstream filePose("Test_0107.txt",ios::app);
	int recognized=0, n=1;
	//ofstream f2("TestElab2Threads_ElaborateImage_Timing.txt", ios::app);
	ofstream f3("TestHTTP_ElaborateImage.txt", ios::app);
	CvImage image(cvSize(CAM_W,CAM_H), IPL_DEPTH_8U, 1);
	Pose_Marker *pose, *pose2;
	if(!f3) {
        cout<<"Errore nell'apertura del file!";
		f3.close();
        return -1;
    }
	//f2<<"[ElaborateImage] - Test start"<<endl;
	CvImage inImage;//=reinterpret_cast<IplImage*>(lpParam);
	double tot=0 ;
	int frames=0,emptyCicle=0;
	endTask=false;
	clock_t start_ElaborateImage=clock(),t5,t6;
	f3<<"Test Start"<<testDescription<<testTime<<endl;
	f3<<"[ElaborateImage] - Start elaborating "<<start_ElaborateImage<<endl;
	filePose<<endl<<"Parametri [380,45102 246,24552 846,63525 765,88469 -0,23620   0,36864   -0,00032   0,00024  0,00000 0,0 5](toolbox) -no undist AICON - relative test"<<endl;
	double xcentro=381.5066, ycentro=230.9;
	/****/
	double alpha=23;//degree
	double b_tan_alpha;

	/****/
	
	while(true)
	{
		{
			t5=clock();
			Guard p(imageQueueCS);
			if(!imageQueue.empty())
			{
				inImage=imageQueue.front();
				imageQueue.pop();
			}
			else if (toStop)
			{
				toStop=false;
				endTask=true;
				break;
			}
			else
			{
				emptyCicle++;
			}
		}
		if(inImage)
		{
			frames++;
			cvCopyImage(inImage,image);
			pose=calibrator->FindPattern(image); //devo ritornare anche la posa per metterla nel file jpeg
			filePose<<"image "<<n<<" ";
			if (pose)
			{
				/*CvPoint pt[4], *rect = pt;
				CvPoint xpt[2], *xline = xpt;
				CvPoint ypt[2], *yline = ypt;
				xpt[0]=cvPoint(xcentro,0);
				xpt[1]=cvPoint(xcentro,480);
				ypt[0]=cvPoint(0,ycentro);
				ypt[1]=cvPoint(704,ycentro);
				b_tan_alpha= 352/(pose->distZ*tan(alpha*PI/180));
				double xc=xcentro+(b_tan_alpha*pose->distX);
				double yc=ycentro+(b_tan_alpha*pose->distY);
	
				pt[0]=cvPoint(xc-2,yc-2);
				pt[1]=cvPoint(xc+2,yc-2);
				pt[2]=cvPoint(xc+2,yc+2);
				pt[3]=cvPoint(xc-2,yc+2);
				cout<<pose->distX<<" "<<pose->distY<<" ";
				cout<<xc<<" "<<yc<<endl;
				cvPolyLine(image, (&rect), &count, 1, 1, CV_RGB(255,255,255), 1, CV_AA, 0 );
				cvPolyLine(image, (&xline), &xcount, 1, 1, CV_RGB(255,255,255), 1, CV_AA, 0 );
				cvPolyLine(image, (&yline), &ycount, 1, 1, CV_RGB(255,255,255), 1, CV_AA, 0 );
				cvCircle(image,cvPoint(xc,yc),2,CV_RGB(0,0,0));*/
				//cvShowImage("center",image);
				//cvWaitKey();
				//cvDestroyWindow("center");

				//test resize image
				//if (pose->distZ>70)
				//{//pp 381.5066 230.9
				//	/* da prove empiriche ricavo che 
				//		distanza_da_marker_in_cm * dimensione_marker_in_pixel = 12500 (circa)
				//		distanza_da_marker_in_cm / 75 = cm_al_pixel_sull_immagine
				//	*/
				//	/*resize cercando di centrare il marker*/
				//	/*double cm_al_pixel=pose->distZ/75;
				//	double xc=381.5066+(cm_al_pixel*pose->distX);
				//	double yc=230.9+(cm_al_pixel*pose->distY);
				//	double x,y;
				//	
				//	(xc>(inImage->width/4))? x=xc-(inImage->width/4) : x=0;
				//	(yc>(inImage->height/4))? y=yc-(inImage->height/4): y=0 ;
				//	CvRect roi=cvRect(x, y, inImage->width/2, inImage->height/2);*/

				//	/*resize verso centro*/
				//	CvRect roi=cvRect(inImage->width/4, inImage->height/4, inImage->width/2, inImage->height/2);
				//	cvSetImageROI(inImage,roi);
				//	cvResize(inImage,image);
				//	//cvShowImage("resized",image);
				//	//cvWaitKey();
				//	cvResetImageROI(inImage);
				//	pose2=calibrator->FindPattern(image);
				//	if(pose2)
				//	{
				//		filePose<<"resized";
				//		pose=pose2;
				//	}
				//}
				filePose<<pose->toLine();
			}
			else 
				filePose<<endl;
			n++;
			SaveFile(inImage, pose);
			inImage.release();
			
		#ifdef USEFTP
			SendViaFTP(pose);
		#endif
			t6=clock();
			//f2<<"[ElaborateImage] - Elaborazione: "<<double(t6-t5)<<endl;
			tot+=(t6-t5);
			if(pose)
			{
				free(pose);
				recognized++;
			}
		}
	}
	clock_t end_ElaborateImage=clock();
	f3<<"[ElaborateImage] - End elaborating "<<end_ElaborateImage<<endl;
	f3<<"[ElaborateImage] - Delay "<<double(end_ElaborateImage-end_e)<<endl;
	f3<<"[ElaborateImage] - Total elaboration time: "<<double(end_ElaborateImage-start_ElaborateImage)<<" - "<<frames<<" frames"<<endl;
	f3<<"[ElaborateImage] - Total avarage elaboration time: "<<double((end_ElaborateImage-start_ElaborateImage)/frames)<<endl;
	f3<<"[ElaborateImage] - Rate: "<<(frames/double(end_ElaborateImage-start_ElaborateImage))*1000<<" frames/sec"<<endl;
	f3<<"[ElaborateImage] - Correctly recognized "<<recognized<<endl;
	f3<<"[ElaborateImage] - Wasted Time: "<<double(end_ElaborateImage-start_ElaborateImage-tot)<<" - Empty Clicles: "<<emptyCicle<<endl<<endl;
	
	return 1;
}

void SendViaFTP(Pose_Marker* pose)
{
	string msg,received, ftpServer="localhost", ftpPort="2000", ftpUser="", ftpPass="";
	int maxRcv;

	if(!pose)
	{
		maxRcv=1;
		msg="-1\n";
	}
	else
	{
		maxRcv=7;
		msg=pose->toString();
	}
	
	if(!ftps->FTPconnect(ftpUser,ftpPass))
	{
		cerr<<"[SendViaFTP] Error: connectFTP()"<<endl;
		return;
	}
	ftps->FTPsendData(msg);


	for(int i=0;i<maxRcv;i++)
	{
		ftps->FTPrcvData(&received);
		cout<<"Received: "<<received<<endl;
	}
}


string getUniqueFileName()
{
	stringstream ss;
	{
	Guard l(fileNumbCS);
	ss<<folder<<"//"<<imgNameBase<<fileNumber<<ext;
	fileNumber++;
	}
	return ss.str();
}

string getFileNameToLoad()
{
	stringstream ss;
	{
	//Guard l(fileNumbCS);
	ss<<imgNameBaseToLoad<<fileNumberToLoad<<ext;
	fileNumberToLoad++;
	}
	return ss.str();
}

string getTimeFileName()
{
	stringstream ss;
	time_t imgTime;
	//struct tm  *timeinfo=new tm();
	//char *buf;

	//imgTime=time(0);
	time(&imgTime);
	ss<<imgNameBase<<imgTime<<ext;
	
	/*string s=ctime(&imgTime);
	//localtime_s(timeinfo,&imgTime);
	//buf=asctime(timeinfo);
	//s[s.end()]='\0'; //tolgo \n finale
	s=s.substr(0,s.length()-1);
	imgName=imgNameBase;
	std::remove(s.begin(), s.end(), ' ');
	imgName.append(s);
	imgName.append(ext);*/
	
	return ss.str();
}

void changeFileNameToPose(int n)
{
	string comment;
	string options="quality=90";
	float r,p,y;

	for(int k=0;k<n; k++)
	{
		stringstream ss;
		ss<<"images\\Fapture__1006__"<<k<<".jpeg";
		
		CvImage img = readJpegFile((char*)(ss.str().c_str()),comment);

		if(img)
		{
			sscanf(comment.c_str(),"roll:%f\npitch:%f\nyaw:%f\n",&r,&p,&y);
			stringstream tt;
			tt<<"test\\"<<k<<"_640x480_"<<r<<"_"<<p<<"_"<<y<<"_"<<".jpeg";

			writeJpegFile ((char*)(tt.str().c_str()), img,options, comment);
		}
	}

}