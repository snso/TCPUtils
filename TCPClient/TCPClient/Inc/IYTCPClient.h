#pragma once
#include "IYExport.h"

 class IYTCPClient
{
public:
	IYTCPClient() {}
	virtual ~IYTCPClient() {}

public:
	 ///≥ı ºªØsocket
	 virtual bool Init() = 0;

	 virtual bool Connect(const char* sIP, const int nPort) = 0;

	 virtual bool Close() = 0;

	 virtual bool SendData(const char* pData, const int nLen) = 0;

	 virtual bool RecvData(char** pData, int* nLen) = 0;
	
};

 IYAPI bool CreatYTCPClient(IYTCPClient** ppIYTCPClient);
