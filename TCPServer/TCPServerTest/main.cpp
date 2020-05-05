
#include <iostream>

#include "IYTCPServer.h"
#include "YTimeUtils.h"

#pragma comment(lib, "TCPServer.lib")
#pragma comment(lib, "Utils.lib")

#define TCP_PORT 1233

YTimeUtils*		m_pTimeUtils = new YTimeUtils;
unsigned int	m_nRecvSize  = 0;

IYTCPServer* pIYTCPServer = nullptr;

void recvFun(const TCPClient* pClient, const char* pData, const unsigned int nLen)
{
	m_nRecvSize += nLen;
	double dTime = m_pTimeUtils->GetElapsedSecond();
	if (dTime >= 1.0)
	{
		printf("recvSpeed:%.2f Mb/s\n", m_nRecvSize * 8 / 1014 / 1024 / dTime);
		m_nRecvSize = 0;
		m_pTimeUtils->Update();
	}
	
	if (pIYTCPServer)
	{
		//pIYTCPServer->SendData(pClient, pData, nLen);
	}
}

void RunSend()
{
	const int nLen = 1024;
	char cData[nLen];
	while (pIYTCPServer)
	{
		pIYTCPServer->SendAllData(cData, nLen);
	}
}

int main()
{
	bool bRet = CreatYTCPServer(&pIYTCPServer);
	bRet = pIYTCPServer->Init(TCP_PORT);
	pIYTCPServer->Start(recvFun);
	getchar();
	//pIYTCPServer->Stop();
	//pIYTCPServer->Close();
	delete pIYTCPServer;
	pIYTCPServer = nullptr;
	return 0;
}


