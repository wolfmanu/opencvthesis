#include "StdAfx.h"
#include "Pose_Marker.h"

Pose_Marker::Pose_Marker(double roll_, double pitch_, double yaw_, 
						 double distX_, double distY_, double distZ_, 
						 int mId)
{
	roll = roll_;
	pitch = pitch_;
	yaw = yaw_;
	distX = distX_;
	distY = distY_;
	distZ = distZ_;
	markerId = mId;
	errorState=0;
}

Pose_Marker::Pose_Marker(void)
{
	errorState=0;
}

Pose_Marker::~Pose_Marker(void)
{
}

std::string Pose_Marker::toString()
{
	std::stringstream s;
	s << "roll: "<< roll << std::endl;
	s << "pitch: "<< pitch << std::endl;
	s << "yaw: "<< yaw << std::endl;
	s << "distX: "<< distX << std::endl;
	s << "distY: "<< distY << std::endl;
	s << "distZ: "<< distZ << std::endl;
	s << "markerId: "<< markerId << std::endl;
	s << "errorState: "<<errorState << std::endl;
	return s.str();
}

std::string Pose_Marker::toFTP()
{
	std::stringstream s;
	s << roll << std::endl;
	s << pitch << std::endl;
	s << yaw << std::endl;
	s << distX << std::endl;
	s << distY << std::endl;
	s << distZ << std::endl;
	s << markerId << std::endl;
	s << errorState << std::endl;
	return s.str();
}


std::wstring Pose_Marker::toLine()
{
	std::wstringstream s;
	s << _T("\t")<< roll ;
	s << _T("\t")<< pitch ;
	s << _T("\t")<< yaw ;
	s << _T("\t")<< distX ;
	s << _T("\t")<< distY;
	s << _T("\t")<< distZ ;
	s << _T("\t")<< markerId << std::endl;
	return s.str();
}

bool operator ==(const Pose_Marker &pm1, const Pose_Marker &pm2)
{
	return (pm1.roll == pm2.roll &&
			pm1.pitch == pm2.pitch &&
			pm1.yaw == pm2.yaw &&
			pm1.distX == pm2.distX &&
			pm1.distY == pm2.distY &&
			pm1.distZ == pm2.distZ &&
			pm1.markerId == pm2.markerId);

}
