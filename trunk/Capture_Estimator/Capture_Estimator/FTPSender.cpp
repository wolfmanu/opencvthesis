#include "StdAfx.h"
#include "FTPSender.h"

#define FTP_DEBUG

FTPSender::FTPSender(void)
{
	WSACleanup();
	ConnectSocket = INVALID_SOCKET;
	result = NULL;
	ptr = NULL;
}

FTPSender::~FTPSender(void)
{
	WSACleanup();
	if(result)
		freeaddrinfo(result);
	if(ptr)
		freeaddrinfo(ptr);
}

int FTPSender::startUpConnection(string serverName, string serverPort)
{
	int iResult;
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) 
	{
        printf("[FTPSender]startUpConnection - WSAStartup failed: %d\n", iResult);
        return 0;
    }


    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	/*int WSAAPI getaddrinfo(
	  __in_opt  PCSTR pNodeName,			A pointer to a NULL-terminated ANSI string 
											that contains a host (node) name or a numeric 
											host address string. For the Internet protocol, 
											the numeric host address string is a dotted-decimal 
											IPv4 address or an IPv6 hex address.
	  __in_opt  PCSTR pServiceName,
	  __in_opt  const ADDRINFOA *pHints,
	  __out     ADDRINFOA *ppResult
	);*/

	iResult = getaddrinfo(serverName.c_str(), serverPort.c_str(), &hints, &result);
    if ( iResult != 0 ) 
	{
        printf("[FTPSender]startUpConnection - getaddrinfo failed: %d\n", iResult);
        return 0;
    }
	
	return 1;
}

int FTPSender::connectFTP(string serverName, string serverPort, string user, string pwd)
{
	string sUser="USER ";
	string sPwd="PASS ";
	string nl="\n\r";
	string welcomeMsg;
	int iResult;

	if(!startUpConnection(serverName, serverPort))
	{
		printf("[FTPSender]connectFTP - Error at startUpConnection()\n");
        return 0;
    }

	// Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL;ptr=ptr->ai_next) 
	{
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) 
		{
            printf("[FTPSender]connectFTP - Error at socket(): %ld\n", WSAGetLastError());
            return 0;
        }
        
        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) 
		{
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }

		iResult = rcvData(&welcomeMsg);	//Receive Welcome Message
		std::cout<<welcomeMsg;
		
        break;
	}

    if (ConnectSocket == INVALID_SOCKET) 
	{
        printf("[FTPSender]connectFTP - Unable to connect to server!\n");
        return 0;
    }


	return 1;
}

int FTPSender::sendData(string data)
{
	int iResult;

	iResult = send(ConnectSocket, data.c_str(), data.length() , 0 );
   
    if (iResult == SOCKET_ERROR) {
        printf("[FTPSender]sendData - send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        return 0;
    }
	return iResult;
}

int FTPSender::rcvData(string* data)		//bloccante
{
	int iResult;
	char recvbuf[DEFAULT_BUFLEN];
	ZeroMemory( &recvbuf, sizeof(recvbuf) );

	iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);

	data->assign(recvbuf);
	return iResult;
}