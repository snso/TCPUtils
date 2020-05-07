#pragma once
#include <vector>
#include <mutex>
#include <atomic>

#include "IYTCPServer.h"
#include "CYServerTask.h"

#define MAX_LINK_NUM	10000

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
	
	WORK_STATUS		m_eWorkStats;

private:

	SOCKET						m_nSocket;
	
	std::mutex					m_mVectorSocket;
	std::vector<TCPClient*>		m_vTCPClient;
	std::atomic<bool>			m_bWork;

	std::mutex					m_mutexRecv;
	std::condition_variable		m_cvRecv;

	FunTCPServerRecv			m_fRecv;
	bool						m_bCreateRecv;

	///����Task
	std::mutex					m_mVectorTask;
	std::vector<CYServerTask*>	m_vServerTask;

private:

	void _AddClient(const SOCKET nSocket, const sockaddr_in sClientAddr);

	///�رտͻ���
	void _CloseClient(const TCPClient* pClient);

	///�������
	void _OnMonitorLink();
};
