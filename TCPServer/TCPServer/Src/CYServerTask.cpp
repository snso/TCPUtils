#include "CYServerTask.h"

CYServerTask::CYServerTask()
	: m_maxSocket(INVALID_SOCKET)
	, m_tTask(nullptr)
	, m_bWork(false)
	, m_eWorkStatus(WORK_INIT)
	, m_fRecv(nullptr)
{
	m_vClient.clear();
	FD_ZERO(&m_fdRead);
	FD_ZERO(&m_fdExp);

	m_pRecvBuf = new char[RECV_DATA_LEN];
}

CYServerTask::~CYServerTask()
{
	delete m_tTask;
	m_tTask = nullptr;

	m_vClient.clear();
	FD_ZERO(&m_fdRead);
	FD_ZERO(&m_fdExp);

	delete[] m_pRecvBuf;
}

bool CYServerTask::StartTask(FunTCPServerRecv fRecv)
{
	//1����ʼ����
	m_bWork = true;
	//2����ֵ�ص�����
	m_fRecv = fRecv;

	if (!m_tTask)
	{
		m_tTask = new std::thread(std::mem_fn(&CYServerTask::_OnProcessEvent), this);
		m_tTask->detach();
	}
	else
	{
		//�����źŽ��п�ʼ
		while (WORK_RUNING != m_eWorkStatus)
		{
			std::unique_lock<std::mutex> lock(m_mutexWork);
			m_cvWork.notify_one();
		}
	}

	return true;
}

bool CYServerTask::StopTask()
{
	m_bWork = false;
	///��������˿�ʼ�ŵȴ�����
	if (m_tTask)
	{
		while (WORK_WAITSINGAL != m_eWorkStatus)
		{
			std::chrono::milliseconds milTime(1);
			std::this_thread::sleep_for(milTime);
		}
		m_fRecv = nullptr;
		delete m_tTask;
		m_tTask = nullptr;
	}
	return true;
}

///���ӿͻ���
bool CYServerTask::AddClient(TCPClient * pClient)
{
	if(nullptr != pClient && m_vClient.size() < FD_SETSIZE)
	{
		std::lock_guard<std::mutex> mLck(m_mClient);
		//��¼Task�еĿͻ���
		m_vClient.push_back(pClient);
		//���ӵ�select��fd������
		FD_SET(pClient->m_nFD, &m_fdRead);
		FD_SET(pClient->m_nFD, &m_fdExp);
		//�ҵ����FD
		m_maxSocket = m_maxSocket > pClient->m_nFD ? m_maxSocket : pClient->m_nFD;
		return true;
	}
	else
	{
		return false;
	}
}

///�Ƴ��ͻ���
bool CYServerTask::DelClient(TCPClient * pClient)
{
	bool bRet = false;
	std::lock_guard<std::mutex> mLck(m_mClient);
	std::vector<TCPClient*>::iterator iter = m_vClient.begin();
	m_maxSocket = INVALID_SOCKET;
	while (iter != m_vClient.end())
	{
		if (*iter == pClient)
		{
			m_vClient.erase(iter);
			FD_CLR(pClient->m_nFD, &m_fdRead);
			FD_CLR(pClient->m_nFD, &m_fdExp);
			bRet = true;
		}
		else
		{
			//����ͳ�����ֵ
			m_maxSocket = m_maxSocket > (*iter)->m_nFD ? m_maxSocket : (*iter)->m_nFD;
			iter++;
		}
	}
	return bRet;
}

///��ȡTask�пͻ��˵���Ŀ
int CYServerTask::GetClientNum()
{
	return (int)m_vClient.size();
}

void CYServerTask::_OnProcessEvent()
{
	while (true)
	{
		if (!m_bWork)
		{
			m_eWorkStatus = WORK_WAITSINGAL;
			std::unique_lock<std::mutex> lock(m_mutexWork);
			m_cvWork.wait(lock);
			if (WORK_CLOSE == m_eWorkStatus)
			{
				break;
			}
			else
			{
				m_eWorkStatus = WORK_RUNING;
			}
		}
		else
		{

			if (m_vClient.empty())
			{
				std::chrono::milliseconds milTime(1);
				std::this_thread::sleep_for(milTime);
				continue;
			}

			
			///��ʱ�ͻ���vector
			std::vector<TCPClient*>	vTCPClient;
			fd_set	fdRead;
			fd_set	fdExp;
			{
				std::unique_lock<std::mutex> lock(m_mClient);
				vTCPClient = m_vClient;
				fdRead = m_fdRead;
				fdExp = m_fdExp;
			}

			struct timeval sWaitTime = { 0, 1000 };
			int nCount = select((int)m_maxSocket + 1, &fdRead, nullptr, &fdExp, &sWaitTime);

			if (nCount <= 0)
			{
				continue;
			}


			for (int i = (int)vTCPClient.size() - 1; i >= 0; i--)
			{
				if (FD_ISSET(vTCPClient[i]->m_nFD, &fdRead))
				{
					//���ܲ��ص�
					if (!_OnProcessData(vTCPClient[i]))
					{
					//	_CloseClient(vTCPClient[i]);
					}
				}
			}
		}
	}
	m_eWorkStatus = WORK_EXIT;
}

bool CYServerTask::_OnProcessData(TCPClient* pClient)
{
	const int nRecvLen = RECV_DATA_LEN;
	int nCount = recv(pClient->m_nFD, m_pRecvBuf, nRecvLen, 0);

	if (nCount > 0 && m_fRecv)
	{
		m_fRecv(pClient, m_pRecvBuf, nCount);
	}
	return nCount > 0;
}
