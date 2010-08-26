#include "StdAfx.h"
#include "Network.h"

Network::Network()
{}

Network::Network(string serverName, string serverPort)
{
	setNetwork(serverName, serverPort);
}

Network::~Network(void)
{
	closesocket(ConnectSocket);
	
	WSACleanup();
	if(result)
		freeaddrinfo(result);

}

void Network::setNetwork(std::string serverName, std::string serverPort)
{
	int iResult;
	
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) 
	{
        printf("[Network]startUpConnection - WSAStartup failed: %d\n", iResult);
    }
	else
	{
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
			printf("[Network]startUpConnection - getaddrinfo failed: %d\n", iResult);
		}
	}
}

int Network::NWConnect(void)
{
	int iResult;

	// Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL;ptr=ptr->ai_next) 
	{
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) 
		{
            printf("[Network]connect - Error at socket(): %ld\n", WSAGetLastError());
            return -1;
        }
        
        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) 
		{
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
	}

    return 1;
}

int Network::NWsendData(const char* data,int len)
{
	int iResult;

	iResult = send(ConnectSocket, data, len , 0 );
   
    if (iResult == SOCKET_ERROR) {
        printf("[Network]sendData - send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        return 0;
    }
	return iResult;
}

int Network::NWrcvData(char* data, int len)		//bloccante
{
	int iResult;
	//ZeroMemory( &data, len );

	iResult = recv(ConnectSocket, data, len, 0);

	return iResult;
}

int Network::NWCloseConnection(void)
{
	int iResult;
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 0;
	}
	closesocket(ConnectSocket);	
	return 1;
}