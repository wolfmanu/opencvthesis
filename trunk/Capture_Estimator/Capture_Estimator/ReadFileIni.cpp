#/*---------------------------------------------------------------------------
#Program.......: ReadFileIniApp
#File..........: ReadFileIni.cpp
#Purpose.......: implementation of the ReadFileIni class.
#Author........: P. Lanza
#Modified......: P. Lanza
#Created.......: 07/09/2005
#Last Changed..: 17/03/2006
#Version.......: 01.00
#Copyright.....:
#License.......:
#ToDo..........:
#Note..........:
#---------------------------------------------------------------------------*/


#include "ReadFileIni.h"
#include "Assertion.h"
#include "ErrVisualize.h"
#include <fstream>
#include <sstream>
#include <string>

#include <iostream>

using namespace std;



ReadFileIni::ReadFileIni() { // default constructor
  fileName="no filename";
  ss.str(""); //I clear the streambuffer associated.
}


ReadFileIni::ReadFileIni( const std::string& input, const std::string& comment){ //! constructor that opens the file stream associated


  fileName = "no filename"; // as default value
  ss.str(""); //I clear the streambuffer associated.
  // I assign the input name to the ifstream
  in.open(input.c_str());
  // I try to verify that it is possible open this stream
  if(ASSERTFILE(in,input)) {
   // In this case ASSERTFILE manages all the errors.
   // The file exists and it was correctly opened!
   // I can assign the fileName correctly.
  fileName = input;
  // now I can opens it
   string line;
   size_t posString;
   s = ""; // clear the private string s.
   pos=0; // clear also the iterator
   while (getline(in,line)) {
     // I strips the comment line that begins with a '#' or '//' or other characters as
     // indicated in 'comment' variable

     // verify that 'comment' is not included within a string variable
     posString = line.rfind('"');
     pos = line.rfind(comment.c_str());
     if ( (pos<posString) & (posString != string::npos) ) {
       s += line+ '\n';
     }
     else {
      if (pos != string::npos) { // there is a comment
        if (pos != 0) {
          line = line.substr(0,pos);
          s += line + '\n';
        } // otherwise skip the whole line
      }
      else s += line+ '\n';
     };
   };
   ss<<s;
  }
  in.close();
}


string  ReadFileIni::getfileName() { //! it returns the file name associated to the input stream
  return fileName; // returned by value because it is a small class.
}


string  ReadFileIni::setOpenfileName(const std::string& input, const std::string& comment) { //! it sets the file name and open the stream associated
  // I try to assign the input name to the ifstream
  in.open(input.c_str());
  // I try to verify that it is possible open this stream
  if(ASSERTFILE(in,input)) {
   // In this case ASSERTFILE manages all the errors.
   // The file exists and it was correctly opened!
   // I can assign the fileName correctly.
  fileName=input;
  // now I can opens it
   string line;
   size_t posString;
   ss.str().clear();
   ss.str(""); //I clear the streambuffer associated.
   s=""; // clear the private string s.
   pos=0; // clar also the iterator
   while (getline(in,line)) {
     // I strips the comment line that begins with a '#' or '//' or other characters as
     // indicated in 'comment' variable

     // verify that 'comment' is not included within a string variable
     posString = line.rfind('"');
     pos = line.rfind(comment.c_str());
     if ( (pos<posString) & (posString != string::npos) ) {
       s += line+ '\n';
     }
     else {
       if (pos != string::npos) { // there is a comment
         if (pos != 0) {
           line = line.substr(0,pos);
           s += line + '\n';
         } // otherwise skip the whole line
       }
       else s += line + '\n';
     };
    };
   ss<<s;
  }
  in.close();
  return fileName;
}


char* ReadFileIni::file() {
    // this method return all read file:
    return (char*)ss.str().c_str();
}





// Methods which return an int error variable if the read is not correct.

int  ReadFileIni::getDoubleValErr(const std::string& str, double& value, const std::string& para) { //! It returns a 1 if it fails

 tmp=para;
 pos=s.find(tmp,0);
 pos+= tmp.length();
 tmp=str;
 pos=s.find(tmp,pos); // begins from pos position


 if (pos==string::npos) { return 1;
 }
 else {
   // find '='
    pos=s.find("=",pos);
    if (pos==string::npos) { return 2;
    }
   ss.seekg(pos+1, ios::beg);
   ss >>value;
   return 0;
 };
}


int ReadFileIni::getFloatValErr(const std::string& str, float& value, const std::string& para) { //! It returns a float value

 tmp=para;
 pos=s.find(tmp,0);
 pos+= tmp.length();
 tmp=str;
 pos=s.find(tmp,pos); // begins from pos position

 if (pos==string::npos) { return 1;
 }
 else {
   // find '='
    pos=s.find("=",pos);
    if (pos==string::npos) { return 2;
    }
   ss.seekg(pos+1, ios::beg);
   ss >>value;
   return 0;
 };
}


int ReadFileIni::getIntValErr(const std::string& str, int& value, const std::string& para) { //! It returns an integer value

 tmp=para;
 pos=s.find(tmp,0);
 pos+= tmp.length();
 tmp=str;
 pos=s.find(tmp,pos);
 if (pos==string::npos) { return 1;
 }
 else {
   // find '='
    pos=s.find("=",pos);
    if (pos==string::npos) { return 2;
    }
   ss.seekg(pos+1, ios::beg);
   ss >>value;
   return 0;
 };
}


int ReadFileIni::getStringValErr(const std::string& str, std::string& value, const std::string& para) { //! It returns a string value

 tmp=para;
 pos=s.find(tmp,0);
 pos+= tmp.length();
 tmp=str;
 pos=s.find(tmp,pos);
 if (pos==string::npos) { return 1;
 }
 else {
   // find '='
    pos=s.find("=",pos);
    if (pos==string::npos) { return 2;
    }
    ss.seekg(pos+1, ios::beg);
    ss >>value;


   return 0;
 };
}



int ReadFileIni::getStringQuotedValErr(const std::string& str, std::string& value, const std::string& para) { //! It returns a string value

 size_t pos1;


 tmp=para;
 pos=s.find(tmp,0);
 pos+= tmp.length();
 tmp=str;
 pos=s.find(tmp,pos);
 if (pos==string::npos) { return 1;
 }
 else {
   // find '='
    pos=s.find("=",pos);
    if (pos==string::npos) { return 2;
    }
    // now I check from this point the character "
     pos=s.find('"',pos);
     if (pos==string::npos) { return 1;
     }
     else {
    //temporary store of pos;
      pos1=pos+1;
      pos=s.find('"',pos1);
       if (pos==string::npos) { return 1;
      }
      else {
       value=s.substr(pos1, pos-pos1 );
      return 0;
     };
    };
   };
}






// Methods which return the value and in case of error the management of the reading error is devouted to ErrVisualize.


double  ReadFileIni::getDoubleVal(const std::string& str) { //! It returns a double value

 double  value=0.0;
 tmp=str+'=';
 pos=s.find(tmp,0);
 if (pos==string::npos) { ERRORF("Specificator for numerical input not existent", 1100);
 }
 else {
   ss.seekg(pos+tmp.length(), ios::beg);
   ss >>value;
 };
  return value;
}

float ReadFileIni::getFloatVal(const std::string& str) { //! It returns a float value
 float  value=0.0;
 tmp=str+'=';
 pos=s.find(tmp,0);
 if (pos==string::npos) { ERRORF("Specificator for numerical input not existent", 1200);
 }
 else {
   ss.seekg(pos+tmp.length(), ios::beg);
   ss >>value;
 };
  return value;
}




int ReadFileIni::getIntVal(const std::string& str) { //! It returns an integer value

 int  value=0;
 tmp=str+'=';
 pos=s.find(tmp,0);
 if (pos==string::npos) { ERRORF("Specificator for numerical input not existent", 1300);
 }
 else {
   ss.seekg(pos+tmp.length(), ios::beg);
   ss >>value;
 };
  return value;
}


std::string ReadFileIni::getStringVal(const std::string& str) { //! It returns a string value

 string value;
 tmp=str+'=';
 pos=s.find(tmp,0);
 if (pos==string::npos) { ERRORF("Specificator for string input not existent", 1100);
 }
 else {
   ss.seekg(pos+tmp.length(), ios::beg);
   ss >>value;
 };
  return value;
}




std::string ReadFileIni::getStringQuotedVal(const std::string& str) { //! It returns a string value

 string value;
 size_t pos1;

 tmp=str+'=';
 pos=s.find(tmp,0);
 if (pos==string::npos) { ERRORF("Specificator for string input not existent", 1200);
 }
 else {// now I check from this point the character "
   pos=s.find('"',pos);
   if (pos==string::npos) {  ERRORF("Specificator for first apex not existent", 1202);
   }
   else {
    //temporary store of pos;
    pos1=pos+1;
    pos=s.find('"',pos1);
     if (pos==string::npos) {  ERRORF("Specificator for last apex not existent", 1203);

     }
     else {
      value=s.substr(pos1, pos-pos1 );
     return value;
     };
   };
 };
 return value;
};



int ReadFileIni::getDoubleVectorErr(const std::string& str, double* data, int index, const std::string& para) { //! It returns a double vector

 double value;
 int loop;

 tmp=para;
 pos=s.find(tmp,0);
 pos+= tmp.length();
 tmp=str;
 pos=s.find(tmp,pos);
 if (pos==string::npos) { return 1;
 }
 else {
   // find '='
    pos=s.find("=",pos);
    if (pos==string::npos) { return 2;
    }
    ss.seekg(pos+1, ios::beg);
   // This method does not verify that the input vector is really existing!!
   // the loop starts from 0!!
   for (loop=0; loop<index; loop++) {
     ss >>value;
    *(data+loop)=value;
   }
 };
  return 0;
}


int ReadFileIni::getFloatVectorErr(const std::string& str, float* data, int index, const std::string& para) { //! It returns a double vector

 float value;
 int loop;

 tmp=para;
 pos=s.find(tmp,0);
 pos+= tmp.length();
 tmp=str;
 pos=s.find(tmp,pos);
 if (pos==string::npos) { return 1;
 }
 else {
   // find '='
    pos=s.find("=",pos);
    if (pos==string::npos) { return 2;
    }
    ss.seekg(pos+1, ios::beg);
   // This method does not verify that the input vector is really existing!!
   // the loop starts from 0!!
   for (loop=0; loop<=index; loop++) {
     ss >>value;
    *(data+loop)=value;
   }
 };
  return 0;
}


int ReadFileIni::getIntVectorErr(const std::string& str, int* data, int index, const std::string& para) { //! It returns a double vector

 int value;
 int loop;

 tmp=para;
 pos=s.find(tmp,0);
 pos+= tmp.length();
 tmp=str;
 pos=s.find(tmp,pos);
 if (pos==string::npos) { return 1;
 }
 else {
   // find '='
    pos=s.find("=",pos);
    if (pos==string::npos) { return 2;
    }
    ss.seekg(pos+1, ios::beg);
   // This method does not verify that the input vector is really existing!!
   // the loop starts from 0!!
   for (loop=0; loop<=index; loop++) {
     ss >>value;
    *(data+loop)=value;
   }
 };
  return 0;
}

