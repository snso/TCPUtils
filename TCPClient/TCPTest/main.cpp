#include <iostream>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <vector>

#include "IYTCPClient.h"

#pragma comment(lib, "TCPClient.lib")

#define TCP_SERVER_IP "192.168.1.120"
//#define TCP_SERVER_IP "127.0.0.1"
#define TCP_PORT 1233

bool isRun = true;

void CmdThread()
{

}

void RunClient(IYTCPClient* pIYTCPClient, char* pData, int nLen, unsigned int nIndex)
{
	char sInput[1024];
	memcpy(sInput, &nIndex, sizeof(nIndex));
	
	int len = sizeof(sInput);
	if (!pIYTCPClient->SendData(sInput, len)) 
	{
		printf("send error\n");
		getchar();
	}

	if (pData && nLen != 0 && pIYTCPClient->RecvData(&pData, &nLen))
	{
		//printf("recvLen : %d - %s\n", nLen, pData);
	}
}

void ClientEvent(const int nCount)
{
	std::vector<IYTCPClient *> vClient;
	for (int i = 0 ; i < nCount; i++)
	{
		IYTCPClient* pIYTCPClient = nullptr;
		bool bRet = CreatYTCPClient(&pIYTCPClient);
		bRet = pIYTCPClient->Init();
		if (bRet)
		{
			bRet = pIYTCPClient->Connect(TCP_SERVER_IP, TCP_PORT);
			if (bRet)
			{
				vClient.push_back(pIYTCPClient);
			}
			else
			{
				printf("connect error\n");
				delete pIYTCPClient;
			}
		}
	}

	unsigned int nIndex = 0;
	while (isRun)
	{
		for (int i = vClient.size() - 1; i >= 0; i--)
		{
			RunClient(vClient[i], nullptr, 0, nIndex++);
		}
	}

	for (int i = 0; i < nCount; i++)
	{
		delete vClient[i];
	}
	vClient.clear();
}

int main()
{
	std::thread ctrlThead(CmdThread);
	ctrlThead.detach();

	const int nEventNum = 1000;
	const int nThreadNum = 2;

	for (int i = 0; i < nThreadNum; i++)
	{
		std::thread runThread(ClientEvent, nEventNum / nThreadNum);
		runThread.detach();
	}
	getchar();
	return 0;
}

