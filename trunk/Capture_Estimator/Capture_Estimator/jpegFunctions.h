/*---------------------------------------------------------------------------
#Program.......: jpegFunctions.h
#File..........: jpegExample.h
#Purpose.......: Write a jpeg functions in charge to read/write a file in accordance with jpeg standard
#Author........: P. Lanza
#Modified......: P. Lanza
#Created.......: 25/02/2009
#Last Changed..: 25/02/2009
#Version.......: 01.01
#Copyright.....:
#License.......:
#ToDo..........:
#Note..........:


This version is able to save and load ASCII plain comments
into the jpeg format 


*/
#pragma once


#include "jmorecfg.h"
#include "cv.h"
#include <iostream>

#include <fstream>
#include <sstream>

//----------------------------------------------------------------------------//
//                    writeJpegFile                                           //
//----------------------------------------------------------------------------//
// this function write on file a jpeg image. The parameters are the following:
// filename is the name of the file
// img is the input image ( CvImage class)
// quality is form 1 to 100 It represents the quality expressed in per cent
// string is an optional variable where it is possible to store plain text up to 65533 characters
// Please note that the text format is into plain text without any non printable characters

void writeJpegFile (char * filename, CvImage& img, std::string options= "quality=95", std::string remarks= "");

//----------------------------------------------------------------------------//
//                    writeJpegMem                                           //
//----------------------------------------------------------------------------//
// this function write on a memory buffer already existing a jpeg image. The parameters are the following:
// outBuffer the unsigned char* buffer already existing
// jpegSize returns the size of jpeg file stored into the outBuffer
// img is the input image ( CvImage class)
// quality is form 1 to 100 It rapresents the quality expressed in percentual
// string is an optional variable where it is possible to store plain text up to 65533 characters
// Please note that the text format is into plain text without any non printable characters
// As rule of thumb an outBuffer which dimension is close to 1 Mbytes is generally sufficient 
// for all jpeg compressed images.

void writeJpegMem (unsigned char * outBuffer, int& jpegSize, CvImage& img,  std::string options= "quality=95", std::string remarks= "");


//----------------------------------------------------------------------------//
//                    readJpegFile                                            //
//----------------------------------------------------------------------------//
// This function read the Jpeg file by means of its file name.The parameters are the following:
// filename is the name of the jpeg file
// comment is an optional variable where it is possible to store plain text up to 65533 characters
// Please note that the text format is into plain text without any non printable characters
// This function loads the image into CvImage class 

IplImage* readJpegFile (char * filename, std::string &comment);



//----------------------------------------------------------------------------//
//                    readJpegMeme                                            //
//----------------------------------------------------------------------------//
// This function read the Jpeg file in buffer. The parameters are the following:
// inBuffer the jpeg file
// comment is an optional variable where it is possible to store plain text up to 65533 characters
// Please note that the text format is into plain text without any non printable characters
// This function loads the image into CvImage class.

IplImage* readJpegMem (unsigned char* inBuffer, int jpegSize, std::string &comment);

int readJpegMem (CvImage& img, unsigned char* inBuffer, int jpegSize, std::string &comment);
