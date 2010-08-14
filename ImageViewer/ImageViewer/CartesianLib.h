#ifndef CARTESIANLIB_H
#define CARTESIANLIB_H

#include "myDefs.h"
#include "MatrixLib.h"

#define RPY2VECT_EPS			0.000000001
#define VERSOR_LEMMA_TH			0.001 


// Rotation matrix 
typedef union {
	
	GREAL			mat[9];
	struct {
		VECT3	 i; // 1st column
		VECT3	 j; // 2nd column
		VECT3	 k; // 3rd column
	} col;

} ROTMATRIX;



// Transformation matrix 
typedef union  {
	
	GREAL			mat[12];
	struct {
		ROTMATRIX	r;
		VECT3		v; 
	} rev;

} TRANSFMATRIX;



/*  Conversion from vector to matrix and viceversa   */
void vect2RPY(VECT3 in, ROTMATRIX * out );
void vect2Tmatrix(VECT6 in, TRANSFMATRIX * out );
void RPY2vect(ROTMATRIX  in, VECT3 * out );
void Tmatrix2Vect(TRANSFMATRIX  in, VECT6 * out );


/*  Displacement between two Cartesian frames   */
void versorLemma(ROTMATRIX  in1, ROTMATRIX  in2, VECT3 * out );
void cartErrorTmatrix(TRANSFMATRIX  in1, TRANSFMATRIX  in2, VECT6 * out );
void cartErrorVect6(VECT6  in1, VECT6  in2, VECT6 * out );


/*  Product of Rotation and Transformation matrices and 3-components vectors   */
short RmatrixByScal(ROTMATRIX RMat1, GREAL scal, ROTMATRIX* Out);
short RmatrixByRmatrix(ROTMATRIX RMat1, ROTMATRIX RMat2, ROTMATRIX* Out);
short TmatrixByTmatrix(TRANSFMATRIX TMat1, TRANSFMATRIX TMat2, TRANSFMATRIX* Out);
short RmatrixByVector(ROTMATRIX RMat,VECT3 Vect,VECT3* Out);
short TmatrixByVector(TRANSFMATRIX TMat, VECT3 Vect ,VECT3* Out);
short Vect6ByVect6(VECT6 v1, VECT6 v2, VECT6* Vout);


/*  Other operations on Rotation and Transformation matrices and 3-components vectors   */
short sumRmatrix(ROTMATRIX RMat1, ROTMATRIX RMat2, ROTMATRIX* Out);
void angleAxis(VECT3 v, ROTMATRIX* Out);
void strapDown(VECT3 wdt, GREAL dt, ROTMATRIX Rin, ROTMATRIX* Out); 
void intTmatrix(VECT6 vin, GREAL dt, TRANSFMATRIX Tin, TRANSFMATRIX* Out); 
void intVect6(VECT6 vin, GREAL dt, VECT6 vold, VECT6* Out); 
void rigidBodyTrans(VECT6 vin, VECT3 dist, VECT6* out); 


short InvTmatrix(TRANSFMATRIX TMat1, TRANSFMATRIX* Out);


#endif


