#pragma once
/*---------------------------------------------------------------------------
#Program.......: any program
#File..........: assertion.h
#Purpose.......: assertation and defensive programming global functions.
#Author........: P. Lanza ( partially based on B. Eckel open source code see copyright notice below
#Modified......: P. Lanza
#Created.......: 28/08/2005
#Last Changed..: 30/08/2005
#Version.......: 01.00
#Copyright.....:
#License.......:
#ToDo..........:
#Note..........:  Status ok. The tests are all ok!
#---------------------------------------------------------------------------*/

//: :require.h
// From Thinking in C++, 2nd Edition
// Available at http://www.BruceEckel.com
// (c) Bruce Eckel 2000
// Copyright notice in Copyright.txt
// Test for error conditions in programs
// Local "using namespace std" for old compilers


//TO USE ASSERTION WITHOUT ErrVisualize ENABLE THIS STATEMENT

#include <fstream>
#include <sstream>
#include <iostream>
#include "ErrVisualize.h"
#ifndef ASSERTATION_H
#define ASSERTATION_H


// some usefull macro always defined by Bruce Eckel


/*!\def PRINT(data)
 \brief This macro prints the value of data

*/

/*!\def TRACE(x)
 \brief This macro prints the value of the statements (x) and executes the statement.

*/

/*!\def PRINTMACRO
 \brief This compiling variable when is defined activate the macro PRINT and TRACE.

*/


#ifndef PRINTMACRO
#define PRINTMACRO

#ifdef PRINTMACRO
// this macro prints only the name of any variable
#define PRINTNAME(x) std::cout << #x " = "<<'\n';
// this macro prints the value of any variable.
#define PRINT(x) std::cout << #x " = " << x <<'\n';
// this macro prints the value of any statements executed..
#define TRACE(x) std::cout << #x " = " << x <<endl, x
#elif // the same MACRO when the print message is not require
#define PRINTNAME(x) ;
#define PRINT(x) ;
#define TRACE(x) x
#endif
#endif // refered to PRINTMACRO


// definition of some usefull MACROs


/*!\def ASSERTM(data, msg)
 \brief A macro similar to assert macro of standard C

This macro returns true if is all ok, otherwise it sends an message error to cerr or, if it existing, an Error_Manager class.In the message error are specified the File and the line of code of where the macro has been called.

*/

/*!\def ASSERTARGS(argc, args, msg)
 \brief A macro that verifies that argc arguments are equal to args specified.

This macro returns true if is all ok, otherwise it sends an message error to cerr or, if it existing, an Error_Manager class.In the message error are specified the File and the line of code of where the macro has been called.

*/

/*!\def ASSERTMINARGS(argc, args, msg)
 \brief A macro that verifies that argc arguments are at minimum equal to args specified.

This macro returns true if is all ok, otherwise it sends an message error to cerr or, if it existing, an Error_Manager class.In the message error are specified the File and the line of code of where the macro has been called.

*/

/*!\def ASSERTFILE(argc, args, msg)
 \brief A macro that verifies that the file stream indicated and its name are really existing or in good state.

This macro returns true if is all ok, otherwise it sends an message error to cerr or, if it existing, an Error_Manager class.In the message error are specified the File and the line of code of where the macro has been called.

*/


#define ASSERTM(data, msg) assertfn( data, #msg,  __FILE__, __LINE__ )
#define ASSERTARGS(argc, args, msg) assertArgsfn( argc, args, #msg, __FILE__, __LINE__ )
#define ASSERTMINARGS(argc, args, msg) assertArgsfn( argc, args, #msg, __FILE__, __LINE__ )
#define ASSERTFILE(in, filename) assertFilefn( in, filename, __FILE__, __LINE__ )


// Una asserzione sa sempre da quale file e` stata attivata grazie alle macro. E` pertanto inutile aggiungere il nuemro di errore: risulta essere troppo complicato e sviante per il programmatore


bool assertfn(bool requirement,
  const std::string& msg = "Requirement failed",
  const std::string& nfile= "no filename code available ",
	int nline=9999 ){ //nline=9999 is a default line

using namespace std;
 ostringstream tmp;
  if (requirement==false) {
    tmp<<msg.c_str()<<" <info> in "<<nfile+" at "; // <info is a tag for hide or recovery the FILE and LINE info
  if (nline==9999) { tmp<<"no line available"<<endl;}
  else { tmp<<nline<<endl; };
  ErrVisualize::instance()->msg(tmp.str(),10 ,"A", nfile, nline);

  }
  return requirement;
}

bool assertArgsfn(int argc, int args,
  const std::string& msg = "Must use %d arguments",
  const std::string& nfile= "no filename code available ",
  int nline=9999 ){ //nline=9999 is a default line

using namespace std;
 ostringstream tmp;
   if (argc != args + 1) {
     if (msg=="Must use %d arguments") {
       tmp<<"Must use "<<args<<" arguments. "; }
     else { tmp<<msg.c_str(); }
     tmp<<" <info> in "<<nfile<<" at ";
      if (nline==9999) { tmp<<"no line available\n "; }
      else { tmp<<nline<<endl; };
      ErrVisualize::instance()->msg(tmp.str(),20 ,"A", nfile, nline);
     return false;
   }
   return true;
}

bool assertMinArgsfn(int argc, int minArgs,
  const std::string& msg = "Must use %d arguments",
  const std::string& nfile= "no filename code available ",
  int nline=9999 ){ //nline=9999 is a default line

using namespace std;
 ostringstream tmp;
  if(argc < minArgs + 1) {
     if (msg=="Must use at least %d arguments") {
       tmp<<"Must use at least "<<minArgs<<"arguments. ";
     }
     else { tmp<<msg.c_str(); }
     tmp<<" <info> in "<<nfile<<" at ";
      if (nline==9999) { tmp<<"no line available"<<endl; }
      else { tmp<<nline<<endl; };
      ErrVisualize::instance()->msg(tmp.str(),30 ,"A", nfile, nline);
     return false;
  }
  return true;
}

bool assertFilefn(std::ifstream& in,
  const std::string& filename = "no filename specified",
  const std::string& nfile= "no file name code available ",
  int nline=9999 ){ //nline=9999 is a default line

using namespace std;
 ostringstream tmp;
  if(!in) {
    tmp<<"Could not open file "<<filename.c_str();
    tmp<<" <info> in "<<nfile<<" at ";
      if (nline==9999) { tmp<<"no line available"<<endl; }
      else { tmp<<nline<<endl; };
      ErrVisualize::instance()->msg(tmp.str(),40 ,"A",nfile, nline);
    return false;
  }
  return true;
}

bool assertFilefn(std::ofstream& out,
  const std::string& filename = "no filename specified",
  const std::string& nfile= "no filename available ",
  int nline=9999 ){ //nline=9999 is a default line

using namespace std;
 ostringstream tmp;
  if(!out) {
    tmp<<"Could not open file "<<filename.c_str();
    tmp<<" <info> in "<<nfile<<" at ";
      if (nline==9999) { tmp<<"no line available"<<endl; }
      else { tmp<<nline<<endl; };
      ErrVisualize::instance()->msg(tmp.str(),50 ,"A", nfile, nline);
    return false;
  }
  return true;
}




#endif // ASSERTATION_H ///:~




