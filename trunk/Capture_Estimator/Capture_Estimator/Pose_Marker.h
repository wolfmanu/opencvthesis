#pragma once

class Pose_Marker
{
public:
	Pose_Marker(double roll, double pitch, double yaw,	
				double distX, double distY, double distZ, 
				int mId);
	Pose_Marker(void);

	~Pose_Marker(void);

	double roll, pitch, yaw;		//degree
	double distX, distY, distZ;		//cm
	
	int errorState;

	int markerId;

	std::string toString();
	std::string toLine();
	std::string toFTP();

	friend bool operator ==(const Pose_Marker &pm1, const Pose_Marker &pm2);
	
};
