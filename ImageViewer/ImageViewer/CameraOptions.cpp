#include "StdAfx.h"
#include "CameraOptions.h"



CameraOptions::CameraOptions(void):
	viewPose(false),
	viewCenter(false), 
	sendFtp(false),
	saveImg(false), 
	viewVideo(true),
	elaborate(false),
	ftpUser(""),
	ftpPass(""), 
	saveFold(""), 
	saveName(""),
	ftpServer(""),
	ftpPort("")
{
}


CameraOptions::~CameraOptions(void)
{
}

void CameraOptions::setUser(wstring ws)
{
	wftpUser=ws;
	ftpUser.assign(ws.begin(),ws.end());
}
void CameraOptions::setPass(wstring ws)
{
	wftpPass=ws;
	ftpPass.assign(ws.begin(),ws.end());
}
void CameraOptions::setServer( wstring ws)
{
	wftpServer=ws;
	ftpServer.assign(ws.begin(),ws.end());
}
void CameraOptions::setPort( wstring ws)
{
	wftpPort=ws;
	ftpPort.assign(ws.begin(),ws.end());
}
void CameraOptions::setFold(wstring ws)
{
	wsaveFold=ws;
	saveFold.assign(ws.begin(),ws.end());
}
void CameraOptions::setName( wstring ws)
{
	wsaveName=ws;
	saveName.assign(ws.begin(),ws.end());
}