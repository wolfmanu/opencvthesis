#ifndef MATRIXLIB_H
#define MATRIXLIB_H

#include <stdio.h>
#include "myDefs.h"

#define MAX_MATRIX_DIM	50
#define SVD_EPS			0.00000001

/*  Sum and difference  */
short GT_SumMatrix(GREAL* A, char m, char n, GREAL* B, char k, char j, GREAL* OUT,char Sign);
short GT_SumVect3(VECT3 Vect1, VECT3 Vect2, VECT3* Out);
short GT_SumVect6(VECT6 Vect1, VECT6 Vect2, VECT6* Out);
short GT_SubVect3(VECT3 Vect1, VECT3 Vect2, VECT3* Out);
short GT_SubVect6(VECT6 Vect1, VECT6 Vect2, VECT6* Out);

/*  Product of matrices and vectors   */
short GT_MultMatrix(GREAL* A, char m, char n, GREAL* B, char k, char p, GREAL* OUT);
GREAL GT_ScalarProd(VECT3 Vect1, VECT3 Vect2);
void GT_CrossProd(VECT3 Vect1, VECT3 Vect2, VECT3* Out);
void GT_ElementProdVect6(VECT6 Vect1, VECT6 Vect2, VECT6* Out);

/*  Copy of matrices and vectors   */
void GT_CopyMatrix(GREAL* src, char m, char n, GREAL* dest);
void GT_CopyVect3(VECT3 src, VECT3 * dest);
void GT_CopyVect6(VECT6 src, VECT6 * dest);

/*  Norm of vectors   */
GREAL GT_NormVect3(VECT3 Vect);
GREAL GT_NormVect6(VECT6 Vect);

/*  Product of vectors by scalar   */
void GT_Vect3ByScal(VECT3 v, GREAL Scal ,VECT3* OUT);
void GT_Vect6ByScal(VECT6 v, GREAL Scal ,VECT6* OUT);

/*  SVD decomposition   */
//void svdcmp(GREAL *a, int m, int n, GREAL *w, GREAL *v, GREAL *rv1);
short GT_RegPinv(GREAL *J, int m, int n, GREAL *JPInv, GREAL treshold, GREAL lambda,GREAL *prod);

/*  Miscellaneous   */
void GT_TransMatrix(GREAL* A, char m, char n, GREAL* OUT);
GREAL GT_Max3Vect3(VECT3 v1, VECT3 v2, VECT3 v3, VECT3* OUT);	

/*  Debug   */
void printMtx(char * string, GREAL * in, int row, int col);

#endif





	

