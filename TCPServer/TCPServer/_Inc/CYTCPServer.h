#pragma once
#include "IYTCPServer.h"
#include <vector>
#include <mutex>
#include <atomic>

#ifdef _WIN32

#define FD_SETSIZE 1024
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#else
#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif

#define RECV_DATA_LEN	1 * 1024 * 1024
#define MAX_LINK_NUM	10000

class CYTCPServer : public IYTCPServer
{
public:
	CYTCPServer();
	~CYTCPServer();

public:
	
	///初始化socket
	virtual bool Init(const int port);

	///开始工作
	virtual bool Start(FunTCPServerRecv fRecv);

	///停止工作
	virtual bool Stop();

	///发送信息
	virtual bool SendData(const TCPClient* pClient, const char* nBuf, const int nLen);

	///发送给所有客户端
	virtual bool SendAllData(const char* nBuf, const int nLen);

	///接受数据
	virtual bool RecvData(unsigned char** ppData, unsigned int* nLen);

	///关闭
	virtual bool Close();

private:

	enum WORK_STATUS
	{
		WORK_INIT,
		WORK_RUNING,
		WORK_WAITSINGAL,
		WORK_CLOSE,
		WORK_EXIT,
	};

	WORK_STATUS		m_eWorkStats;

private:

	SOCKET						m_nSocket;
	SOCKET						m_maxScoket;

	std::mutex					m_mVectorSocket;
	std::vector<TCPClient*>		m_vTCPClient;
	std::atomic<bool>			m_bWork;

	std::mutex					m_mutexRecv;
	std::condition_variable		m_cvRecv;

	FunTCPServerRecv			m_fRecv;
	bool						m_bCreateRecv;

	char*						m_pRecvBuf;


private:
	///关闭客户端
	void _CloseClient(const TCPClient* pClient);

	///检测连接
	void _OnMonitorLink();

	///处理数据
	bool _OnProcessData(TCPClient* sClient);
};
