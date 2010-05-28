/*---------------------------------------------------------------------------
Program.......: ErrVisualizeApp.exe
File..........: ErrVisualize.cpp
Purpose.......: ErrVisualize class implementation
Author........: P. Lanza
Modified......: P. Lanza
Created.......: 30/08/2005
Last Changed..: 06/09/2005
Copyright.....:
License.......:


---------------------------------------------------------------------------*/

#include "ErrVisualize.h"
#include <iostream>
#include <list>     // STL list
#include <fstream>
#include <time.h>   // for time functions

#include <windows.h> // for GUI messages


using namespace std;

//Initialisation of the Singleton at the end of this file!
//With this initialisation we avoid a lot of troubles during the linking
// with others classes other ErrVisualizeTest.



ErrVisualize::ErrVisualize(bool val)  :
 debugInfo(val),
 timeInfo(true),
 decoratedInfo(true),
 emulatedInfo(false),
 GUIInfo(0),
 lasterrNum(0),
 lastnline(9999)  {
 lastMsgString=" ";
 outString=" ";
 lastnfile="no file name available";
 ltime=0;
 type='e';
 retValue=-1;
}




int ErrVisualize::msg( const std::string& input,  // message
            int errNum,               // number error
            const std::string& level,  // level specification: F, E, W, A M
            const std::string& nfile,  // typically filename from __FILE__
            const int nline) {          // typically line number from __LINE__


  ostringstream tmp; // local varible to build the string

  lastMsgString=input;
  // evaluate the input message in accordance with the decoratedInfo flag
  if (decoratedInfo) {// This means that the output string should be visualised on standard stream
    tmp<<"  ------------start error message-------------------"<<endl;
  }

  // evaluate the input message in accordance with the debugInfo flag
  if (debugInfo) {// I find <info> in the message received
    tmp<<input<<endl;
    type=level[0];
    type=tolower(type);
      switch (type){
       case 'f' : tmp<<"Fatal error in "; break;
       case 'e' : tmp<<"Error in "; break;
       case 'w' : tmp<<"Warning in "; break;
       case 'a' : tmp<<"Assertion in "; break;
       case 'm' : tmp<<"Message error in "; break;
       case 'i' : break;
       default  : tmp<<"Error in "; break;
      }
    tmp<<nfile<<" at line "<<nline;
  }


  else { tmp<<input;
  }
  tmp<<endl; // add a carriege return


  // evaluate the input message in accordance with the TimeInfo flag
  if (timeInfo) {// attach the time information
    time(&ltime);
    tmp<<ctime(&ltime)<<endl;
  }

  // re-evaluate the input message in accordance with the DecoratedInfo flag
  if (decoratedInfo) {// This means that the output string should be visualised on standard stream
    tmp<<"  ------------end error message---------------------"<<endl;
  }

  // evaluate the input message in accordance with the spec ( call spec)


  // call output for visualize the message
  outString=tmp.str();
  lasterrNum=errNum;
  lastnfile=nfile,
  lastnline=nline;
  retValue=out(); // call the output method (potrebe chiamarlo mettendo in evidenza il livello di errore
  return retValue;


}

int ErrVisualize::out() {

  switch (GUIInfo) {
  case 0:      cerr<<outString<<endl; break;
  case 1:      retValue=GUImsg(); break; // only GUI
  case 2:      cerr<<outString<<endl; retValue=GUImsg(); break;      //GUI and cerr
  default:     cerr<<outString<<endl; break; // only cerr
  }
  // now I call emulation() wheter the emulated flag is enabled.
  if (emulatedInfo) { emulation(); };


  return retValue; //
}


const string ErrVisualize::lastMsg() {return outString;} // output


const string ErrVisualize::moreDetails(const int details) {

  ostringstream tmp; // local varible to build the string

  // return a string to the caller as default message.


  tmp<<" No more information on the kind of error indicated ( number ";
  tmp<<details<<" ) are available now!";
  outString=tmp.str();

 return outString;  // return by value ( String is a little obj)
}

  // get and set functions relative to DebugInfo private variable

bool ErrVisualize::getDebugInfo() {return debugInfo; };
void ErrVisualize::setDebugInfo(const bool value) {debugInfo=value; };

// enable disable functions relative to DebugInfo

void ErrVisualize::enableDebugInfo() {setDebugInfo(true); };
void ErrVisualize::disableDebugInfo() {setDebugInfo(false); };

  // get and set functions relative to timeInfo private variable

bool ErrVisualize::getTimeInfo() {return timeInfo; };
void ErrVisualize::setTimeInfo(const bool value) {timeInfo=value; };

  // enable disable functions relative to TimeInfo

void ErrVisualize::enableTimeInfo() {setTimeInfo(true); }
void ErrVisualize::disableTimeInfo() {setTimeInfo(false); }

  // get and set functions relative to decoratedInfo private variable

bool ErrVisualize::getDecoratedInfo() {return decoratedInfo; };
void ErrVisualize::setDecoratedInfo(const bool value) {decoratedInfo=value; };

  // enable disable functions relative to DecoratedInfo

void ErrVisualize::enableDecoratedInfo() {setDecoratedInfo(true); }
void ErrVisualize::disableDecoratedInfo() {setDecoratedInfo(false); }

  // get and set functions relative to emulatedInfo private variable

bool ErrVisualize::getEmulatedInfo() {return emulatedInfo; };
void ErrVisualize::setEmulatedInfo(const bool value) {emulatedInfo=value; firstMsg=true; };

  // enable disable functions relative to DebugInfo

void ErrVisualize::enableEmulatedInfo() {setEmulatedInfo(true); }
void ErrVisualize::disableEmulatedInfo() {setEmulatedInfo(false); }


  // get and set functions relative to decoratedInfo private variable

int  ErrVisualize::getGUIInfo() {return GUIInfo; };
void ErrVisualize::setGUIInfo(const int value) {
  if ((value<0) || (value>=3)) { // there is an input error!!
  ostringstream tmp; // local varible to build the string
   tmp<<"The GUIInfo value shall be limited within 1 to 2 included values."<<endl;
   tmp<<"The input value is "<<value<<endl;
   msg( tmp.str(), 100, "E",  __FILE__, __LINE__ );
   //   ERROR(tmp.str(),100);
  }
  else {
    GUIInfo=value;
  }
};



int ErrVisualize::GUImsg() {  //it is called only by out() method
  // this function is enbled in this version of the ErrVisualize class.
  // all error messages are sent to cerr
  //
  //cerr<<outString<<endl;
  // verify that kind of error should be visualised:
  // evaluate the input message in accordance with the debugInfo flag

      int retValue = -1;
      switch (type){
       case 'f' : retValue = printf("OpenCV and IP library Fatal Error"); break;
       case 'e' : retValue = printf("OpenCV and IP library Fatal Error"); break;
       case 'w' : retValue = printf("OpenCV and IP library Warning"); break;
       case 'a' : retValue = printf("OpenCV and IP library Assert Condition Failed"); break;
       case 'm' : retValue = printf("OpenCV and IP library Message"); break;
       case 'i' : retValue = printf("OpenCV and IP library Information"); break;
       default:   retValue = printf("OpenCV and IP library Fatal Error"); break;
      }
      //f: retValue doesn't matter
//e: retValue doesn't matter
//f: retValue doesn't matter
//w: retValue doesn't matter
//a: retValue Abort Retry Ignore
//m: retValue Yes No Cancel
//i: retValue doesn't matter
//default: retValue doesn't matter
  return retValue;


 /* only for information
// all MessageBox function styles are avialbale on MSDN site


    MessageBox(NULL, "Oh my God, goodbye, cruel world!", "Pg Window", MB_OK);
    // Information
    MessageBox(NULL, "Icon question  OKCANCEL", "Error!",MB_ICONQUESTION | MB_OKCANCEL);
    MessageBox(NULL, "Icon asterisk YESNO", "Error!",MB_ICONASTERISK | MB_YESNO);
    MessageBox(NULL, "Icon asterisk YESNOCANCEL", "Error!",MB_ICONASTERISK | MB_YESNOCANCEL);
    MessageBox(NULL, "Window Registration Failed!", "Error!",MB_ICONEXCLAMATION | MB_OK);
    MessageBox(NULL, "Window Registration Failed!", "Error!",MB_ICONINFORMATION | MB_OK);
    MessageBox(NULL, "Window Abort Retry Ignore...", "Error!",MB_ABORTRETRYIGNORE | MB_OK);
    // for Errors
    MessageBox(NULL, "Icon Error", "Error!",MB_ICONERROR | MB_OK );


*/




}



void ErrVisualize::emulation() { //it is called only by out() method

  static list<int> errNumList;
  list<int>::iterator pos;
  ofstream outf;
  static int nummsg=0;



  if (firstMsg) { //that it is the first Msg to process as Emulation
     time_t timeloc; // local variable

    // the flag is updated:
    firstMsg=false;
    // clear the list container
    errNumList.erase(errNumList.begin(), errNumList.end());
    // insert the first element into the list
    errNumList.push_back(lasterrNum);
    // Open a file stream where to store all messages
    outf.open("ErrorMsgLog.txt", ios::out);
    // ASSERTFILE(outf,"ErrorMsgLog.txt");
     time(&timeloc); // local variable
     outf<<endl<<endl<<"New session of Error Messages Log file "<<endl;
     outf<<ctime(&ltime)<<endl<<endl;
     nummsg=1;

  }
  else {
    outf.open("ErrorMsgLog.txt", ios::out| ios::app);
    // search if another element with the same number is already existing

	//Il find() non esiste...
    //pos = find(errNumList.begin(), errNumList.end(), lasterrNum);
    //if (pos==errNumList.end()) { errNumList.push_back(lasterrNum);
    //}
    //else { // error message
    //  // write a message into the file stream
    //  // re open the file stream each time for more security
    //  //      outf.open("ErrorMsgLog.txt", ios::out| ios::app);
    //  //   assertFile(outf,"ErrorMsgLog.txt");

    //  outf<<"$$ERROR MESSAGE LOG FILE: equal number error: "<<lasterrNum<<endl;
    //}
	//Il find() non esiste...
  }
  // store the last Msg read
  outf<<endl;
  outf<<"message number: "<<nummsg++<<endl;
  outf<<"Error number: "<<lasterrNum<<endl;
  outf<<"File: "<<lastnfile<<endl;
  outf<<"Line: "<<lastnline<<endl;
  outf<<"visualized message: "<<endl;
  outf<<outString;
  outf.close();
}


int ErrVisualize::retVal()
{
  return retValue;
}


// This instruction is relevant to the intilisation of the ErrVisualize singleton.
// it is necessary to initilise the singleton first to use it
ErrVisualize ErrVisualize::Err(true);  // instatiation of the singleton! DebugInfo=true: enable all Debug info

