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

	///��ʼ����
	bool StartTask(FunTCPServerRecv fRecv);

	///ֹͣ����
	bool StopTask();
	
	///���ӿͻ���
	bool AddClient(TCPClient* pClient);

	///�Ƴ��ͻ���
	bool DelClient(TCPClient* pClient);

	///��ȡTask�пͻ��˵���Ŀ
	int GetClientNum();



private:
	void _OnProcessEvent();

	bool _OnProcessData(TCPClient* pClient);

private:

	///�ͻ��˴洢vector
	std::mutex					m_mClient;
	std::vector<TCPClient *>	m_vClient;

	///select ���� ע�⣺��m_vClientʹ��ͬһ����
	int							m_maxSocket;
	fd_set						m_fdRead;
	fd_set						m_fdExp;

	///�߳�����
	std::thread*				m_tTask; 
	bool						m_bWork;
	WORK_STATUS					m_eWorkStatus;

	//�����ź�
	std::mutex					m_mutexWork;
	std::condition_variable		m_cvWork;

	//������[�ص�]
	FunTCPServerRecv			m_fRecv;
	char*						m_pRecvBuf;

		
};
