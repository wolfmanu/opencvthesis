#pragma once

#define BUFLEN 1024

class Network
{
private:

	WSADATA wsaData;	//contains information about the Windows Sockets implementation.
    SOCKET ConnectSocket;
    struct addrinfo *result, *ptr , hints;

public:

	Network::Network();
	Network(std::string serverName, std::string serverPort);
	~Network(void);

	void setNetwork(std::string serverName, std::string serverPort);
	int NWConnect(void);
	int NWsendData(const char* data, int len);
	int NWrcvData(char* data, int len);
	int NWCloseConnection(void);

	static DWORD testConnectivity(std::string ip)
	{
		HANDLE hIcmpFile;
		unsigned long ipaddr = INADDR_NONE;
		DWORD dwRetVal = 0;
		char SendData[] = "Data Buffer", ipchar[14];
		LPVOID ReplyBuffer = NULL;
		DWORD ReplySize = 0;

		ipaddr = inet_addr(ip.c_str());
	    
		hIcmpFile = IcmpCreateFile();
		if (hIcmpFile == INVALID_HANDLE_VALUE) {
			printf("\tUnable to open handle.\n");
			printf("IcmpCreatefile returned error: %ld\n", GetLastError() );
			return 0;
		}    

		ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
		ReplyBuffer = (VOID*) malloc(ReplySize);
		if (ReplyBuffer == NULL) {
			printf("\tUnable to allocate memory\n");
			return 0;
		}    
		
		return IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), 
							NULL, ReplyBuffer, ReplySize, 1000);
	}

	


	


};
