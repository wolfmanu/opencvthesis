#include "StdAfx.h"
#include "FTPSender.h"

#define FTP_DEBUG

FTPSender::FTPSender(string serverName, string serverPort)
:net(serverName, serverPort)
{
}

FTPSender::~FTPSender(void)
{
}


int FTPSender::FTPconnect(string user, string pwd)
{
	string sUser= "USER ";
	string sPwd= "PASS ";
	string nl="\n\r";
	string Msg;
	int iResult;

	if( net.NWConnect()==-1)
	{
		cout<<"FTP connection error"<<endl;
		return -1;
	}

	iResult = FTPrcvData(&Msg);	//Receive Welcome Message
	//std::cout<<welcomeMsg;
		
	//Inviare nome e pw per la connessione ftp se richiesto
	if(user=="")
		return 1;
	
	sUser.append(user);
	sUser.append(nl);
	FTPsendData(sUser);
	FTPrcvData(&Msg);
	if(Msg[0]>3)
	{
		cout<<"FTP usermane error"<<endl;
		return -2;
	}
	if(pwd=="")
		return 1;

	sPwd.append(pwd);
	sPwd.append(nl);
	FTPsendData(sPwd);
	FTPrcvData(&Msg);
	if(Msg[0]>3)
	{
		cout<<"FTP password error"<<endl;
		return -3;
	}
	return 1;
}

int FTPSender::FTPsendData(string data)
{
	int iResult;
	string sdata(data.begin(),data.end());
	
	iResult = net.NWsendData(sdata.c_str(),sdata.length());
   
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