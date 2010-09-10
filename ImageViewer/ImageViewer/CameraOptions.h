#pragma once

using namespace std;

class CameraOptions
{
public:
	CameraOptions(void);
	//CameraOptions(bool vp, bool vc, bool sf, bool si, bool vv, string fu, string fp, string sfo, string sn);

	~CameraOptions(void);

	void setUser( wstring ws);
	void setPass( wstring ws);
	void setServer(wstring ws);
	void setPort( wstring ws);
	void setFold( wstring ws);
	void setName( wstring ws);
	void setCameraIp( wstring ws1,wstring ws2,wstring ws3, wstring ws4);

	bool viewPose, viewCenter, viewVideo, sendFtp, saveImg, elaborate, useUsb;
	string ftpUser, ftpPass, ftpServer, ftpPort, saveFold, saveName, cameraIp; 
	wstring wftpUser, wftpPass, wftpServer, wftpPort, wsaveFold, wsaveName, wip1, wip2, wip3, wip4; 

};
