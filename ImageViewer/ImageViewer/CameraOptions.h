#pragma once

using namespace std;

class CameraOptions
{
public:
	CameraOptions(void);
	//CameraOptions(bool vp, bool vc, bool sf, bool si, bool vv, string fu, string fp, string sfo, string sn);

	~CameraOptions(void);

	bool viewPose, viewCenter, viewVideo, sendFtp, saveImg, elaborate;
	wstring ftpUser, ftpPass,ftpServer, saveFold, saveName; 

};
