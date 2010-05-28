#/*---------------------------------------------------------------------------
#Program.......: ReadFileIniApp
#File..........: ReadFileIni.h
#Purpose.......: definition of the ReadFileIni class.
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

// documentation for initialisation file

/*
20 Febbraio 2006

Occorre aggiornare la documentazione e i tests relativi a questa classe:

- Innanzitutto e' possibile scegliere i caratteri delimitatori per i commenti
# oppure //.
- tali caratteri sono letti correttamente se posti all'interno di una variabile stringa quoted
esempio:
  stringa inizio commento = $$
           value = "string with $$ is a remark!"

Mancano ancora dei test per le varie funzioni: soprattutto l'uso
che se ne fa quando vengono usate con l'indice 'para'
come inizio paragrafo

Piu' in generale occorre mettere mano a piu' tests
con queste nuove opzioni


*/

/*! \page page1 Initialisation files rules

An Initialisation file is a simple plain test file where the following simple rules are respected:
- Each comment line begins with the symbol # or //
- Each assignment variable is followed by a '='
- Each assignment variable can assign numerical values or also string values.
- Each assignment variable can define vector variable. Each numerical value is separated with simple space character
- Each assignment variable is univocally defined.

In the following is shown an initialisation file as example:

\verbatim
[world]
world=modello2.wrl
pose=1.5 -1.5 0.8 1.0 -0.1 0.7

[calibration]
calibration=art.cal

[segmentation]
#         processing parameters
#borders=10
maxdev=0.5
threshold=2.0
minlength=2
minsegsize=6.0
sigma=1.0
\endverbatim

The word enclosed in the square braces do not assume a particular meaning during the elaboration of the file by means of the ReadFileIni class.

*/


/*
TO DO
 7 October 2006

E` da migliorare lo strip dei caratteri '#' o '//'
poiche` puo` esistere il caso di una linea come:
 var = "testo quotato con caratteri # e // "
 e questa linea non viene correttamente interpretata


Occorre fare dei testi esaustivi anche su tutte le variabili modificate ...


*/







#ifndef READFILE_H
#define READFILE_H

#include <fstream>
#include <sstream>
#include <string>

//! The main purpose of this class is to read the input or initialisation file.
/*! This class is in charge to read simple plain text file of initialisation. It is composed by two constructors ( default constructor and constructor with the input name of the file) and different members in charge to read the different kind of inputs ( double, float, integer and string) in accordance with the simple rules of the plain text of initialisation.


\sa Initialisation file

\todo
- define the Vector method for the class Vector or Matrix in the next version of this class it is needed.

*/

class ReadFileIni {

 public:
//! The hook to test the private members and data
  friend class ReadFileIniTclass;
  // constructors
//! default constructor
  ReadFileIni();

//! constructor that opens the file stream associated
  ReadFileIni( const std::string& input, const std::string& comment="#");

//! it returns the file name associated to the input stream
  std::string  getfileName();

//! it returns the file read
char* ReadFileIni::file();

//! it sets the file name and open the stream associated
  std::string  setOpenfileName(const std::string& input, const std::string& comment="#");

//! It returns a double value in val.
/*!If the value has been correctly read the returns value is 0 otherwise the methods returns an error number.
\param input this parameter defines the assignment variable
\param value is the numerical value returned by the method
\param para is optional. This search along the file the title indicated as string where to start
the seacrh of the input name parameter.

\sa Initialisation files rules
 */
  int getDoubleValErr(const std::string& str, double& value, const std::string& para="");

//! It returns a float value in val.
/*!If the value has been correctly read the returns value is 0 otherwise the methods returns an error number.
\param input this parameter defines the assignment variable
\param value is the numerical value returned by the method
\param para is optional. This search along the file the title indicated as string where to start
the search of the input name parameter.

\sa  Initialisation files rules
 */
  int getFloatValErr(const std::string& str, float& value, const std::string& para="");

//! It returns an integer value in val.
/*! If the value has been correctly read the returns value is 0 otherwise the methods returns an error number.
\param input this parameter defines the assignment variable
\param value is the numerical value returned by the method
\param para is optional. This search along the file the title indicated as string where to start
the seacrh of the input name parameter.

\sa  Initialisation files rules
 */
  int getIntValErr(const std::string& str, int& value, const std::string& para="");

//! It returns a string value in val.
/*! If the value has been correctly read the returns value is 0 otherwise the methods returns an error number.
\param input this parameter defines the assignment variable
\param value is the string returned by the method
\param para is optional. This search along the file the title indicated as string where to start
the seacrh of the input name parameter.

 */
  int getStringValErr(const std::string& str, std::string& value, const std::string& para="");


//! It returns a string value in val.The quoted string has been read starting with a " and lasting with a " character.
/*! If the value has been correctly read the returns value is 0 otherwise the methods returns an error number.
\param input this parameter defines the assignment variable
\param value is the string returned by the method
\param para is optional. This search along the file the title indicated as string where to start
the seacrh of the input name parameter.

 */
  int getStringQuotedValErr(const std::string& str, std::string& value, const std::string& para="");


//! It returns a double vector
/*!
This function resturns a double pointer to a vector. The pointer is given as input data and it is specified the number of elements that compond the vector.
This method does not verify that the input vector is really existing!!
\param para is optional. This search along the file the title indicated as string where to start
the seacrh of the input name parameter.

Furthermore the number of elements specified begins from 1 and not 0. That measn that an array of 5 elements the input data \index =5 and not 4 taking into account that the first element is positioned to 0.
*/
  int getDoubleVectorErr(const std::string& str, double* data, int index, const std::string& para="");


//! It returns a float vector
/*!
This function resturns a float pointer to a vector. The pointer is given as input data and it is specified the number of elements that compond the vector.
This method does not verify that the input vector is really existing!!
\param para is optional. This search along the file the title indicated as string where to start
the seacrh of the input name parameter.

Furthermore the number of elements specified begins from 0 and not 1. That measn that an array of 5 elements the input data \index =5 and not 4 taking into account that the first element is positioned to 0.
*/
  int getFloatVectorErr(const std::string& str, float* data, int index, const std::string& para="");


//! It returns a int vector
/*!
This function resturns a double pointer to a vector. The pointer is given as input data and it is specified the number of elements that compond the vector.
This method does not verify that the input vector is really existing!!
\param para is optional. This search along the file the title indicated as string where to start
the seacrh of the input name parameter.

Furthermore the number of elements specified begins from 1 and not 0. That measn that an array of 5 elements the input data \index =5 and not 4 taking into account that the first element is positioned to 0.
*/
  int getIntVectorErr(const std::string& str, int* data, int index, const std::string& para="");



//! It returns a double value in val.
/*!
If the value has been correctly read the returns value is the double value.
Otherwise the methods calls the ErrVisualize method as fatal error. In this case the error is completely manages by the ErrVisualize class.
*/
  double getDoubleVal(const std::string& str);

//! It returns a float value.
/*!
If the value has been correctly read the returns value is the float value.
Otherwise the methods calls the ErrVisualize method as fatal error. In this case the error is completely manages by the \bErrVisualize class.
*/
  float getFloatVal(const std::string& str);

//! It returns an integer value.
/*!
If the value has been correctly read the returns value is the integer value.
Otherwise the methods calls the ErrVisualize method as fatal error. In this case the error is completely manages by the \bErrVisualize class.
*/
  int getIntVal(const std::string& str);

//! It returns a string value.
/*!
If the value has been correctly read the returns value is the string value.
Otherwise the methods calls the ErrVisualize method as fatal error. In this case the error is completely manages by the \bErrVisualize class.
*/
  std::string getStringVal(const std::string& str);


//! It returns a string value in val.The quoted string has been read starting with a " and lasting with a " character.
/*! If the value has been correctly read the returns value is 0 otherwise the methods returns an error number.
\param input this parameter defines the assignment variable
\param value is the string returned by the method
 */
  std::string getStringQuotedVal(const std::string& str);



 private:
//! It stores the file name of the fstream associated.
  std::string fileName;
//! private data of the input stream
  std::ifstream in;
//! string where is stored the file
  std::string s;
//! iterator for finding the strings
  size_t pos;
//! private string memeber for temporary usage
  std::string tmp;
//! stringstream where is stored all the fstream
  std::stringstream ss;

};


#endif // READFILE_H
