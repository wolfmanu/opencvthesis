
#include <math.h>
#include <string.h>

#include "MatrixLib.h"

#define MATRIXLIB_SIGN(a,b)	((b) >= 0.0 ? fabs(a) : -fabs(a))
#define MATRIXLIB_MAX(a,b)	((a) > (b)  ? (a) : (b))
#define MATRIXLIB_MIN(a,b)	((a) < (b)  ? (a) : (b))

/*
*
****************************  Sum and difference of matrices/vectors  *************************
*
*/

//  Sum of two matrices   
short GT_SumMatrix(GREAL* A, char m, char n, GREAL* B, char k, char j, GREAL* OUT,char Sign) {

	int i;
	int size;

	if (m!=k) {
		return -1;
	}

	if (n!=j) {
		return -1;
	}

	size = m * n;

	if (Sign=='+') {
		for (i=0;i<size;i++) {
			OUT[i] = A[i] + B[i];
		}
	} else if (Sign=='-') {
		for (i=0;i<size;i++) {
			OUT[i] = A[i] - B[i];
		}
	} else {
		return -2;
	}

	return 1;
}

//  Sum of two 3-components vectors   
short GT_SumVect3(VECT3 Vect1, VECT3 Vect2, VECT3* Out) {

	return GT_SumMatrix((GREAL*)Vect1.a,3,1,(GREAL*)Vect2.a,3,1,(GREAL*)Out->a,'+');

}

//  Sum of two 6-components vectors   
short GT_SumVect6(VECT6 Vect1, VECT6 Vect2, VECT6* Out) {

	return GT_SumMatrix((GREAL*)Vect1.a,6,1,(GREAL*)Vect2.a,6,1,(GREAL*)Out->a,'+');
}

//  Difference of two 3-components vectors   
short GT_SubVect3(VECT3 Vect1, VECT3 Vect2, VECT3* Out) {

	return GT_SumMatrix((GREAL*)Vect1.a,3,1,(GREAL*)Vect2.a,3,1,(GREAL*)Out->a,'-');

}

//  Difference of two 6-components vectors   
short GT_SubVect6(VECT6 Vect1, VECT6 Vect2, VECT6* Out) {

	return GT_SumMatrix((GREAL*)Vect1.a,6,1,(GREAL*)Vect2.a,6,1,(GREAL*)Out->a,'-');

}

/*																					
*
***************************  Product of matrices/vectors  *************************
*
*/

//  Product of two matrices   
short GT_MultMatrix(GREAL* A, char m, char n, GREAL* B, char k, char p, GREAL* OUT) {

	int i;
	int j;
	int l;

	if (n!=k) {
		return -1;
	}


	for (i=0;i<m*p;i++) {
		OUT[i] = 0;
	}

	for (l=0;l<p;l++) {
		for (i=0;i<m;i++) {
			for (j=0;j<n;j++) {
				OUT[i+l*m]    +=    A[i+m*j]   *   B[j+l*n];
			}
		}
	}
	return 1;
}



//  Scalar product of two 3-components vectors   
GREAL GT_ScalarProd(VECT3 V1, VECT3 V2) {

	return (V1.a[0]*V2.a[0]+V1.a[1]*V2.a[1]+V1.a[2]*V2.a[2]);

}

//  Cross product of two 3-components vectors   
void GT_CrossProd(VECT3 Vect1, VECT3 Vect2, VECT3* Out) {

	Out->a[0] = Vect1.a[1]*Vect2.a[2]-Vect1.a[2]*Vect2.a[1];
	Out->a[1] = Vect1.a[2]*Vect2.a[0]-Vect1.a[0]*Vect2.a[2];
	Out->a[2] = Vect1.a[0]*Vect2.a[1]-Vect1.a[1]*Vect2.a[0];
}

void GT_ElementProdVect6(VECT6 Vect1, VECT6 Vect2, VECT6* Out){

	Out->a[0] = Vect1.a[0] * Vect2.a[0];
	Out->a[1] = Vect1.a[1] * Vect2.a[1];
	Out->a[2] = Vect1.a[2] * Vect2.a[2];
	Out->a[3] = Vect1.a[3] * Vect2.a[3];
	Out->a[4] = Vect1.a[4] * Vect2.a[4];
	Out->a[5] = Vect1.a[5] * Vect2.a[5];
}


/*																					
*
***************************  Copy of matrices/vectors  *************************
*
*/

//  Make a copy of a Matrix 
void GT_CopyMatrix(GREAL* src, char m, char n, GREAL* dest) {

	int i;

	for(i=0;i<m*n;i++)
	{
		dest[i]=src[i];
	}
}

//  Make a copy of a 3-components vector 
void GT_CopyVect3(VECT3 src, VECT3 * dest) {

	int i;

	for(i=0;i<3;i++)
	{
		dest->a[i]=src.a[i];
	}
}

//  Make a copy of a 6-components vector 
void GT_CopyVect6(VECT6 src, VECT6 * dest) {

	int i;

	for(i=0;i<6;i++)
	{
		dest->a[i]=src.a[i];
	}
}




/*																					
*
***************************  Miscellaneous  *************************
*
*/


//  Computes the norm of a 3-components vector   
GREAL GT_NormVect3(VECT3 V) {

	return sqrt(V.a[0]*V.a[0]+V.a[1]*V.a[1]+V.a[2]*V.a[2]);

}

//  Computes the norm of a 6-components vector   
GREAL GT_NormVect6(VECT6 V) {

	return sqrt(V.a[0]*V.a[0]+V.a[1]*V.a[1]+V.a[2]*V.a[2]+V.a[3]*V.a[3]+V.a[4]*V.a[4]+V.a[5]*V.a[5]);

}

//  Product of a 3-components vector by a scalar  
void GT_Vect3ByScal(VECT3 v, GREAL Scal ,VECT3* OUT) {

	int i;

	for (i=0;i<3;i++) {
		OUT->a[i] = v.a[i] * Scal;
	}
}

//  Product of a 6-components vector by a scalar  
void GT_Vect6ByScal(VECT6 v, GREAL Scal ,VECT6* OUT) {

	int i;

	for (i=0;i<6;i++) {
		OUT->a[i] = v.a[i] * Scal;
	}
}



//  Transpose a matrix (NOTE: the input and the output matrices must be different!!!)  
void GT_TransMatrix(GREAL* A, char m, char n, GREAL* OUT) {

	int i;
	int j;
	
	for (i=0;i<m;i++) {
		for (j=0;j<n;j++) {
			OUT[j+n*i] = A[i+m*j];
		}
	}
}


//  Among the 3 3-components input vectors, returns the maximum norm one (and its norm)  
GREAL GT_Max3Vect3(VECT3 v1, VECT3 v2, VECT3 v3, VECT3* OUT){		

	GREAL  t1,t2,t3;
	
	t1 = GT_NormVect3(v1);
	t2 = GT_NormVect3(v2);
	t3 = GT_NormVect3(v3);

	if (t2>t1)
	{
		if(t2>t3)
		{
			*OUT = v2;
			return t2;
		}
		else
		{
			*OUT = v3;
			return t3;
		}
	}
	else
	{
		if(t3>t1)
		{
			*OUT = v3;
			return t3;
		}
		else
		{
			*OUT = v1;
			return t1;
		}
	}
}




/*																					
*
***************************  SVD computation  *************************
*
*/

//  Return  sqrt(a*a+b*b) (used by GT_svdcmp) 
GREAL GT_pythag(GREAL a, GREAL b)
{
	return sqrt(a*a+b*b);
}


//  Computes the SVD decomposition of a matrix a[m*n]	(a = U*S*V')   
//	The matrix V (NOT the transpose V') replaces "a" on output. 
//  The diagonal matrix of singular values S is output as a vector w[1..n].
//	The matrix U is output as v[n*n].

void GT_svdcmp(GREAL *a, int m, int n, GREAL *w, GREAL *v, GREAL *rv1)
{
	int flag, i, its, j, jj, k, l, nm;
	GREAL anorm, c, f, g, h, s, scale, x, y, z;

	g=0.0;
	scale=0.0;
	anorm=0.0;

	for (i=0; i<n; i++)
	{
		l = i+1;
		rv1[i] = scale*g;
		g = 0.0;
		s = 0.0;
		scale = 0.0;


		if (i<m)
		{
			for (k=i; k<m; k++)
				scale += fabs(a[k+m*i]);
			if (scale)
			{
				for (k=i; k<m; k++)
				{
					a[k+m*i] /= scale;
					s += a[k+m*i]*a[k+m*i];
				}
				f = a[i+m*i];
				g = -MATRIXLIB_SIGN(sqrt(s),f);
				h = f*g-s;
				a[i+m*i] = f-g;

				for (j=l;j<n;j++)
				{
					for(s=0.0, k=i; k<m; k++)
						s += a[k+m*i]*a[k+m*j];
					f = s/h;
					for (k=i; k<m; k++)
						a[k+m*j] += f*a[k+m*i];
				}
				for (k=i; k<m; k++)
					a[k+m*i] *= scale;
			}
		}
			
		w[i] = scale*g;
		g = 0.0;
		s = 0.0;
		scale = 0.0;

		if (i<m && i!=(n-1))
		{
			for(k=l; k<n; k++)
				scale += fabs(a[i+m*k]);
			if (scale)
			{
				for(k=l; k<n; k++)
				{
					a[i+m*k] /= scale;
					s += a[i+m*k]*a[i+m*k];
				}
				f = a[i+m*l];
				g = -MATRIXLIB_SIGN(sqrt(s),f);
				h = f*g-s;
				a[i+m*l] = f-g;

				for (k=l; k<n; k++)
					rv1[k] = a[i+m*k]/h;
				for (j=l; j<m; j++)
				{
					for (s=0.0, k=l; k<n; k++)
						s += a[j+m*k]*a[i+m*k];
					for (k=l; k<n; k++)
						a[j+m*k] += s*rv1[k];
				}
				for (k=l; k<n; k++)
					a[i+m*k] *= scale;
			}
		}
		anorm = MATRIXLIB_MAX(anorm, (fabs(w[i])+fabs(rv1[i])));
	}

	for (i=n-1; i>=0; i--)
	{
		if (i<n-1)
		{
			if (g)
			{
				for (j=l; j<n; j++)
					v[j+n*i] = (a[i+m*j]/a[i+m*l])/g;
				for (j=l; j<n; j++)
				{
					for (s=0.0, k=l; k<n; k++)
						s += a[i+m*k]*v[k+n*j];
					for (k=l; k<n; k++)
						v[k+n*j] += s*v[k+n*i];
				}
			}
			for (j=l; j<n; j++)
			{
				v[i+n*j] = 0.0;
				v[j+n*i] = 0.0;
			}
		}
		v[i+n*i] = 1.0;
		g = rv1[i];
		l=i;
	}

	for (i=MATRIXLIB_MIN(m,n)-1; i>=0; i--)
	{
		l = i+1;
		g = w[i];

		for (j=l; j<n; j++)
			a[i+m*j] = 0.0;

		if(g)
		{
			g = 1.0/g;

			for (j=l; j<n; j++)
			{
				for (s=0.0, k=l; k<m; k++)
					s += a[k+m*i]*a[k+m*j];
				f = (s/a[i+m*i])*g;
				for (k=i; k<m; k++)
					a[k+m*j] += f*a[k+m*i];
			}
			
			for (j=i; j<m; j++) a[j+m*i] *= g;
		}

		else
		{
			for (j=i; j<m; j++)
				a[j+m*i] = 0.0;
		}
		++a[i+m*i];
	}

	for (k=n-1; k>=0; k--)
	{
		for (its=1; its<=30; its++)
		{
			flag = 1;
			for (l=k; l>=0; l--)
			{
				nm = l-1;
				if ((GREAL)(fabs(rv1[l])+anorm) == anorm)
				{
					flag = 0;
					break;
				}
				if ((GREAL)(fabs(w[nm])+anorm) == anorm)
					break;
			}
			if (flag)
			{
				c = 0.0;
				s = 1.0;
				for (i=l; i<=k; i++)
				{
					f = s*rv1[i];
					rv1[i] = c*rv1[i];
					if ((GREAL)(fabs(f)+anorm) == anorm)
						break;
					g = w[i];
					h = GT_pythag(f,g);
					w[i] = h;
					h = 1.0/h;
					c = g*h;
					s = -f*h;
					for (j=0; j<m; j++)
					{
						y = a[j+m*nm];
						z = a[j+m*i];
						a[j+m*nm] = y*c+z*s;
						a[j+m*i] = z*c-y*s;
					}
				}
			}

			z = w[k];
			if (l==k)
			{
				if (z < 0.0)
				{
					w[k] = -z;
					for (j=0; j<n; j++)
						v[j+n*k] = -v[j+n*k];
				}
				break;
			}

			if (its == 30)
				printf("No convergence in 30 GT_svdcmp iterations");
			x = w[l];
			nm = k-1;
			y = w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
			g = GT_pythag(f,1.0);
			f = ((x-z)*(x+z)+h*((y/(f+MATRIXLIB_SIGN(g,f)))-h))/x;
			c = 1.0;
			s = 1.0;

			for (j=l; j<=nm; j++)
			{
				i = j+1;
				g = rv1[i];
				y = w[i];
				h = s*g;
				g = c*g;
				z = GT_pythag(f,h);
				rv1[j] = z;
				c = f/z;
				s = h/z;
				f = x*c+g*s;
				g = g*c-x*s;
				h = y*s;
				y *= c;
				
				for (jj=0; jj<n; jj++)
				{
					x = v[jj+n*j];
					z = v[jj+n*i];
					v[jj+n*j] = x*c+z*s;
					v[jj+n*i] = z*c-x*s;
				}
				z = GT_pythag(f,h);
				w[j] = z;
				if (z)
				{
					z = 1.0/z;
					c = f*z;
					s = h*z;
				}
				f = c*g+s*y;
				x = c*y-s*g;

				for (jj=0; jj<m; jj++)
				{
					y = a[jj+m*j];
					z = a[jj+m*i];
					a[jj+m*j] = y*c+z*s;
					a[jj+m*i] = z*c-y*s;
				}
			}
		
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}

}


//  Raised cosine function  (out = lambda*cos[(in/th)*(PI/2)])
GREAL GT_RaisedCos(GREAL in, GREAL th, GREAL lambda)
{
	if (in<0) 
		in += (2*in);

	if (0<=in && in<=th)
	{
		return (lambda * cos((in/th)*(PI/2)));
	}
	else 
	{
		return 0.0;
	}
}


//  Computes the SVD-based regularized matrix pseudoinversion (a = U*S*V')
short GT_RegPinvOld(GREAL *J, int m, int n, GREAL *JPInv, GREAL treshold, GREAL lambda, GREAL *prod)
{
	static GREAL SVD[3*MAX_MATRIX_DIM];
	static GREAL V[MAX_MATRIX_DIM];
	static GREAL S[MAX_MATRIX_DIM];
	static GREAL U[MAX_MATRIX_DIM];
	static GREAL temp[MAX_MATRIX_DIM];
	static GREAL temp2[MAX_MATRIX_DIM];

	int	i, j;
	GREAL Reg;
	short flag = 0;

	GT_TransMatrix(J,m,n,V);

	GT_svdcmp(V, n, m, S, U, SVD);
	
	*prod = 1.0;

	for (i=0; i<m; i++)
	{
		if (S[i] <= SVD_EPS)
		{
			*prod = (*prod) * S[i]; 
			S[i]=0;
			flag += 1;
		}
		else
		{
			Reg = GT_RaisedCos(S[i],treshold,lambda);
			
			*prod = (*prod) * S[i]; 
			
			if (Reg!=0) 
				flag += 1;

			S[i] = S[i]/(S[i]*S[i]+Reg);
		}

	}

	// temp=V*S
	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			temp[i*n+j] = V[i*n+j]*S[i];
		}
	}

	// Transpone U
	GT_TransMatrix(U,m,m,temp2);

	// Jpinv = V*S*U'
	GT_MultMatrix(temp,n,m,temp2,m,m,JPInv);

	return flag;

}



//  Computes the SVD-based regularized matrix pseudoinversion (a = U*S*V')
short GT_RegPinv(GREAL *J, int m, int n, GREAL *JPInv, GREAL treshold, GREAL lambda, GREAL * prod)
{
	int tempswap;
	static GREAL SVD[3*MAX_MATRIX_DIM];
	static GREAL V[MAX_MATRIX_DIM];
	static GREAL S[MAX_MATRIX_DIM];
	static GREAL U[MAX_MATRIX_DIM];
	static GREAL temp[MAX_MATRIX_DIM];
	static GREAL temp2[MAX_MATRIX_DIM];

	int	i, j;
	GREAL Reg;
	short flag = 0;		

	GT_TransMatrix(J,m,n,U);

	tempswap=n;
	n=m;
	m=tempswap;


	memset(S,0,MAX_MATRIX_DIM);


	GT_svdcmp(U, m, n, S, V, SVD);
	
	*prod = 1.0;

	for (i=0; i<n; i++)
	{
		if (S[i] <= SVD_EPS)
		{
			*prod = (*prod) * S[i]; 
			S[i]=0;
			flag += 1;
		}
		else
		{
			Reg = GT_RaisedCos(S[i],treshold,lambda);
			
			*prod = (*prod) * S[i]; 
			
			if (Reg!=0) 
				flag += 1;

			S[i] = S[i]/(S[i]*S[i]+Reg);
		}

	}

	for (i=0; i<n; i++)
	{
		for (j=0; j<n; j++)
			temp[n*i+j] = V[n*j+i] * S[j];
	}

//	GT_TransMatrix(V,n,n,temp2);

	// temp=V*S
//	GT_MultMatrix(S,n,n,temp2,n,n,temp);
/*	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			temp[i*n+j] = [i*n+j]*[i];
		}
	}*/

	// Transpone U
//	GT_TransMatrix(U,m,m,temp2);

	// Jpinv = V*S*U'
	GT_MultMatrix(U,m,n,temp,n,n,JPInv);

	return flag;

}




/*																					
*
***************************  Debug  *************************
*
*/


//  Print the elements of a matrix   
void printMtx(char * string, GREAL * in, int row, int col)
{

	int i,j;

	printf("\n%s\n",string);
	for (i=0;i<row;i++)
	{
		for(j=0;j<col;j++)
			printf("%g ",in[j*row+i]);
		printf("\n");
	}
}

