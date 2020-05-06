#pragma once
#include <vector>
#include <mutex>

#include "IYTCPServer.h"

#define RECV_DATA_LEN	1 * 1024 * 1024

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

enum WORK_STATUS
{
	WORK_INIT,
	WORK_RUNING,
	WORK_WAITSINGAL,
	WORK_CLOSE,
	WORK_EXIT,
};

class CYServerTask
{
public:
	CYServerTask();
	~CYServerTask();

	///开始任务
	bool StartTask(FunTCPServerRecv fRecv);

	///停止任务
	bool StopTask();
	
	///增加客户端
	bool AddClient(TCPClient* pClient);

	///移除客户端
	bool DelClient(TCPClient* pClient);

	///获取Task中客户端的数目
	int GetClientNum();



private:
	void _OnProcessEvent();

	bool _OnProcessData(TCPClient* pClient);

private:

	///客户端存储vector
	std::mutex					m_mClient;
	std::vector<TCPClient *>	m_vClient;

	///select 集合 注意：与m_vClient使用同一个锁
	int							m_maxSocket;
	fd_set						m_fdRead;
	fd_set						m_fdExp;

	///线程任务
	std::thread*				m_tTask; 
	bool						m_bWork;
	WORK_STATUS					m_eWorkStatus;

	//工作信号
	std::mutex					m_mutexWork;
	std::condition_variable		m_cvWork;

	//处理函数[回调]
	FunTCPServerRecv			m_fRecv;
	char*						m_pRecvBuf;

		
};
