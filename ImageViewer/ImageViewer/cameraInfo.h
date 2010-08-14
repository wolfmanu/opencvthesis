#pragma once
/*----------------------------------------------------------------------------
  Include file..: regist.h
  Created.......: 18/05/2005
  Last Changed..: 04/11/2005

  define data structures:
    camerInfo, camera_pose, and registrator
  include files for modules:
    caminfo.c, imseg.c, arturo.c, grabfile.c, wrlstuff.c, and cipreg.c

-----------------------------------------------------------------------------*/
#ifndef _REGIST_H_DEFINED
#define _REGIST_H_DEFINED


#include <string> // C++ string class
#include <cv.h>
#include <cvaux.h>
#include <iostream>


class cameraInfo {
 public:

  // camera parameters in accordance with EADS ini file and AICON calibration
  // header in ini file: [CAMERA PARAMETERS]
  std::string NameCamera;                                  // Name of the camera
  std::string Description;                                  // Textual description for the camera
  double CCDWidth;                                          // Width of the CCD chip in mm
  double CCDHeight;                                         // Heigth of the CCD chip in mm
  int ImageWidth;                                           // image width in pixels
  int ImageHeight;                                          // height width in pixels
  double DeltaX;                                            // Shift of the principal point in x direction from the image center (AICON coordinates in mm)
  double DeltaY;                                            //  Shift of the principal point in y direction from the image center (AICON coordinates in mm)
  double FocalLength;                                       // Focal length in mm (negative value)
  double A1;                                                // Camera distortion parameter A1
  double A2;                                                // Camera distortion parameter A2
  double A3;                                                // Camera distortion parameter A3
  double B1;                                                // Camera distortion parameter B1
  double B2;                                                // Camera distortion parameter B2
  double C1;                                                // Camera distortion parameter C1
  double C2;                                                // Camera distortion parameter C2
  double R0;                                                // Camera distortion parameter R0
  // header in ini file: [HAND CAMERA TRANSFORM]
  std::string Name;                                         // Name of the actuator, e.g. HEAD, LEFTARM, etc.
  double HandCameraRoll;                                    // Roll angle of the transform EE -> Camera [°]
  double HandCameraPitch;                                   // Pitch angle of the transform EE -> Camera [°]
  double HandCameraYaw;                                     // Yaw angle of the transform EE -> Camera [°]
  double HandCameraX;                                       // X component of the transform EE -> Camera [m]
  double HandCameraY;                                       // Y component of the transform EE -> Camera [m]
  double HandCameraZ;                                       // Z component of the transform EE -> Camera [m]

  // end of Camera Calibration File
  int number;           // 0..
  int CAM_ID;
  double rtpose[6];     // pose (relative to camera's common ref, e.g. cam0) Note: pose is [r,t]
        // redundant info
  double R[9], t[3];
  double M[12];
  double K[6], sfocal;
  int W,H;
  /// memory size of the image
  int size;
  // image data
  int hasimage;

  // pre evaluated for fast calibration
  int *tablex; // pre evaluated coefficients for fast calibration in x axis
  int *tabley; // pre evaluated coefficients for fast calibration in y axis

  CvImage rawimg;//IplImage* rawimg;
	CvImage unimg;//IplImage* unimg;
//  unsigned char *rawimg;// raw image acquired
//  unsigned char *unimg;	// undistorted, gain corrected image


  //public ctor
  cameraInfo();
  cameraInfo(CvImage src);

  //methods:
  void loadImage(CvImage src);


  //public dtor
  ~cameraInfo();


};



#endif
