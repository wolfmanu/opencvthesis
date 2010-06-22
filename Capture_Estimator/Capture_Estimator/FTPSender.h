#pragma once

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib

#define PORT "21"
#define BUFLEN 4096

class FTPSender
{
public:
	FTPSender(string serverName, string serverPort=PORT);

	~FTPSender(void);

	int FTPrcvData(string* data);
	int FTPsendData(string data);
	int FTPconnect(string user, string pwd);
	

private:
	Network net;
};
