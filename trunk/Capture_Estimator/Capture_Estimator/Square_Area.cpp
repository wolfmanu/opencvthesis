#include "StdAfx.h"
#include "Square_Area.h"

Square_Area::Square_Area(CvPoint* corners[],double area)
{
	this->area=area;
	for(int i=0; i<4; i++)
		this->corners[i]=*(corners[i]);
}

Square_Area::~Square_Area(void)
{
}

