#include "StdAfx.h"
#include "CameraOptions.h"



CameraOptions::CameraOptions(void):
	viewPose(false),
	viewCenter(false), 
	sendFtp(false),
	saveImg(false), 
	viewVideo(true),
	elaborate(false),
	useUsb(false),
	ftpUser(DEF_USER),
	ftpPass(DEF_PASSWORD), 
	saveFold(DEF_SAVE_FOLDER), 
	saveName(DEF_IMG_FILE_BASE_NAME_SAVE),
	ftpServer(DEF_SERVER),
	ftpPort(DEF_PORT),
	cameraIp(DEF_CAMERA_IP),
	wip1(DEF_CAMERA_IP1),
	wip2(DEF_CAMERA_IP2),
	wip3(DEF_CAMERA_IP3),
	wip4(DEF_CAMERA_IP4)
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
void CameraOptions::setCameraIp( wstring ws1,wstring ws2,wstring ws3, wstring ws4)
{
	wip1=ws1;
	wip2=ws2;
	wip3=ws3;
	wip4=ws4;
	cameraIp.assign(ws1.begin(),ws1.end());
	cameraIp.append(".");
	cameraIp.append(ws2.begin(),ws2.end());
	cameraIp.append(".");
	cameraIp.append(ws3.begin(),ws3.end());
	cameraIp.append(".");
	cameraIp.append(ws4.begin(),ws4.end());
}