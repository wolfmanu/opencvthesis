#pragma once

class Square_Area
{
public:
	Square_Area(CvPoint* corners[],double area);
	~Square_Area(void);

	int Square_Area::operator>(const Square_Area& elem)
	{
		return this->area<elem.area; //al contrario perchè mi serve dal piu grande e il set ordina dal piu piccolo
	}
	
	struct classcomp {
		bool operator() (const Square_Area* lhs, const Square_Area* rhs) const
		{
			return lhs->area > rhs->area;
		}
	};


	CvPoint corners[4];
	double area;
private:
	
	
};
