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
	extern int errstate;
	double derrstate=errstate;
	double dummydata=0.0;
	char str[100+1]; //10char*10values + 1'/0'
	sprintf(str,"%10.4f",derrstate);
	sprintf(&(str[10]),"%10.4f",distX);
	sprintf(&(str[20]),"%10.4f",distY);
	sprintf(&(str[30]),"%10.4f",distZ);
	sprintf(&(str[40]),"%10.4f",roll);
	sprintf(&(str[50]),"%10.4f",pitch);
	sprintf(&(str[60]),"%10.4f",yaw);
	sprintf(&(str[70]),"%10.4f",markerId);
	sprintf(&(str[80]),"%10.4f",dummydata);
	sprintf(&(str[90]),"%10.4f",dummydata);
	
	
	return str;
}


std::wstring Pose_Marker::toLine()
{
	std::wstringstream s;
	s.precision(3);
	s << _T("roll ") ;
	s << _T("pitch ") ;
	s << _T("yaw");
	s << _T("\t")<< roll ;
	s << _T("\t")<< pitch ;
	s << _T("\t")<< yaw <<endl;
	s << _T("x    ") ;
	s << _T("y     ");
	s << _T("z  ") ;
	s << _T("\t")<< distX ;
	s << _T("\t")<< distY;
	s << _T("\t")<< distZ ;
	s.dec;
	s << _T("\t\tID ")<< markerId << std::endl;
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
