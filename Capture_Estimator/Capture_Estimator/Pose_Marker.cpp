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
	return s.str();
}