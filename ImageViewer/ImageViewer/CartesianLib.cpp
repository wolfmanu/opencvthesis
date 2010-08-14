
#include <math.h>

#include "CartesianLib.h"


// Cartesian 3-components vector to Rotation Matrix (Roll, -Pitch, Yaw)
void vect2RPY(VECT3 in, ROTMATRIX * out )
{
  
	GREAL cr,sr,cp,sp,cy,sy;

	cr = cos(in.a[0]);
    sr = sin(in.a[0]);
    cp = cos(in.a[1]);
    sp = sin(in.a[1]);
    cy = cos(in.a[2]);
    sy = sin(in.a[2]);

    out->col.i.l.x =  cr * cp;
    out->col.i.l.y =  sr * cp;
    out->col.i.l.z =  sp;

    out->col.j.l.x = -sr * cy - cr * sp * sy;
    out->col.j.l.y =  cr * cy - sr * sp * sy;
    out->col.j.l.z =  cp * sy;

    out->col.k.l.x =  sr * sy - cr * sp * cy;
    out->col.k.l.y = -cr * sy - sr * sp * cy;
    out->col.k.l.z =  cp * cy;

}

// RPY velocity to angle axis vel, (Roll, -Pitch, Yaw)
void RPYVel2AngVel(VECT3 vel, VECT3 rpy, VECT3 * out ) {

	GREAL cr,sr,cp,sp;

	/* Compute sins */
	cr = cos(rpy.a[0]);
    sr = sin(rpy.a[0]);
    cp = cos(rpy.a[1]);
    sp = sin(rpy.a[1]);

	/* Compute vel */
	out->a[0] =            sr*vel.a[1] + cr*cp*vel.a[2];
	out->a[1] =          - cr*vel.a[1] + sr*cp*vel.a[2];
	out->a[2] = vel.a[0] +             +    sp*vel.a[2];
	
	return;
}

// Cartesian 6-components vector to Transformation Matrix (Rotation part as Roll,-Pitch,Yaw)
void vect2Tmatrix(VECT6 in, TRANSFMATRIX * out )
{
	vect2RPY(in.v3[0],&(out->rev.r));
	GT_CopyVect3(in.v3[1],&(out->rev.v));
}


// Rotation matrix to Cartesian 3-components vector
void RPY2vect(ROTMATRIX  in, VECT3 * out )
{
  
	GREAL cr,sr;

	if((fabs(in.col.i.l.x) < RPY2VECT_EPS) && (fabs(in.col.i.l.y) < RPY2VECT_EPS))
	{
		out->a[0] = 0;
		out->a[1] = PI/2*in.col.i.l.z;
		out->a[2] = atan2(-in.col.j.l.x,in.col.j.l.y);
	}
	else
	{
		out->a[0] = atan2(in.col.i.l.y,in.col.i.l.x);

		sr = sin(out->a[0]);
		cr = cos(out->a[0]);

		out->a[1] = atan2(in.col.i.l.z,(in.col.i.l.x*cr + in.col.i.l.y*sr));

		out->a[2] = atan2((in.col.k.l.x*sr - in.col.k.l.y*cr),(-in.col.j.l.x*sr + in.col.j.l.y*cr));
	}
}

// Transformation matrix to Cartesian 6-components vector [ang|lin]
void Tmatrix2Vect(TRANSFMATRIX  in, VECT6 * out )
{
	RPY2vect(in.rev.r,&(out->v3[0]));
	GT_CopyVect3(in.rev.v,&(out->v3[1]));
}


// Computes 3-components attitude vector between two Rotation Matrices 
void versorLemma(ROTMATRIX  in1, ROTMATRIX  in2, VECT3 * out )
{
  
	int     i;
	GREAL  costh, sinth, theta;
	GREAL  maxnorm;
	VECT3   temp1,temp2,temp3;
	VECT3   rosinth;


	costh = (GT_ScalarProd(in1.col.i, in2.col.i)
		    +GT_ScalarProd(in1.col.j, in2.col.j)
		    +GT_ScalarProd(in1.col.k, in2.col.k)
		    -1) * 0.5;

	GT_CrossProd(in1.col.i, in2.col.i, &temp1);
	GT_CrossProd(in1.col.j, in2.col.j, &temp2);
	GT_CrossProd(in1.col.k, in2.col.k, &temp3);
	
	for (i=0;i<3;i++)
	{
		rosinth.a[i] = 0.5*(temp1.a[i] + temp2.a[i] + temp3.a[i]);
	}
	
    sinth = GT_NormVect3(rosinth);



	if(sinth>VERSOR_LEMMA_TH)
	// 0 < theta < 180 degrees
	{
		theta = atan2(sinth,costh);
		for(i=0;i<3;i++)
			out->a[i] = (rosinth.a[i] / sinth) * theta;
	}
	else
	{
		if(costh>0)
		// theta = 0 degrees
		{
			for(i=0;i<3;i++)
				out->a[i] = 0;
		}
		else
		// theta = 180 degrees
		{
			GT_SumVect3(in1.col.i, in2.col.i,&temp1); 
			GT_SumVect3(in1.col.j, in2.col.j,&temp2); 
			GT_SumVect3(in1.col.k, in2.col.k,&temp3);
			
			maxnorm = GT_Max3Vect3(temp1,temp2,temp3,&temp1);

			if(maxnorm!=0)
				for(i=0;i<3;i++)
					out->a[i] = temp1.a[i] * PI / maxnorm;
			else
				for(i=0;i<3;i++)
					out->a[i] = 0;
		}
	}
}

// Computes 6-components Cartesian error with 2 Transformation matrices as input 
// err = [errang|errlin] where in1 = x* and  in2 = x 
void cartErrorTmatrix(TRANSFMATRIX  in1, TRANSFMATRIX  in2, VECT6 * out )
{
	int			i;
	VECT3  errang;

	versorLemma(in1.rev.r,in2.rev.r,&errang);

	for(i=0;i<3;i++) 
	{
		out->v3[0].a[i] = - errang.a[i] ;
		out->v3[1].a[i] = in1.rev.v.a[i] - in2.rev.v.a[i] ;
	}
}


// Computes 6-components Cartesian error with 2 6-components vectors as input 
// err = [errang|errlin] where in1 = x* and  in2 = x 
void cartErrorVect6(VECT6  in1, VECT6  in2, VECT6 * out )
{
	int			i;
	ROTMATRIX	Ra,Rb;
	VECT3		errang;

    vect2RPY(in1.v3[0],&Ra);
    vect2RPY(in2.v3[0],&Rb);
	
	versorLemma(Ra,Rb,&errang);

	for(i=0;i<3;i++) 
	{
		out->v3[0].a[i] = - errang.a[i] ;
		out->v3[1].a[i] = in1.v3[1].a[i] - in2.v3[1].a[i] ;
	}
}


//  Product of a rotation matrix by a scalar   
short RmatrixByScal(ROTMATRIX RMat1, GREAL scal, ROTMATRIX* Out)
{
	int i;

	for (i=0;i<9;i++) 
	{
		Out->mat[i] = RMat1.mat[i] * scal;
	}

	return 0;
}

//  Sum of two rotation matrices   
short sumRmatrix(ROTMATRIX RMat1, ROTMATRIX RMat2, ROTMATRIX* Out)
{
	return GT_SumMatrix((GREAL*)RMat1.mat,3,3,(GREAL*)RMat2.mat,3,3,(GREAL*)Out->mat,'+');
}

//  Product of a rotation matrix by a 3-components vector   
short RmatrixByVector(ROTMATRIX RMat,VECT3 Vect,VECT3* Out) 
{
	return GT_MultMatrix((GREAL*)RMat.mat,3,3,(GREAL*)Vect.a,3,1,(GREAL*)Out->a);
}


//  Product of a transformation matrix by a 3-components vector   
short TmatrixByVector(TRANSFMATRIX TMat, VECT3 Vect, VECT3* Out) 
{
	short err;
	VECT3 Tmp;

	err = GT_MultMatrix((GREAL*)TMat.rev.r.mat,3,3,(GREAL*)Vect.a,3,1,(GREAL*)&Tmp);

	if (err!=1) {

		return err;
	}
	return GT_SumMatrix((GREAL*)Tmp.a,3,1,(GREAL*)TMat.rev.v.a,3,1,(GREAL*)Out->a,'+');

}

//  Product of two rotation matrices   
short RmatrixByRmatrix(ROTMATRIX RMat1, ROTMATRIX RMat2, ROTMATRIX* Out) {

	return GT_MultMatrix((GREAL*)RMat1.mat,3,3,(GREAL*)RMat2.mat,3,3,(GREAL*)Out->mat);

}

//  Product of two transformation matrices   
short TmatrixByTmatrix(TRANSFMATRIX TMat1, TRANSFMATRIX TMat2, TRANSFMATRIX* Out) {

	short err;

	err = RmatrixByRmatrix(TMat1.rev.r,TMat2.rev.r,&(Out->rev.r));

	if (err!=1) {

		return err;
	}

	return TmatrixByVector(TMat1, TMat2.rev.v, &(Out->rev.v));

}

//  Inversion of a transformation matrix   
short InvTmatrix(TRANSFMATRIX TMat1, TRANSFMATRIX* Out) {

	
	GT_TransMatrix(TMat1.rev.r.mat, 3, 3, Out->rev.r.mat);
	GT_Vect3ByScal(TMat1.rev.v, -1.0, &(TMat1.rev.v));
	RmatrixByVector(Out->rev.r, TMat1.rev.v, &(Out->rev.v));
	return 0;

}

//  Product of two Vect6 
short Vect6ByVect6(VECT6 v1, VECT6 v2, VECT6* Vout) {

	TRANSFMATRIX TMat1,TMat2,Tout;

	vect2Tmatrix(v1, &TMat1 );
	vect2Tmatrix(v2, &TMat2 );

	RmatrixByRmatrix(TMat1.rev.r,TMat2.rev.r, &(Tout.rev.r));
	TmatrixByVector(TMat1, TMat2.rev.v, &(Tout.rev.v));

	Tmatrix2Vect(Tout, Vout );
	return 0;
}




//  Computes the exponential form of a rotation matrix (Out = e^[W^]Th  with  v=WxTh)   
void angleAxis(VECT3 v, ROTMATRIX* Out) {

	GREAL		norm;						
	GREAL		cth;									
	GREAL		sth;						
	VECT3		versor;						
	ROTMATRIX	crossmatrix;				
	ROTMATRIX	crossmatrix2;				
	ROTMATRIX	identity = {1,0,0,0,1,0,0,0,1};

	norm = GT_NormVect3(v);	

	if(norm != 0)
	{
		GT_Vect3ByScal(v, 1/norm, &versor);
	}
	else
	{
		GT_CopyVect3(v,&versor);
	}

	// cross product matrix  
	crossmatrix.mat[0] = 0.0;
	crossmatrix.mat[1] = versor.l.z;
	crossmatrix.mat[2] = -versor.l.y;

	crossmatrix.mat[3] = -versor.l.z;
	crossmatrix.mat[4] = 0.0;
	crossmatrix.mat[5] = versor.l.x; 

	crossmatrix.mat[6] = versor.l.y;
	crossmatrix.mat[7] = -versor.l.x;
	crossmatrix.mat[8] = 0.0;

	// [sin(Th)] and [1-cos(Th)]  
	cth = 1 - cos(norm);
	sth = sin(norm);	
	
	// Out = I + sin(Th)[W^] + (1-cos(Th))[W^]^2
	RmatrixByRmatrix(crossmatrix, crossmatrix, &crossmatrix2);
	RmatrixByScal(crossmatrix, sth, &crossmatrix);
	RmatrixByScal(crossmatrix2, cth, &crossmatrix2);
	sumRmatrix(crossmatrix,identity,&crossmatrix);
	sumRmatrix(crossmatrix,crossmatrix2,Out);
}

//  Computes the integral of a rotation matrix (Out = e^[wdt^] * Rin )   
void strapDown(VECT3 w, GREAL dt, ROTMATRIX Rin, ROTMATRIX* Out) 
{
	ROTMATRIX	R;
	VECT3		wdt; 
	
	GT_Vect3ByScal(w,dt,&wdt);

	angleAxis(wdt,&R);
	RmatrixByRmatrix(R,Rin,Out);
}

//  Computes the integral of a transformation matrix    
void intTmatrix(VECT6 vin, GREAL dt, TRANSFMATRIX Tin, TRANSFMATRIX* Out) 
{
	strapDown(vin.v3[0],dt,Tin.rev.r,&(Out->rev.r));

	GT_Vect3ByScal(vin.v3[1],dt,&(Out->rev.v));
	GT_SumVect3(Out->rev.v,Tin.rev.v,&(Out->rev.v));
}

//  Computes the integral of a Cartesian 6-components vector
void intVect6(VECT6 vin, GREAL dt, VECT6 vold, VECT6* out) 
{
	ROTMATRIX	R,R2;

	vect2RPY(vold.v3[0],&R);
	strapDown(vin.v3[0],dt,R,&R2);
	RPY2vect(R2,&(out->v3[0]));

	GT_Vect3ByScal(vin.v3[1],dt,&(out->v3[1]));
	GT_SumVect3(out->v3[1],vold.v3[1],&(out->v3[1]));
}

//  Computes a rigid body transformation
// void rigidBodyTrans(VECT6 vin, TRANSFMATRIX in, VECT6* out) 
// {
// 	VECT3 vtemp;
// 
// 	GT_CopyVect3(vin.v3[0],&(out->v3[0]));
// 	GT_CrossProd(vin.v3[0],dist,&vtemp);
// 	GT_SumVect3(vin.v3[1],vtemp,&(out->v3[1]));
// 	  
// }


/*//  Computes a rigid body transformation
void rigidBodyTrans(VECT6 vin, VECT3 dist, VECT6* out) 
{
	VECT3 vtemp;

	GT_CopyVect3(vin.v3[0],&(out->v3[0]));
	GT_CrossProd(vin.v3[0],dist,&vtemp);
	GT_SumVect3(vin.v3[1],vtemp,&(out->v3[1]));
	  
}
*/