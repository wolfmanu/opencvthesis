#include "StdAfx.h"
#include "FTPSender.h"

#define FTP_DEBUG

FTPSender::FTPSender(string serverName, string serverPort):net(serverName, serverPort)
{
}

FTPSender::~FTPSender(void)
{
}


int FTPSender::FTPconnect(string user, string pwd)
{
	string sUser="USER ";
	string sPwd="PASS ";
	string nl="\n\r";
	string welcomeMsg;
	int iResult;

	if(! net.NWConnect())
	{
		cout<<"Connection error"<<endl;
		return 0;
	}

	iResult = FTPrcvData(&welcomeMsg);	//Receive Welcome Message
	//std::cout<<welcomeMsg;
		
	//Inviare nome e pw per la connessione ftp se richiesto
	//TODO
	return 1;
}

int FTPSender::FTPsendData(string data)
{
	int iResult;

	iResult = net.NWsendData(data.c_str(),data.length());
   
    if (iResult == SOCKET_ERROR) {
        printf("[FTPSender]sendData - send failed: %d\n", WSAGetLastError());
		net.~Network();
        return 0;
    }
	return iResult;
}

int FTPSender::FTPrcvData(string* data)		//bloccante
{
	int iResult;
	char recvbuf[BUFLEN];
	ZeroMemory( &recvbuf, sizeof(recvbuf) );

	iResult = net.NWrcvData(recvbuf, BUFLEN);

	data->assign(recvbuf);
	return iResult;
}