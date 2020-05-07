#pragma once
#include <functional>
#include "IYExport.h"

#define MAX_CLIENT_NUM 10000

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
	 ///��ʼ��socket
	 virtual bool Init(const int port) = 0;

	 ///��ʼ����
	 virtual bool Start(FunTCPServerRecv fRecv) = 0;

	 ///ֹͣ����
	 virtual bool Stop() = 0;

	 ///������Ϣ
	 virtual bool SendData(const TCPClient* pClient, const char* nBuf, const int nLen) = 0;

	 ///���͸����пͻ���
	 virtual bool SendAllData(const char* nBuf, const int nLen) = 0;

	 ///��������
	 virtual bool RecvData(unsigned char** ppData, unsigned int* nLen) = 0;

	 ///�ر�
	 virtual bool Close() = 0;
	
};

 IYAPI bool CreatYTCPServer(IYTCPServer** ppIYTCPServer);
