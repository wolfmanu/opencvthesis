#pragma once

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib

#define PORT "21"
#define BUFLEN 4096

class FTPSender
{
public:
	FTPSender(std::string serverName, std::string serverPort=PORT);

	~FTPSender(void);

	int FTPrcvData(std::string* data);
	int FTPsendData(std::string data);
	int FTPconnect(std::string user, std::string pwd);
	

private:
	Network net;
};
