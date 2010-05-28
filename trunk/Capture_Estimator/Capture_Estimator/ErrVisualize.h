/*---------------------------------------------------------------------------
Program.......: ErrVisualizeApp.exe
File..........: ErrVisualize.h
Purpose.......: ErrVisualize class definition
Author........: P. Lanza
Modified......: P. Lanza
Created.......: 30/08/2005
Last Changed..: 06/09/2005
Copyright.....:
License.......:


---------------------------------------------------------------------------*/


#ifndef ERRVISUALIZE_H
#define ERRVISUALIZE_H
#include <sstream>
////#include "ErrVisualizeTclass.h"

//! A general visualisation test of the errors occurred during the program execution.
/*!
This class manages all the errors messages issued by the different parts of the program
in a unique safe class. This class can manage the messages received and send them to
the cerr stream or directly to the GUI interface.
The errors can be subdivided in the following categories ( or levels):
- F as Fatal Error. A Fatal error stops the program without any possibility to recovers
the work session.
- E as normal Error. Sometimes the operator can solve the cause of the error.(e.g a file
name that has not read form the program because it has been stored in a different directory.)
- W Warning message. The Warning message usually do not interrupt the program.
- A as Assertion error. It is a further specification to the program user. An assertion error
 can occurred when Assertions macros defined into Assertion.h header file are used.

Each error is identify with a unique error number. In this way it is possible to have a clear
and easy identification of where has been occurred. A second future advantage is the possibility
to translate the message ( written normally in English) to other language. In fact each error
is identified univocally and therefore its translation is quite simple.

During the emulation usage of this class it is foreseen a check of the error unambiguous.
This check is running automatically by the ErrVisualize class.
ErrVisualize class shall be istantied in emulation state.
In this way each received message is stored in a file stream 'ErrorsMsgLog.txt" and it is check
its relative error number.

The user can distinguish the message errors in different flavours:
- with debug information. The debug information are relative to file name and line number
where the error is occurred;
- with the data and time;
- with a simple decoration ( see the example below) to distinguish the error message to other
printout message when only cerr stream is enabled.

Furthermore for each message error can be added the file name where has been revealed
and the line code where the error has been detected.
To simplify the usage of this class some opportune MACRO has been defined for the user.
This macro requires only the message and the unique error number.

\code
// initialisation of the singleton! DebugInfo=true
ErrVisualize ErrVisualize::Err(true);

// example of call via instance() method. This call is due to the singleton
// structure of the class has is implemented in C++.

 ErrVisualize::instance()->setDebugInfo(true); // example of usage

// other code..

// example of msg() call
 ErrVisualize::instance()->msg(msgtmp.str(),57 ,"a", __FILE__, __LINE__ );

// the above call achieved via Macro ERROR
  ERRORM(msgtmp.str(), 57);

\endcode

examples of error messages depending by the different boolean flags:
\param None parameter

\verbatim
This type of error is undefined.
\endverbatim

\param decoratedInfo active. Introduce header and footer of the error message

\verbatim
  ------------start error message-------------------
This type of error is undefined.
  ------------end error message---------------------
\endverbatim

\param debugInfo active. It indicates the file and the line where the error is occurred.

\verbatim
  ------------start error message-------------------
This type of error is undefined.

Warning in ErrVisualizeTest.cpp at line 1958

  ------------end error message---------------------
\endverbatim

\param timeInfo active. It indicates the date and the time.

\verbatim
  ------------start error message-------------------
This type of error is undefined.

Warning in ErrVisualizeTest.cpp at line 1958
Tue Sep  6 12:31:33 2005

  ------------end error message---------------------
\endverbatim



*/



class ErrVisualize {

 public:
////  friend class  ErrVisualizeTclass; //! The hook to test the private and data members of this class
  static ErrVisualize* instance() { return &Err; } //! This method is used for all functions of this class
  /*! In fact the ErrVisualize class has been thought as
  singleton class and therefore a unique installation is possible. The method instance() returns a pointer to all
  public functions of the class.
  */

//! This function is used as input of  message error.
/*! The following input parameters are used:
 \param input is the string of the message error. This parameter can be a simple string char or a string class
 \param errNum. It specifies the error number. The error number is unambiguous in the main program. To verify that
all errNum are unambiguous it is necessary to activate the class in emulation mode.
 \param level. There are specified the error level. This parameter is a string parameter and therefore is not
fixed format of a single character. This format can be modified adding other characters for future version of this class.
In this class version the following characters are used:
- F as Fatal error;
- E as normal error;
- W as Warning error;
- A as Assertion error;
- M as Message error;
 \param nfile is the number of file where the error has been detected. Normally this parameter has not used by final user because all the calls to this class are embedded into Macro.
 \param nline is the number of line where the error has been detected. Normally this parameter has not used by final user because all the calls to this class are embedded into Macro.

This class calls the out()function autonomously. The user see only the msg() call to visualise the error message.
The possible flavour of error message in the cerr stream depends by the different boolean variables activated.
Some examples are reported in the general presentation of the class.


\sa ERROR ERRORF ERRORE ERRORW ERRORA
*/

  int msg( const std::string& input,
            int errNum,               // num_error
            const std::string& level,  // level specification: F, E, W, A, M
            const std::string& nfile,  // typically filename from __FILE__
            const int nline);          // typically line number from __LINE__

//! This function manages the errors output messages toward cerr or the GUI.
/*! The behaviour of this class depends by the GUIInfo variable value. In this version
the class support only Windows GUI interface of error message. This interface will be provided
in the future version of the class.
*/

  virtual int out();

//! This function returns the last error message processed..

  const std::string lastMsg(); // output

//! This class return a string with more suggestions or tips regarding the revealed error.
/*! The function requires a unique input parameter.
\param details this integer value identifies a possible error (or better identify a category error) requiring
more details on it.
An example of usage it could be given by Assertion error or in case of mathematical errors on the matrix
elaboration.
This interface will be provided in the future version of the class.
*/

  const std::string moreDetails(const int details);  // more suggestions or investigation on the well specified error revealed

  // get and set functions relative to debugInfo private variable

  //! It returns the value of DebugInfo boolean parameter.
  bool getDebugInfo();

  //! It set the value of DebugInfo boolean parameter
  void setDebugInfo(const bool value);

  // enable and disable functions relative to debugInfo

  //! It enables the DebugInfo in the error message.
  void enableDebugInfo();
  //! It disables the DebugInfo in the error message.
  void disableDebugInfo();

  // get and set functions relative to timeInfo private variable

  //! It returns the value of TimeInfo boolean parameter.
  bool getTimeInfo();
  //! It sets the value of TimeInfo boolean parameter
  void setTimeInfo(const bool value);

  // enable and disable functions relative to TimeInfo

  //! It enables the TimeInfo in the error message.
  void enableTimeInfo();
  //! It disables the TimeInfo in the error message.
  void disableTimeInfo();

  // get and set functions relative to decoratedInfo private variable

  //! It returns the value of DecoratedInfo boolean parameter.
  bool getDecoratedInfo();
  //! It sets the value of DecoratedInfo boolean parameter.
  void setDecoratedInfo(const bool value);

  // enable and disable functions relative to DecoratedInfo

  //! It enables the DecoratedInfo in the error message.
  void enableDecoratedInfo();
  //! It disables the DecoratedInfo in the error message.
  void disableDecoratedInfo();

  // get and set functions relative to emulatedInfo private variable

//! It returns the value of EmulatedInfo boolean parameter.
  bool getEmulatedInfo();
  //! It sets the value of DecoratedInfo boolean parameter.
  void setEmulatedInfo(const bool value);

  // enable and disable functions relative to EmulatedInfo

  //! It enables the EmulatedInfo in the error message.
  void enableEmulatedInfo();
  //! It disables the EmulatedInfo in the error message.
  void disableEmulatedInfo();

  // get and set functions relative to GUIInfo private variable

  //! It returns the value of GUIInfo int parameter.
  int  getGUIInfo();
  //! It sets the value of GUIInfo int parameter.
/*! The possible value of the GUIInfo variable are the following:
- 0 means that only cerr stream is enabled for error message output;
- 1 means that only GUI interface is enabled for error message output;
- 2 means that both cerr stream and GUI interface are enabled for error message output;
*/
  void setGUIInfo(const int value);

  int retVal();

 private:
  static ErrVisualize Err;  //! singleton software hook implementation
  bool debugInfo;           //! true= enable all information for debug
  bool timeInfo;            //! true =enable the information relative to the time
  bool decoratedInfo;       //! true=enable decoration output for cerr stream
  bool emulatedInfo;        //! true=emulation behaviour (for test scope)
  bool firstMsg;            //! true= if this is the first message read in emulation mode
  int GUIInfo;              //! Possible values: 0=only stream, 1=only GUI, 2=stream+GUI
  time_t ltime;             //! variable used to store the time information into the error message
  int lasterrNum;           //! last error number
  std::string lastMsgString;//! last error message string
  std::string lastnfile;    //! last file name where the error has been revealed
  int lastnline;            //! last line number where the error has been revealed
  std::string outString;    //! string employed for out error message!
  char type;                //! type of error
  int retValue;             //! numerical return value from the class

//! constructor of the class
/*! The constructor of the class initialise all the internal variables.
In this case because the class has been implemented as singleton it is necessary initialise
at least a numerical variable. The variable that has been chosen was the \b debugInfo boolean parameter.
*/
  ErrVisualize(bool val);
//! prevent copy-construction
  ErrVisualize(const ErrVisualize&) {}; // prevent copy-construction
//! virtual distructor  because out() is a virtual method
  virtual ~ErrVisualize(){}; // virtual destructor because a virtual function has been issued.
//! This method manages the GUI interface. In this version is not implemented.
/*! This method is called only by the out() public method. It is virtual for future implementation.
 */
  int GUImsg(); //in this version is implemented only for Windows O.S!
//! evaluate the unambiguous of the error number
/*! This function is internally rather complex in spite of its software interface
This method stores all the error messages passed to the class into a unique file "ErrorMsgLog.txt"
This file is automatically created if it does not exist or it is reset if it is already existing.
For each error message is verified the unambiguous of the error number associated. The software solution implemented
uses the STL list<int> template. In this way it is not necessary to worry about the dynamic size of the array where
the numerical values are stored.
If a clash between two numerical values is happened an error message is stored in the "ErrorMsgLog.txt" file.

Furthermore for each error message are specified all input parameters of msg(). In this way its possible to
easily recovery any kind of error message.

*/
  virtual void emulation();           // evaluated the univocally of errNum and store all errors messages.


  // for next version
  /*!
    \todo
  In the next version  of this class it is necessary to implement
  - the GUI error interface. Actually only the cerr interface is implemented
  and tested into the code;
  - verify if it is necessary a file stream implementation appositely for the
 ErrVisualize class or whether the actual class is sufficient
  - implementation of moreDetails(const int details) {}. In this function member more
  suggestions or investigation on the well specified error revealed should given to the caller
  when it is possible ( e.g during the assertion calls).
  - implementation of a file where all messages of errors and warnings are written automatically
  - evaluate if different methods, each for each kind of errors are necessary:

  -# virtual  FatalError();
  -# virtual  Warning Error();
  -# virtual  MessageError():
  -# virtual  HardwareError();
  */


};



// definition of the Error MACRO:

// default macro for errors
#define _ERROR( msgStr, errNum)    ErrVisualize::instance()->msg( msgStr, errNum, "E",  __FILE__, __LINE__ );
// default macro for Fatal errors
#define ERRORF(msgStr, errNum)    ErrVisualize::instance()->msg( msgStr, errNum, "F",  __FILE__, __LINE__ );
// default macro for Errors  errors
#define ERRORE(msgStr, errNum)    ErrVisualize::instance()->msg(msgStr, errNum, "E",  __FILE__, __LINE__ );
// default macro for Warning errors
#define ERRORW(msgStr, errNum)    ErrVisualize::instance()->msg(msgStr, errNum, "W",  __FILE__, __LINE__ );
// default macro for Assertion errors
#define ERRORA(msgStr, errNum)    ErrVisualize::instance()->msg(msgStr, errNum, "A",  __FILE__, __LINE__ );
// default macro for Message errors
#define ERRORM(msgStr, errNum)    ErrVisualize::instance()->msg(msgStr, errNum, "M",  __FILE__, __LINE__ );

// default macro for Message information
// no information is added
#define INFO(msgStr, errNum)       ErrVisualize::instance()->msg(msgStr, errNum, "I",  __FILE__, __LINE__ );

#define ERRORRETURN   ErrVisualize::instance()->retVal();


//For Assertion Abort =3 Retry =4 Ignore=5


#endif // definition of ErrVisualize class
