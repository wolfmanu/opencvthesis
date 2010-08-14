#pragma once

class Pose_Marker
{
public:
	Pose_Marker(double roll, double pitch, double yaw,	
				double distX, double distY, double distZ, 
				int mId/*, int pN*/);
	Pose_Marker(void);
	~Pose_Marker(void);

	double roll, pitch, yaw;		//degree
	double distX, distY, distZ;		//cm
	int poseNumber; //for testing

	int markerId;

	std::string toString();
	std::wstring toLine();
};
