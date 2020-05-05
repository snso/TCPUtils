#pragma once
#include <functional>
#include "IYExport.h"

struct TCPClient
{
	int		m_nFD;
};

typedef void(*FunTCPServerRecv)(const TCPClient* pClient, const char* pData, const unsigned int nLen);

 class IYTCPServer
{

public:
	IYTCPServer() {}
	virtual ~IYTCPServer() {}

public:
	 ///初始化socket
	 virtual bool Init(const int port) = 0;

	 ///开始工作
	 virtual bool Start(FunTCPServerRecv fRecv) = 0;

	 ///停止工作
	 virtual bool Stop() = 0;

	 ///发送信息
	 virtual bool SendData(const TCPClient* pClient, const char* nBuf, const int nLen) = 0;

	 ///发送给所有客户端
	 virtual bool SendAllData(const char* nBuf, const int nLen) = 0;

	 ///接受数据
	 virtual bool RecvData(unsigned char** ppData, unsigned int* nLen) = 0;

	 ///关闭
	 virtual bool Close() = 0;
	
};

 IYAPI bool CreatYTCPServer(IYTCPServer** ppIYTCPServer);
