/*---------------------------------------------------------------------------
#Program.......: alenia.exe
#File..........: calibration.cpp
#Purpose.......: implementation of the calibration on the input image in accordance with AICON rules
#Author........:  Ingo Ahrns modified by P. Lanza
#Modified......:  P. Lanza
#Created.......: 12/09/2006
#Last Changed..: 04/03/2007
#Version.......: 03
#Copyright.....:
#License.......:
#ToDo..........:
#Note..........:


#---------------------------------------------------------------------------*/


#include "cameraInfo.h"
#include "calibration.h"
#include "ReadFileIni.h"

#include <math.h>
//#include <vector>
//#include <string>
//#include <fstream>
//#include <iostream>

using namespace std;


static int VERBOSE = 1;
static int DEBUG = 1;   //#### test scope




//***************************************************************************
/** LoadPGM.
 ****************************************************************************
 * This method loads an image from a given file in portable grey map format
 * (PGM).
 *
 * \param    (i) filename Name of the PGM file to be loaded.
 * \param    (o) width Width of the image in pixels.
 * \param    (o) height Height of the image in pixels.
 * \return   true in case of success, false otherwise.
 * \see      SavePGM
 * \version  $Revision 1.0 $
 * \author   Ingo Ahrns (EADS Space Transportation)
 ***************************************************************************/
bool LoadPGM(string filename, int &width, int &height, vector<char> &image)
{
  const int BUF_LEN = 1024;
  FILE *file;
  file = fopen(filename.c_str(), "rb");

  char format[BUF_LEN];
  char tmp[BUF_LEN];
  int  val;

  if (file==NULL)
    return false;

  // read header
  fgets(format, BUF_LEN, (FILE*)file);

  // binary format
  if ((format[0]=='P') && (format[1]=='5'))
  {
    fgets(tmp, BUF_LEN, (FILE*)file);
    if (tmp[0] == '#')
      fgets(tmp, BUF_LEN, (FILE*)file);
    sscanf(tmp, "%d%d", &width, &height);
    fgets(tmp, BUF_LEN, (FILE*)file);

    // read data
    image.resize(width*height);
    vector<char>::iterator I=image.begin();

    if ((fread(&(*I), 1, width*height, file)) != (size_t)(width*height))
      return false;
  }
  // ascii format
  else if ((format[0]=='P') && (format[1]=='2'))
  {
    fclose(file);
    file=fopen(filename.c_str(), "r");

    fgets(format, BUF_LEN, (FILE*)file);
    fgets(tmp, BUF_LEN, (FILE*)file);
    if (tmp[0] == '#')
      fgets(tmp, BUF_LEN, (FILE*)file);
    sscanf(tmp, "%d%d", &width, &height);
    fgets(tmp, BUF_LEN, (FILE*)file);

    // read data
    image.resize(width*height);
    vector<char>::iterator I=image.begin();

    for(int i=0; i<width*height; i++)
    {
      fscanf(file, "%d", &val);
      *I++ = char(val);
    }
  }
  fclose(file);
  return true;
}

//***************************************************************************
/** SavePGM.
 ****************************************************************************
 * This method saves an image to a specified file in portable grey map format
 * (PGM).
 *
 * \param    filename Name of the PGM file to be saved.
 * \param    width Width of the image in pixels.
 * \param    height Height of the image in pixels.
 * \return   true in case of success, false otherwise.
 * \see      LoadPGM
 * \version  $Revision 1.0 $
 * \author   Ingo Ahrns (EADS Space Transportation)
 ***************************************************************************/
bool SavePGM(string filename,int width,int height,vector<char> &image)
{
  const char *commentary = "Demo Image";
  FILE *file=fopen(filename.c_str(), "wb");

  if (file==NULL)
    return false;

  if (fprintf(file, "P5\n#%s\n%d %d\n%d\n", commentary, width, height, 255)>0)
  {
    vector<char>::iterator I=image.begin();
    if ((fwrite(&(*I), 1, width*height, file)) != (size_t)(width*height))
      return false;
  }
  else
    return false;

  fclose(file);
  return true;
}



bool SaveUndistortImagePGM(string filename,int width,int height, char* image)
{
  int size= width*height;
  const char *commentary = "Demo Image";
  FILE *file=fopen(filename.c_str(), "wb");

  if (file==NULL)
    return false;

  if (fprintf(file, "P5\n#%s\n%d %d\n%d\n", commentary, width, height, 255)>0)
  {

//   for(int i=0; i<size; i++)
   {
   fwrite(image, sizeof(char), size, file);
   }

  }
  else
    return false;

  fclose(file);
  return true;
}




//***************************************************************************
/** LoadIOR.
 ****************************************************************************
 * This function reads the camera calibration parameters from a very simple ascii
 * file defined by AICON. This file is called the internal orientation file (*.IOR).
 * For the usage in the EWM project a more convenient ini file will be used.
 *
 * \param    filename Name of the internal orientation file.
 * \param    param Structure for the camera parameters.
 * \return   true in case of success, false otherwise.
 * \version  $Revision 1.0 $
 * \author   Ingo Ahrns (EADS Space Transportation)
 ***************************************************************************/
bool LoadIOR(string filename,CameraParam &param)
{
  ifstream in(filename.c_str(),ios_base::in);

  int nr,dummy;
  in >> nr;
  in >> dummy;
  in >> param.focal;
  in >> param.x_principal;
  in >> param.y_principal;
  in >> param.A1;
  in >> param.A2;
  in >> param.R0;
  in >> param.A3;
  in >> param.B1;
  in >> param.B2;
  in >> param.C1;
  in >> param.C2;
  in >> param.ccd_width;
  in >> param.ccd_height;
  in >> param.img_width;
  in >> param.img_height;

  in.close();
  return true;
}

//***************************************************************************
/** ConvertPixelToAicon.
 ****************************************************************************
 * This function converts a pixel (x,y) given in standard image coordinates
 * with origin at leftmost topmost image corner and x axis from left to right
 * and y axis from top to bottom to a point in AICON's CCD coordinate system.
 * AICON's CCD coordinate system has its origin in the image center and the
 * x axis goes from left to right and the y axis from bottom to top.
 * AICON's coordinates are measured in mm.
 *
 * \param    x Reference to the x coordinate (Input: Standard image coordinates, Output: AICON's CCD coordinate system in mm)
 * \param    y Reference to the y coordinate (Input: Standard image coordinates, Output: AICON's CCD coordinate system in mm)
 * \param    param Structure containing all camera parameters.
 * \param    shift Boolean that enables the shift of the principal point to the image center.
 * \return   Nothing
 * \see      ConvertAiconToPixel
 * \version  $Revision 1.0 $
 * \author   Ingo Ahrns (EADS Space Transportation)
 ***************************************************************************/
void ConvertPixelToAicon(double &x,double &y, cameraInfo *c, bool shift)
{

  double deltax     = c->DeltaX;
  double deltay     = c->DeltaY;
  double x0         = c->CCDWidth/2.0;
  double y0         = c->CCDHeight/2.0;
  double pixel_x_mm = c->CCDWidth / c->ImageWidth;
  double pixel_y_mm = c->CCDHeight / c->ImageHeight;

  if (shift)
  {
    x =   x * pixel_x_mm - x0;
    y = -(y * pixel_y_mm - y0);
  }
  else
  {
    x =  (x * pixel_x_mm - x0) + deltax;
    y = -(y * pixel_y_mm - y0) + deltay;
  }
}

//***************************************************************************
/** ConvertAiconToPixel.
 ****************************************************************************
 * This function converts a pixel (x,y) given in AICON's CCD coordinate system to
 * standard image coordinates with origin at leftmost topmost image corner and x
 * axis from left to right and y axis from top to bottom to a point.
 * AICON's CCD coordinate system has its origin in the image center and the
 * x axis goes from left to right and the y axis from bottom to top.
 * AICON's coordinates are measured in mm.
 *
 * \param    x Reference to the x coordinate (Input: AICON's CCD coordinate system in mm, Output: Standard image coordinates)
 * \param    y Reference to the y coordinate (Input: AICON's CCD coordinate system in mm, Output: Standard image coordinates)
 * \param    param Structure containing all camera parameters.
 * \param    shift Boolean that enables the shift of the principal point to the image center.
 * \return   Nothing
 * \see      ConvertPixelToAicon
 * \version  $Revision 1.0 $
 * \author   Ingo Ahrns (EADS Space Transportation)
 ***************************************************************************/
void ConvertAiconToPixel(double &x, double &y, cameraInfo *c, bool shift)
{

  double deltax     = c->DeltaX;
  double deltay     = c->DeltaY;
  double x0         = c->CCDWidth/2.0;
  double y0         = c->CCDHeight/2.0;
  double pixel_x_mm = c->CCDWidth / c->ImageWidth;
  double pixel_y_mm = c->CCDHeight / c->ImageHeight;



  if (shift)
  {
    x = ( x + x0) / pixel_x_mm;
    y = (-y + y0) / pixel_y_mm;
  }
  else
  {
    x = ( (x - deltax) + x0) / pixel_x_mm;
    y = (-(y - deltay) + y0) / pixel_y_mm;
  }
}

//***************************************************************************
/** DistortAICONPoint.
 ****************************************************************************
 * This function applies the AICON's distortion mapping to a point given
 * in AICON's CCD coordinate system [mm].
 *
 * \param    x Reference to the x coordinate of the the pixel before (input) and after (output) distortion.
 * \param    y Reference to the y coordinate of the the pixel before (input) and after (output) distortion.
 * \param    param Structure containing all camera parameters.
 * \param    shift Boolean that enables the shift of the principal point to the image center.
 * \return   Nothing
 * \see      UndistortAICONPoint
 * \version  $Revision 1.0 $
 * \author   Ingo Ahrns (EADS Space Transportation)
 ***************************************************************************/
void DistortAICONPoint(double &x, double&y, cameraInfo *c, bool shift)
{
  double r2,r4,r6,r02,r04,r06,d0,d1,d2,d3;

  double xh = c->DeltaX;
  double yh = c->DeltaY;
  double r0 = c->R0;
  double a1 = c->A1;
  double a2 = c->A2;
  double a3 = c->A3;
  double b1 = c->B1;
  double b2 = c->B2;
  double c1 = c->C1;
  double c2 = c->C2;

  d2 = 0.;
  d3 = 0.;

  r2  = x * x + y * y;
  r4  = r2  * r2;
  r6  = r4  * r2;
  r02 = r0  * r0;
  r04 = r02 * r02;
  r06 = r04 * r02;

  d0 =  a1 * (r2 - r02) + a2 * (r4 - r04) + a3 * (r6 - r06);

  d1 =  c1 * x + c2 * y;

  d2 = b1 * (r2 + 2. * x * x) + b2 * 2. * x * y;
  d3 = b2 * (r2 + 2. * y * y) + b1 * 2. * x * y;

  if (shift)
  {
    x = x + xh + d0 * x + d1 + d2;
    y = y + yh + d0 * y + d3;
  }
  else
  {
    x = x + d0 * x + d1 + d2;
    y = y + d0 * y + d3;
  }
}

//***************************************************************************
/** UndistortAICONPoint.
 ****************************************************************************
 * This function applies the inverse of AICON's distortion mapping to a point given
 * in AICON's CCD coordinate system [mm].
 *
 * \param    x Reference to the x coordinate of the the pixel before (input) and after (output) undistortion.
 * \param    y Reference to the y coordinate of the the pixel before (input) and after (output) undistortion.
 * \param    param Structure containing all camera parameters.
 * \param    shift Boolean that enables the shift of the principal point to the image center.
 * \return   Nothing
 * \see      DistortAICONPoint
 * \version  $Revision 1.0 $
 * \author   Ingo Ahrns (EADS Space Transportation)
 ***************************************************************************/
void UndistortAICONPoint(double &x, double&y, cameraInfo *c, bool shift)
{
  double r2,r4,r6,r02,r04,r06,d0,d1,d2,d3;
  double hx,hy,hxalt;

  double xh = c->DeltaX;
  double yh = c->DeltaY;
  double r0 = c->R0;
  double a1 = c->A1;
  double a2 = c->A2;
  double a3 = c->A3;
  double b1 = c->B1;
  double b2 = c->B2;
  double c1 = c->C1;
  double c2 = c->C2;


  d2 = 0.;
  d3 = 0.;
  if (shift)
  {
    hx = x - xh;
    hy = y - yh;
  }
  else
  {
    hx = x;
    hy = y;
  }
  hxalt=0.;

  for(;;)
  {
    r2  = hx * hx + hy * hy;
    r4  = r2  * r2;
    r6  = r4  * r2;
    r02 = r0  * r0;
    r04 = r02 * r02;
    r06 = r04 * r02;
    d0  = a1 * (r2 - r02) + a2 * (r4 - r04) + a3 * (r6 - r06);

    d1 =  c1 * hx + c2 * hy;

    d2 = b1 * (r2 + 2.0*hx*hx) + b2 * 2.0*hx*hy;
    d3 = b2 * (r2 + 2.0*hy*hy) + b1 * 2.0*hx*hy;

    hx = x - xh - d0 * hx - d1 - d2;
    hy = y - yh - d0 * hy - d3;
    if(fabs(hxalt-hx) < .000001)
      break;
    hxalt = hx;
  }
  if (shift)
  {
    x = x - xh - d0 * hx - d1 - d2;
    y = y - yh - d0 * hy - d3;
  }
  else
  {
    x = x - d0 * hx - d1 - d2;
    y = y - d0 * hy - d3;
  }
}


//camerInfo *cams
void UndistortedImageTable(cameraInfo *c, bool shift)
{
 int width = c->ImageWidth;
 int height = c->ImageHeight;


  for (int y=0; y<height; y++)
  {
    for (int x=0; x<width; x++)
    {
      double xp = x;
      double yp = y;
      ConvertPixelToAicon(xp,yp,c,shift);
      DistortAICONPoint(xp,yp,c,shift);
      ConvertAiconToPixel(xp,yp,c,shift);
      c->tablex[x+y*width] = (int)(xp+0.5); // store xi
      c->tabley[x+y*width] = (int)(yp+0.5); // store yi

    }
  }
 }



//***************************************************************************
/** UndistortedImageAICON
 ****************************************************************************
 * This function performs the undistortion (rectification) of an image
 * based on AICON's internal camera orientation parameters. This undistortion is
 * performed by applying the DistortAICONPoint function, i.e. it performs a
 * backward directed mapping. This method is faster and does not suffer from
 * producing small gaps and holes in the undistorted image due to a missing
 * surjection property of the mapping.
 *
 * \param    camerInfo struct where are store all AICON parameters and other info
 * \param    oriimg Original (distorted) image.
 * \param    rectimg Rectified (undistorted) image.
 * \param    shift Boolean that enables the shift of the principal point to the image center.
 * \return   Nothing
 * \see      UndistortImage2.
 * \version  $Revision 2.0 $
 * \author   Ingo Ahrns (EADS Space Transportation) modified by P.Lanza
 ***************************************************************************/

void UndistortedImageAICON(cameraInfo *c, bool filled, CvScalar color )
{
 int width = c->ImageWidth;
 int height = c->ImageHeight;
 int step      = c->rawimg.step;
 int channels  = c->rawimg.channels();

 //data      = (uchar *)img->imageData;
 uchar* oriimg =  (uchar *)c->rawimg.data;
 uchar* rectimg = (uchar *)c->unimg.data;


  for (int y=0;y<height;y++)
  {
    for (int x=0;x<width;x++)
    {
      int xi = c->tablex[x+y*width];
      int yi = c->tabley[x+y*width];
      if (   (xi >= 0) && (xi < width)
          && (yi >= 0) && (yi < height))
      {
       //rectimg[x+y*width] = oriimg[xi+yi*width];// original
		   rectimg[x*channels+y*step+0] = oriimg[xi*channels+yi*step+0];
		   rectimg[x*channels+y*step+1] = oriimg[xi*channels+yi*step+1];
		   rectimg[x*channels+y*step+2] = oriimg[xi*channels+yi*step+2];
      }
      else
      {
        if (filled==false)
        {
			rectimg[x*channels+y*step+0] = (unsigned char) color.val[0];
			rectimg[x*channels+y*step+1] = (unsigned char) color.val[1];
			rectimg[x*channels+y*step+2] = (unsigned char) color.val[2];
        }
        else{
        //rectimg[x+y*width] = oriimg[x+y*width];
		   rectimg[x*channels+y*step+0] = oriimg[x*channels+y*step+0];
		   rectimg[x*channels+y*step+1] = oriimg[x*channels+y*step+1];
		   rectimg[x*channels+y*step+2] = oriimg[x*channels+y*step+2];
        }
      }
    }
  }
}




//read the camera calibration parmeter from the file specified as input
void ReadCalibrationCamera (std::string calibrationCameraFile, cameraInfo *c) {

/* excerpt from camera calibration ini file: CAM_FOOT_AIR_CALIB.ini

[CAMERA PARAMETERS]
Name Camera = CAM_FOOT                                        ; Name of the camera
Description = Leg_camera                                      ; Textual description for the camera
CCD Width = 4.7616                                            ; Width of the CCD chip in mm
CCD Height = 3.5712                                           ; Heigth of the CCD chip in mm
Image Width = 1024                                            ; image width in pixels
Image Height = 768                                            ; height width in pixels
Delta X = -0.0488778                                          ; Shift of the principal point in x direction from the image center (AICON coordinates in mm)
Delta Y = -0.00398443                                         ; Shift of the principal point in y direction from the image center (AICON coordinates in mm)
Focal Length = -4.06169                                       ; Focal length in mm (negative value)
A1 = -0.00957641                                              ; Camera distortion parameter A1
A2 = 0.00085977                                               ; Camera distortion parameter A2
A3 = -1.75496e-05                                             ; Camera distortion parameter A3
B1 = -0.000121614                                             ; Camera distortion parameter B1
B2 = 5.66236e-05                                              ; Camera distortion parameter B2
C1 = 0.000571254                                              ; Camera distortion parameter C1
C2 = -0.000673509                                             ; Camera distortion parameter C2
R0 = 1.98                                                     ; Zero distortion parameter R0

[HAND CAMERA TRANSFORM]
Name = Leg                                                    ; Name of the actuator, e.g. HEAD, LEFTARM, etc.
Hand Camera Roll = 0                                          ; Roll angle of the transform EE -> Camera [°]
Hand Camera Pitch = 0                                         ; Pitch angle of the transform EE -> Camera [°]
Hand Camera Yaw = 0                                           ; Yaw angle of the transform EE -> Camera [°]
Hand Camera X = 0                                             ; X component of the transform EE -> Camera [m]
Hand Camera Y = 0                                             ; Y component of the transform EE -> Camera [m]
Hand Camera Z = 0                                             ; Z component of the transform EE -> Camera [m]

*/


  int Err = 0;
  string temp;

  if (VERBOSE) {
    cout<<"Debug -- ReadCalibrationCamera () input file: "<<calibrationCameraFile<<endl;
  };

  ReadFileIni R(calibrationCameraFile);
  if ( VERBOSE) { cout<<"ReadFileINI initialisated!! "<<endl; }
  Err = R.getStringValErr("Name Camera",  c->NameCamera,     "[CAMERA PARAMETERS]");
  Err = R.getStringValErr("Description",  c->Description,    "[CAMERA PARAMETERS]");
  Err = R.getDoubleValErr("CCD Width",    c->CCDWidth,       "[CAMERA PARAMETERS]");
  Err = R.getDoubleValErr("CCD Height",   c->CCDHeight,      "[CAMERA PARAMETERS]");
  Err = R.getIntValErr("Image Width",     c->ImageWidth,     "[CAMERA PARAMETERS]");
  Err = R.getIntValErr("Image Height",    c->ImageHeight,    "[CAMERA PARAMETERS]");
  Err = R.getDoubleValErr("Delta X",      c->DeltaX,         "[CAMERA PARAMETERS]");
  Err = R.getDoubleValErr("Delta Y",      c->DeltaY,         "[CAMERA PARAMETERS]");
  Err = R.getDoubleValErr("Focal Length", c->FocalLength,    "[CAMERA PARAMETERS]");
  Err = R.getDoubleValErr("A1",           c->A1,             "[CAMERA PARAMETERS]");
  Err = R.getDoubleValErr("A2",           c->A2,             "[CAMERA PARAMETERS]");
  Err = R.getDoubleValErr("A3",           c->A3,             "[CAMERA PARAMETERS]");
  Err = R.getDoubleValErr("B1",           c->B1,             "[CAMERA PARAMETERS]");
  Err = R.getDoubleValErr("B2",           c->B2,             "[CAMERA PARAMETERS]");
  Err = R.getDoubleValErr("C1",           c->C1,             "[CAMERA PARAMETERS]");
  Err = R.getDoubleValErr("C2",           c->C2,             "[CAMERA PARAMETERS]");
  Err = R.getDoubleValErr("R0",           c->R0,             "[CAMERA PARAMETERS]");

  if ( VERBOSE) { cout<<"Error value 1 "<<Err<<endl; }


  Err = R.getStringValErr("Name",              c->Name,            "[HAND CAMERA TRANSFORM]");
  Err = R.getDoubleValErr("Hand Camera Roll",  c->HandCameraRoll,  "[HAND CAMERA TRANSFORM]");
  Err = R.getDoubleValErr("Hand Camera Pitch", c->HandCameraPitch, "[HAND CAMERA TRANSFORM]");
  Err = R.getDoubleValErr("Hand Camera Yaw",   c->HandCameraYaw,   "[HAND CAMERA TRANSFORM]");
  Err = R.getDoubleValErr("Hand Camera X",     c->HandCameraX,     "[HAND CAMERA TRANSFORM]");
  Err = R.getDoubleValErr("Hand Camera Y",     c->HandCameraY,     "[HAND CAMERA TRANSFORM]");
  Err = R.getDoubleValErr("Hand Camera Z",     c->HandCameraZ,     "[HAND CAMERA TRANSFORM]");

  if ( VERBOSE) { cout<<"Error value 2 "<<Err<<endl; }

  if (VERBOSE) {
    // print all values reading from the *.ini file
    cout<<"Name Camera:"<<  c->NameCamera     <<endl;
    cout<<"Description: "<<  c->Description    <<endl;
    cout<<"CCD Width: "<<    c->CCDWidth       <<endl;
    cout<<"CCD Height: "<<   c->CCDHeight      <<endl;
    cout<<"Image Width: "<<     c->ImageWidth  <<endl;
    cout<<"Image Height: "<<    c->ImageHeight <<endl;
    cout<<"Delta X: "<<      c->DeltaX         <<endl;
    cout<<"Delta Y: "<<      c->DeltaY         <<endl;
    cout<<"Focal Length: "<< c->FocalLength    <<endl;
    cout<<"A1: "<<           c->A1             <<endl;
    cout<<"A2: "<<           c->A2             <<endl;
    cout<<"A3: "<<           c->A3             <<endl;
    cout<<"B1: "<<           c->B1             <<endl;
    cout<<"B2: "<<           c->B2             <<endl;
    cout<<"C1: "<<           c->C1             <<endl;
    cout<<"C2: "<<           c->C2             <<endl;
    cout<<"R0: "<<           c->R0             <<endl;

    cout<<"Name"<<              c->Name            <<endl;
    cout<<"Hand Camera Roll: "<<  c->HandCameraRoll  <<endl;
    cout<<"Hand Camera Pitch: "<< c->HandCameraPitch <<endl;
    cout<<"Hand Camera Yaw: "<<   c->HandCameraYaw   <<endl;
    cout<<"Hand Camera X: "<<     c->HandCameraX     <<endl;
    cout<<"Hand Camera Y: "<<     c->HandCameraY     <<endl;
    cout<<"Hand Camera Z: "<<     c->HandCameraZ     <<endl;



  }

// these initialisation have moved in the cameraInfo.cpp
 c->size = c->ImageWidth * c->ImageHeight;
 c->tablex = new int[c->size];
 c->tabley = new int[c->size];





} // end ReadCalibrationCamera



int UndistortedImageAICONTest(cameraInfo *c) {



  int width,height;
  int size;
  CameraParam  param;
  char* original;
  char* rectified;
  int* tablex;
  int* tabley;
  //int table
//  Clock C;


  if (VERBOSE) { cout<<"Undistorted image via AICON camera calibration"<<endl; }
  //C.start();

  // read calibration camera parameter
  param.x_principal = c->DeltaX;
  param.y_principal = c->DeltaY;
  param.focal = c->FocalLength;
  param.R0 = c->R0;
  param.A1 = c->A1;
  param.A2 = c->A2;
  param.A3 = c->A3;
  param.B1 = c->B1;
  param.B2 = c->B2;
  param.C1 = c->C1;
  param.C2 = c->C2;
  param.ccd_width = c->CCDWidth;
  param.ccd_height = c->CCDHeight;
  param.img_width = c->ImageWidth;
  param.img_height = c->ImageHeight;

  width = param.img_width;
  height = param.img_height;
  size = width*height;

  cout<<"size: "<<size<<endl;
  cout<<"width: "<<width<<endl;
  cout<<"height: "<<height<<endl;

  original = new char[size];
  rectified = new char[size];
  tablex = new int[size];
  tabley = new int[size];


    for (int i=0; i<size; ++i)
    {
//        original[i] = c->rawimg[i]; // with STL 47 ms wihout STL 0 ms!!
    }


    //C.end();

    if (DEBUG) {
        // only for test scope ...
        cout<<"image loaded!"<<endl;
        for (int i=0; i<100; ++i)
        {
            cout<<"i: "<<i<<"   "<<original[i]<<endl;
        }
     }

  if (VERBOSE)
     {
//         cout<<"Time employed to copy rawimg: "<<C.elapsedTime()<<" ms"<<endl;
     }

  //C.start();
  // integral function
  //UndistortImage1(param,width,height,original,rectified,false);
  //UndistortImageTable(param,width,height,tablex,tabley,false);

  cout<<" Undistorted Table end calculation"<<endl;
  //C.start();
  //UndistortImage(width,height,tablex,tabley,original,rectified,false);
  cout<<" Undistorted image end calculation"<<endl;
  //C.end();
  if (VERBOSE)
     {
//         cout<<"Time employed to rectify the input image: "<<C.elapsedTime()<<" ms"<<endl;
     }

    for (int i=0; i<size; ++i)
    {
//        c->unimg[i] = rectified[i]; // with STL 47 ms wihout STL 0 ms!!
    }



  // this function save the image if it is necessary
  //C.start();
  SaveUndistortImagePGM("rectified.pgm",width,height,rectified);
  //C.end();
  if (VERBOSE)
     {
//         cout<<"Time employed to save the rectified image: "<<C.elapsedTime()<<" ms"<<endl;
     }

cout<<" end calculation"<<endl;
  return 0;

};


