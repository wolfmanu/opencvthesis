// Capture_Estimator.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "jpegFunctions.h"
#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>

using namespace std;

string cameraIp = DEF_CAMERA_IP;
string imgNameBaseToSave = DEF_IMG_FILE_BASE_NAME_SAVE;
string ext = DEF_EXTENSION;
string saveFolder = DEF_SAVE_FOLDER;
string loadFolder = DEF_LOAD_FOLDER;
string imgNameBaseToLoad = DEF_IMG_FILE_BASE_NAME_LOAD;
string user = DEF_USER;
string password = DEF_PASSWORD;
string server = DEF_SERVER;
string port = DEF_PORT;
string calibfile = DEF_CALIB_FILE;
LONG fileNumberToSave = DEF_IMG_FILE_START_NUM_SAVE;
LONG fileNumberToLoad = DEF_IMG_FILE_START_NUM_LOAD;
LONG numFrameToElab = DEF_NUM_FRAME_TO_ELAB;
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
	printf("Loading tracker init files...");
	if(!tracker->init(ARTKPcalib.c_str(), 1.0f, 2500.0f))
	{
		printf("ERROR: tracker init() failed.\nLoading default parameters\n");
		ofstream tempfile;
		tempfile.open ("temp.cal");
		tempfile << "ARToolKitPlus_CamCal_Rev02"<<endl;
		tempfile << "704 480 380.45102 246.24552 846.63525 765.88469 -0.23620   0.36864   -0.00032   0.00024  0.00000 0.0 5"<<endl;
		tempfile.close();
		tracker->init("temp.cal", 1.0f, 2500.0f);
		return -1;
	}
	printf("Loading trackerLite init files...");
	if(!trackerLite->init(ARTKPcalib.c_str(), 1.0f, 2500.0f))
	{
		printf("ERROR: tracker init() failed.\nLoading default parameters\n");
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
			
			printf( "Found Marker %d\n", markerIdfound);
			
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
			printf( "Marker not detected with threshold %d\n", tracker->getThreshold());
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
/****************************** Capture_Estimator ***********************************/
/************************************************************************************/
/************************************************************************************/

bool toStop=false;
bool endTask=true;
queue<Mat> imageQueue;
CriticalSection imageQueueCS;
FTPSender *ftps;
Calibrator *calibrator;
CriticalSection fileNumbCS;
string imgName;
long lTimeout;
HANDLE hThread;

DWORD WINAPI WaitThread ( LPVOID );
void setAlarm();
void stopAlarm();

string getFileNameToLoad();
int CalibrateCamera(); 
int SaveFile(Mat image, Pose_Marker *pose);
DWORD WINAPI ElaborateImage( LPVOID lpParam );
string getUniqueFileName();
Mat LoadMyImage();
void SendViaFTP(Pose_Marker* pose);
void showHelp();

int errstate=0;
int elaboratedFrame=0;

bool usecamera=false,
		useftp=false,
		useload=false,
		usesave=false,
		usevideo=true;

int main(int argc, char** argv)
{		
	if(argc>1)
	{	
		if(strcmp(argv[1], HELP)==0)
		{
			showHelp();
			system("pause");
			return 0;
		}
		for(int i=1; i<argc; i++)
		{
			if(strcmp(argv[i], USE_CAMERA)==0)
			{
				usecamera=true;
				if(useload)
				{
					cout<<"Incompatible load and camera combination\nAborting\n";
					system("pause");
					return 0;
				}
			}
			else if(strcmp(argv[i], USE_LOAD)==0)
			{
				useload=true;
				if(usecamera)
				{
					cout<<"Incompatible load and camera combination\nAborting\n";
					system("pause");
					return 0;
				}
			}
			else if(strcmp(argv[i], NO_VIDEO)==0)
				usevideo=false;
			else if(strcmp(argv[i], USE_FTP)==0)
				useftp=true;
			else if(strcmp(argv[i], USE_SAVE)==0)
				usesave=true;
			else if(strcmp(argv[i], IMG_FILE_BASE_NAME_LOAD)==0)
			{
				if(++i<argc)
					if(argv[i][0]!='-')
						imgNameBaseToLoad=(argv[i]);
					else
						cout<<"Invalid file name "<<argv[i]<<". Using default"<<endl;
				else 
					cout<<"Missing file name. Using default"<<endl;
			}
			else if(strcmp(argv[i], IMG_FILE_BASE_NAME_SAVE)==0)
			{
				if(++i<argc)
					if(argv[i][0]!='-')
						imgNameBaseToSave=argv[i];
					else
						cout<<"Invalid file name "<<argv[i]<<". Using default"<<endl;
				else 
					cout<<"Missing file name. Using default"<<endl;
			}
			else if(strcmp(argv[i], IMG_FILE_START_NUM_LOAD)==0)
			{
				if(++i<argc && argv[i][0]!='-')
					fileNumberToLoad=strtol(argv[i], NULL, 10);
				else 
					cout<<"Missing file start number. Using default"<<endl;
			}
			else if(strcmp(argv[i], IMG_FILE_START_NUM_SAVE)==0)
			{
				if(++i<argc && argv[i][0]!='-')
					fileNumberToSave=strtol(argv[i], NULL, 10);
				else 
					cout<<"Missing file start number. Using default"<<endl;
			}
			else if(strcmp(argv[i], HOW_MANY_LOAD)==0)
			{
				if(++i<argc && argv[i][0]!='-')
				{
					numFrameToElab=strtol(argv[i], NULL, 10);
					if(numFrameToElab==0)
					{
						cout<<"Invalid number of file to elaborate."<<argv[i]<<endl;
						system("pause");
						return 0;
					}
				}
				else 
					cout<<"Missing number of file to elaborate. Using default"<<endl;
			}
			else if(strcmp(argv[i], WIDTH)==0)
			{
				if(++i<argc || argv[i][0]!='-')
				{
					cam_w=atoi(argv[i]);
					if(cam_w==0)
						{
							cout<<"Invalid camera width "<<argv[i]<<endl;
							system("pause");
							return 0;
						}
					}
				else 
					cout<<"Missing camera width. Using default"<<endl;
			}
			else if(strcmp(argv[i], HEIGHT)==0)
			{
				if(++i<argc || argv[i][0]!='-')
				{
					cam_h=atoi(argv[i]);
					if(cam_w==0)
					{
						cout<<"Invalid camera height "<<argv[i]<<endl;
						system("pause");
						return 0;
					}
				}
				else 
					cout<<"Missing camera height. Using default"<<endl;
			}
			else if(strcmp(argv[i], CAMERA_IP)==0)
			{
				if(++i<argc)
					cameraIp=argv[i];
			}
			else if(strcmp(argv[i], EXTENSION)==0)
			{
				if(++i<argc)
					ext=argv[i];
			}
			else if(strcmp(argv[i], SAVE_FOLDER)==0)
			{
				if(++i<argc)
				{
					saveFolder=argv[i];
					wstring wstr(saveFolder.begin(), saveFolder.end());
					
					if(GetFileAttributes(wstr.c_str())==INVALID_FILE_ATTRIBUTES)
					{
						cout<<"[Warnign] - The directory doesn't exist. Create it? [y/n] ";
						char a;
						cin>>a;
						if(a=='y' || a=='Y')
						{
							if(!CreateDirectory(wstr.c_str(), NULL))
							{
								cout<<endl<<"Error creating the directory. Using default "<<DEF_SAVE_FOLDER<<endl;
								saveFolder = DEF_SAVE_FOLDER;
							}
							else
								cout<<endl<<"Directory created succesfully."<<endl;
						}
						else
						{
							cout<<endl<<"Directory not created. Using default "<<DEF_SAVE_FOLDER<<endl;
							saveFolder = DEF_SAVE_FOLDER;
						}
					}
				}
			}
			else if(strcmp(argv[i], LOAD_FOLDER)==0)
			{
				if(++i<argc)
					loadFolder=argv[i];
			}
			else if(strcmp(argv[i], USER)==0)
			{
				if(++i<argc)
					user=argv[i];
			}
			else if(strcmp(argv[i], PASSWORD)==0)
			{
				if(++i<argc)
					password=argv[i];
			}
			else if(strcmp(argv[i], SERVER)==0)
			{
				if(++i<argc)
					server=argv[i];
			}
			else if(strcmp(argv[i], PORT)==0)
			{
				if(++i<argc)
					port=argv[i];
			}
			else if(strcmp(argv[i], CALIB_FILE)==0)
			{
				if(++i<argc)
					calibfile=argv[i];
			}
			else
			{
				cout<<"Invalid "<<argv[i]<<" command\n";
				system("pause");
				return 0;
			}
		}
	}
	Mat frame;
	int key=0;
	char recvbuf[BUFLEN];
	int iResult;
	int headerLen = HEADER_LEN;
	string bu;
	
	int count=1;
	int o;
	
	unsigned char* bufJpeg, *auxbufJpeg;
	string comment="";
	
	if(useftp)
	{
		ftps=new FTPSender(server, port );
		int err=ftps->FTPconnect(user, password);
		if(err==-1)
			errstate=FTP_CONNECTION_ERR;
		else if(err==-2)
			errstate=FTP_USER_ERR;
		else if(err==-3)
			errstate=FTP_PASSWOR_ERR;
	}
	
	calibrator = new Calibrator(calibfile);
		
	if(CalibrateCamera()==-1)
		errstate=CALIB_FILE_READ_ERR;

	if(usecamera)
	{
		if(Network::testConnectivity(cameraIp)==0)
		{
			cout<<"Camera not connected at ip:"<<cameraIp<<endl;
			cout<<"Aborting"<<endl;
			system("pause");
			return CAM_NOT_CONNECTED;
		}   

		bu="GET /cgi-bin/video.jpg HTTP/1.1\r\n";
		bu+="Host: ";
		bu+=cameraIp;
		bu+="\r\n";
		bu+="Connection: Keep-Alive\r\n\r\n";

		Network cameraNet(cameraIp, "80");
	
		while( key != 'q' && elaboratedFrame<=numFrameToElab) 
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

				Mat imgJ(readJpegMem (bufJpeg,o,comment));
			
				if(usevideo)
					imshow( "wnd", imgJ );
				
				cameraNet.NWCloseConnection();

			//if(key=='e')
			//{
				///image.create(imgJ.rows, imgJ.cols, imgJ.type() );
				//imgJ.copyTo( image);
				{
					Guard p(imageQueueCS);
					imageQueue.push(imgJ);
				}
				if(elaboratedFrame==0)
					QueueUserWorkItem(ElaborateImage, 0,0);

				key=0;
				elaboratedFrame++;
			//}
			}
		
			key = cvWaitKey(1);	
		}
		toStop=true;
		cvDestroyAllWindows();
	}


	else if(useload)
	{
		while(elaboratedFrame<=numFrameToElab)
		{
			Mat frame=LoadMyImage();
			if(!frame.empty())
			{
				Guard p(imageQueueCS);
				imageQueue.push(frame);
			}
			if(elaboratedFrame==0)
				QueueUserWorkItem(ElaborateImage, 0,0); //faccio partire elavorazione la prima volta
			elaboratedFrame++; 
		}
		toStop=true;
	}
	
	system("pause");
	return 0;
}


int CalibrateCamera()
{
	if (!calibrator->Initialize()){
		throw "[CalibrateCamera] - Initialize() Failed\n";
		return -1;
	}

	return 0;
}


DWORD WINAPI WaitThread ( LPVOID ) 
{
	Sleep (lTimeout);
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

DWORD WINAPI ElaborateImage( LPVOID lpParam )
{
	int count=4,xcount=2, ycount=2;
	
	int recognized=0, n=1;
	
	Mat image(cvSize(cam_w,cam_h), IPL_DEPTH_8U, 1);
	Pose_Marker *pose, pose_old;
	
	Mat inImage;
	
	int frames=0;
	endTask=false;
	
	if(useftp)
		SendViaFTP(&pose_old);	//x inviare stato di errore prima di iniziare elaborazione immagine

	while(true)
	{
		{
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
		}
		if(!inImage.empty())
		{
			inImage.copyTo(image);
			pose=calibrator->FindPattern(image); //devo ritornare anche la posa per metterla nel file jpeg
			
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
			
			if(usesave)
				SaveFile(inImage, pose);
			inImage.release();

			if(useftp)
				SendViaFTP(pose);

			pose_old=*pose;
			free(pose);
		}
	}
	return 1;
}

void SendViaFTP(Pose_Marker* pose)
{
	string msg=pose->toFTP();
	
	if(errstate!=FTP_CONNECTION_ERR	&& errstate!=FTP_USER_ERR && errstate!=FTP_PASSWOR_ERR)
		ftps->FTPsendData(msg);
}


string getUniqueFileName()
{
	stringstream ss;
	{
	Guard l(fileNumbCS);
	ss<<saveFolder<<"//"<<imgNameBaseToSave<<fileNumberToSave<<DEF_EXTENSION;
	fileNumberToSave++;
	}
	return ss.str();
}

string getFileNameToLoad()
{
	stringstream ss;
	{
	ss<<loadFolder<<"//"<<imgNameBaseToLoad<<fileNumberToLoad<<ext;
	fileNumberToLoad++;
	}
	return ss.str();
}

Mat LoadMyImage()
{
	string inputImage=getFileNameToLoad();
	
	Mat im=imread(inputImage, 0); //grayscale
	if(im.empty())
		cout<<"File "<<inputImage<<" does not exist. Skipped\n";
		
	return im;
}

void showHelp()
{
	cout<<"Usage Capture_Estimator.exe [OPTION][PARAMETER VALUE]"<<endl;
	cout<<"Options:"<<endl;
	cout<< NO_VIDEO		<<"\tDo not display the stream"	<<endl;
	cout<< USE_CAMERA	<<"\t\tCapture the images from the camera"<<endl;
	cout<< USE_LOAD		<<"\t\tLoad the images from files"<<endl;
	cout<< USE_FTP		<<"\t\tSend results via FTP"<<endl;
	cout<< USE_SAVE		<<"\t\tSave images with results"<<endl;
		cout<<endl;
	cout<<"Parameters:"<<endl;
	cout<< CAMERA_IP				<<" xxx.xxx.xxx.xxx\tThe ip of the camera"<<endl;
		cout<<endl;
	cout<< CALIB_FILE				<<" filename\tThe calibration file"<<endl;
		cout<<endl;
	cout<< IMG_FILE_BASE_NAME_SAVE	<<" basename\t\tThe base name of the file to save(without extension)"<<endl;
	cout<< SAVE_FOLDER				<<" folder\tThe folder where save the images"<<endl;
	cout<< IMG_FILE_START_NUM_SAVE	<<" nn\t\tThe first number of the file to save"<<endl;
		cout<<endl;	
	cout<< IMG_FILE_BASE_NAME_LOAD	<<" basename\t\tThe base name of the file to load(without extension)"<<endl;
	cout<< EXTENSION				<<" .xxx\t\tThe extension of the file to load (default .jpg)"<<endl;
	cout<< LOAD_FOLDER				<<" folder\tThe folder from where load the images"<<endl;
	cout<< IMG_FILE_START_NUM_LOAD	<<" nn\t\tThe first number of the file to load"<<endl;
		cout<<endl;	
	cout<< HOW_MANY_LOAD			<<" nn\tHow many images elaborate (both for camera or load)"<<endl;
		cout<<endl;
	cout<< USER						<<" user\t\t\tThe username of the FTP connection"<<endl;
	cout<< PASSWORD					<<" password\t\tThe passord of the FTP connection"<<endl;
	cout<< SERVER					<<" xxx.xxx.xxx.xxx\tThe ip of the FTP server (default localhost)"<<endl;
	cout<< PORT						<<" nnnn\t\t\tThe port of th FTP connection"<<endl;
		cout<<endl;	
	cout<< HELP						<<"\tShow this help"<<endl;
}