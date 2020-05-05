#pragma once
#include "IYTCPServer.h"
#include <vector>
#include <mutex>
#include <atomic>

#define RECV_DATA_LEN 102400

class CYTCPServer : public IYTCPServer
{
public:
	CYTCPServer();
	~CYTCPServer();

public:
	
	///��ʼ��socket
	virtual bool Init(const int port);

	///��ʼ����
	virtual bool Start(FunTCPServerRecv fRecv);

	///ֹͣ����
	virtual bool Stop();

	///������Ϣ
	virtual bool SendData(const TCPClient* pClient, const char* nBuf, const int nLen);

	///���͸����пͻ���
	virtual bool SendAllData(const char* nBuf, const int nLen);

	///��������
	virtual bool RecvData(unsigned char** ppData, unsigned int* nLen);

	///�ر�
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


private:
	void _CloseClient(const TCPClient* pClient);

	void _OnMonitorLink();

	bool _OnProcessData(TCPClient* sClient);
};
