#pragma once
#include "Square_Area.h"

class Squares
{
public:
	Squares(void);
	~Squares(void);

	int vvv(void);
	void findSquare(IplImage* image);
	void drawSquares( IplImage* img, CvSeq* squares );
	void Squares::MydrawSquares( IplImage* cpy, CvPoint pt[] );

	set<Square_Area*,Square_Area::classcomp> SqAreaSet;

private:

	int thresh;
	IplImage* img ;
	IplImage* img0 ;
	CvMemStorage* storage ;
	const char* wndname ;

	
	CvSeq* findSquares4( IplImage* img, CvMemStorage* storage );
	double angle( CvPoint* pt1, CvPoint* pt2, CvPoint* pt0 );
	CvRect extractRectFromPoints(CvPoint p1,CvPoint p2,CvPoint p3,CvPoint p4);
};
