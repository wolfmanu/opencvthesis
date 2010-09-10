#include "StdAfx.h"
#include "FTPSender.h"

#define FTP_DEBUG

using namespace std;

std::string nl="\r\n";//\015\012

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
	
	string Msg;
	int iResult;

	if( net.NWConnect()==-1)
	{
		std::cout<<"FTP connection error"<<endl;
		return -1;
	}

	iResult = FTPrcvData(&Msg);	//Receive Welcome Message
	if(iResult<=0)
	{
		std::cout<<"FTP connection error"<<endl;
		return -1;
	}
	//std::cout<<welcomeMsg;
		
	//Inviare nome e pw per la connessione ftp se richiesto
	/*if(user=="")
		return 1;*/
	
	sUser.append(user);
	sUser.append(nl);
	FTPsendData(sUser);
	FTPrcvData(&Msg);
	if(Msg[0]=='4' || Msg[0]=='5')
	{
		std::cout<<"FTP usermane error"<<endl;
		return -2;
	}
	//230, meaning that the client has permission to access files under that username; 
	//or with code 331 or 332, meaning that permission might be granted after a PASS requ
	if(Msg[0]=='2')
		return 1; //pw not needed
	/*if(pwd=="")
		return 1;*/

	sPwd.append(pwd);
	sPwd.append(nl);
	FTPsendData(sPwd);
	FTPrcvData(&Msg);
	if(Msg[0]=='4' || Msg[0]=='5')
	{
		std::cout<<"FTP password error"<<endl;
		return -3;
	}
	return 1;
}

int FTPSender::FTPsendData(string data)
{
	int iResult;
	string sdata(data.begin(),data.end());
	sdata.append(nl);
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