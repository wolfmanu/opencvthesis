#pragma once

#include "cameraInfo.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;


/***************************************************************************/
/** CameraParam.

  This is the structure that contains the main camera calibration parameters.
  These are:

   <ul>
     <li> The image width in pixels \f$w_{img}\f$.
     <li> The image height in pixels \f$h_{img}\f$
     <li> Focal length \f$f\f$ in mm (negative value).
     <li> Delta value \f$\Delta x\f$ for the shift of the principal point [mm] in x direction.
          X axis is from left to right with origin in the image center.
     <li> Delta value \f$\Delta y\f$ for the shift of the principal point [mm] in y direction.
          Y axis is from bottom to top with origin in the image center.
     <li> Distortion parameter \f$a_1\f$
     <li> Distortion parameter \f$a_2\f$
     <li> Distortion parameter \f$a_3\f$
     <li> Distortion parameter \f$b_1\f$
     <li> Distortion parameter \f$b_2\f$
     <li> Distortion parameter \f$c_1\f$
     <li> Distortion parameter \f$c_2\f$
     <li> Distortion parameter \f$r_0\f$
     <li> CCD chip width \f$w_{ccd}\f$ in mm
     <li> CCD chip height \f$h_{ccd}\f$ in mm
   </ul>

  \version  \$Revision: 1.0 $
  \author   Ingo Ahrns (Ingo.Ahrns@space.eads.net)
****************************************************************************/

struct CameraParam
{
  /// X coordinate of the principal point in AICON's CCD coordinates in mm
  double x_principal;
  /// Y coordinate of the principal point in AICON's CCD coordinates in mm
  double y_principal;
  /// Focal length in mm (negative value).
  double focal;
  /// Distortion parameter r0
  double R0;
  /// Distortion parameter A1
  double A1;
  /// Distortion parameter A2
  double A2;
  /// Distortion parameter A3
  double A3;
  /// Distortion parameter B1
  double B1;
  /// Distortion parameter B2
  double B2;
  /// Distortion parameter C1
  double C1;
  /// Distortion parameter C2
  double C2;
  /// Width of the CCD chip in mm
  double ccd_width;
  /// Height of the CCD chip in mm
  double ccd_height;
  /// Width of the image in pixels
  int    img_width;
  /// Height of the image in pixels
  int    img_height;
};


//read the camera calibration parameter from the file specified as input
void ReadCalibrationCamera (std::string calibrationCameraFile, cameraInfo *c);

//pre evaluated calibration coefficients for fast routine on image calibration
void UndistortedImageTable(cameraInfo *c, bool shift = false);

// apply the camera calibration
// filled parameter means that the image is
void UndistortedImageAICON(cameraInfo *c, bool filled=true, CvScalar color=CV_RGB(255,0,0) );

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
bool LoadPGM(string filename, int &width, int &height, vector<char> &image);

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
bool SavePGM(string filename,int width,int height,vector<char> &image);

bool SaveUndistortImagePGM(string filename,int width,int height, char* image);

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
bool LoadIOR(string filename,CameraParam &param);

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
void ConvertPixelToAicon(double &x,double &y, cameraInfo *c, bool shift=true);

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
void ConvertAiconToPixel(double &x, double &y, cameraInfo *c, bool shift = true);

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
void DistortAICONPoint(double &x, double&y, cameraInfo *c, bool shift=true);

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
void UndistortAICONPoint(double &x, double&y, cameraInfo *c, bool shift=true);

int UndistortedImageAICONTest(cameraInfo *c);

