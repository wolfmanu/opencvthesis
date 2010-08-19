#ifndef MYDEFS_H
#define MYDEFS_H



#define PI			3.14159265358979323846

typedef double GREAL; 

typedef struct vectList3{

	double x; 
	double y; 
	double z;

} VECTLIST3;

typedef struct vectList6{

	double phi;
	double psi;
	double theta;
	double x;
	double y;
	double z;

} VECTLIST6;



// 3-components vector 
typedef union vect3 {

	double		a[3];
	VECTLIST3	l;

} VECT3;


// 6-components vector 
typedef union vect6{

	double		a[6];
	VECTLIST6	l;
	VECT3		v3[2];

} VECT6;


// null vectors constant 
//const VECT3	ZEROVECT3 = {0.0, 0.0, 0.0};
//const VECT6	ZEROVECT6 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};


#endif




