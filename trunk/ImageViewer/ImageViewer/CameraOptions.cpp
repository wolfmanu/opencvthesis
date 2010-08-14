#include "StdAfx.h"
#include "CameraOptions.h"



CameraOptions::CameraOptions(void):
	viewPose(false),
	viewCenter(false), 
	sendFtp(false),
	saveImg(false), 
	viewVideo(true),
	elaborate(false),
	ftpUser(L""),
	ftpPass(L""), 
	saveFold(L""), 
	saveName(L""),
	ftpServer(L"")
{
}

//CameraOptions::CameraOptions(bool vp, bool vc, bool sf, bool si, bool vv, string fu, string fp, string sfo, string sn):
//	viewPose(vp),
//	viewCenter(vc), 
//	sendFtp(sf),
//	saveImg(si), 
//	viewVideo(vv),
//	ftpUser(fu),
//	ftpPass(fp), 
//	saveFold(sfo), 
//	saveName(sn)
//{
//}

CameraOptions::~CameraOptions(void)
{
}
