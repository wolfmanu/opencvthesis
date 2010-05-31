#pragma once

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib

#define DEFAULT_BUFLEN 100
#define PORT "21"
#define BUFLEN 1024

class FTPSender
{
public:
	FTPSender(void);
	~FTPSender(void);

	int rcvData(string* data);
	int sendData(string data);
	int connectFTP(string serverName, string serverPort, string user, string pwd);
	

private:
	WSADATA wsaData;	//contains information about the Windows Sockets implementation.
    SOCKET ConnectSocket;
    struct addrinfo *result, *ptr , hints;

	int startUpConnection(string serverName, string serverPort);

};
