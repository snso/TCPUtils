
#include "CYTCPServer.h"
#include "IYNet_Errcode.h"
#include <stdio.h>
#include <thread>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#include <WS2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <functional>
#endif // _WIN32

CYTCPServer::CYTCPServer()
	: m_eWorkStats(WORK_INIT)
	, m_nSocket(INVALID_SOCKET)
	, m_bWork(false)
	, m_fRecv(nullptr)
	, m_bCreateRecv(false)
{
	m_vTCPClient.clear();
}

CYTCPServer::~CYTCPServer()
{
	Close();
}

///初始化socket
bool CYTCPServer::Init(const int port)
{
	bool bRet = false;
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
#endif // _WIN32
	
	m_nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_nSocket != INVALID_SOCKET)
	{
		bRet = true;
		sockaddr_in sSockAddr;
		sSockAddr.sin_family = AF_INET;
		sSockAddr.sin_port = htons(port);//host to net unsigned short
#ifdef _WIN32
		sSockAddr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
		sSockAddr.sin_addr.s_addr = INADDR_ANY;
#endif // _WIN32

		int nRet = bind(m_nSocket, (sockaddr*)&sSockAddr, sizeof(sockaddr_in));
		if (SOCKET_ERROR != nRet)
		{
			nRet = listen(m_nSocket, 50);
			if (SOCKET_ERROR == nRet)
			{
				bRet = false;
			}
		}
		else
		{
			bRet = false;
		}

		if (!bRet)
		{
#ifdef _WIN32
			closesocket(m_nSocket);
#else
			close(m_nSocket);	
#endif
			m_nSocket = INVALID_SOCKET;
		}
	}
	else
	{
		bRet = false;
	}

	return bRet;
}

bool CYTCPServer::Start(FunTCPServerRecv fRecv)
{
	//1、开始工作
	m_bWork = true;
	//2、赋值回调函数
	m_fRecv = fRecv;

	//3、注册回调函数，创建接受线程，如回调函数为null 将需要主动调用RecvData接口
	if (!m_bCreateRecv)
	{
		//如果创建了连接线程将不再重复创建
		std::thread tLinkThread(std::bind(&CYTCPServer::_OnMonitorLink, this));
		tLinkThread.detach();
		//首次进入直接进入runing 状态
		m_eWorkStats = WORK_RUNING;
		m_bCreateRecv = true;

		///创建服务端Task
		std::lock_guard<std::mutex> oLck(m_mVectorTask);
		for (int i = floor(MAX_CLIENT_NUM / FD_SETSIZE); i >= 0; i--)
		{
			CYServerTask* pServerTask = new CYServerTask();
			m_vServerTask.push_back(pServerTask);
			pServerTask->StartTask(fRecv);
		}
	}
	else
	{
		//发送信号进行开始
		while (WORK_RUNING != m_eWorkStats)
		{
			std::unique_lock<std::mutex> lock(m_mutexRecv);
			m_cvRecv.notify_one();
		}
	}
	return true;
}

bool CYTCPServer::Stop()
{
	m_bWork = false;
	///如果调用了开始才等待结束
	if(m_bCreateRecv)
	{
		while (WORK_WAITSINGAL != m_eWorkStats)
		{
			std::chrono::milliseconds milTime(1);
			std::this_thread::sleep_for(milTime);
		}
		m_fRecv = nullptr;

		///关闭服务端Task
		std::lock_guard<std::mutex> oLck(m_mVectorTask);
		std::vector<CYServerTask*>::iterator iter = m_vServerTask.begin();
		while(iter != m_vServerTask.end())
		{
			(*iter)->StopTask();
			iter = m_vServerTask.erase(iter);
		}
	}
	m_bCreateRecv = false;
	return true;
} 

///发送信息
bool CYTCPServer::SendData(const TCPClient* pClient, const char* nBuf, const int nLen)
{
	if (nullptr == pClient || pClient->m_nFD == INVALID_SOCKET)
	{
		return false;
	}
	int bRet = false;
	std::vector<TCPClient*>	vTCPClient;
	{
		std::unique_lock<std::mutex> lock(m_mVectorSocket);
		vTCPClient = m_vTCPClient;
	}

	for (int i = (int)vTCPClient.size() - 1; i >= 0; i--)
	{
		if (pClient->m_nFD == vTCPClient[i]->m_nFD)
		{
			int nSendLen = 0;
			while (nSendLen < nLen)
			{
				int len = send(pClient->m_nFD, nBuf + nSendLen, nLen - nSendLen, 0);
				if (len <= 0)
				{
					bRet = false;
					break;
				}
				nSendLen += len;
			}

			if (nSendLen == nLen)
			{
				bRet = true;
			}
		}
	}
	return bRet;
}

///发送给所有客户端
bool CYTCPServer::SendAllData(const char* nBuf, const int nLen)
{
	int bRet = false;
	std::vector<TCPClient*>	vTCPClient;
	{
		std::unique_lock<std::mutex> lock(m_mVectorSocket);
		vTCPClient = m_vTCPClient;
	}

	for (int i = (int)vTCPClient.size() - 1; i >= 0; i--)
	{
		int nSendLen = 0;
		while (nSendLen < nLen)
		{
			int len = send(vTCPClient[i]->m_nFD, nBuf + nSendLen, nLen - nSendLen, 0);
			if (len <= 0)
			{
				_CloseClient(vTCPClient[i]);
				bRet = false;
				break;
			}
			nSendLen += len;
		}

		if (nSendLen == nLen)
		{
			bRet = true;
		}
		
	}
	return bRet;
}

///接受数据
bool CYTCPServer::RecvData(unsigned char** ppData, unsigned int* nLen)
{
	bool bRet = true;


	return bRet;
}


bool CYTCPServer::Close()
{
	if (INVALID_SOCKET != m_nSocket)
	{
		if (WORK_RUNING == m_eWorkStats)
		{
			Stop();
		}

#ifdef _WIN32
		closesocket(m_nSocket);
		WSACleanup();
#else
		close(m_nSocket);
#endif
		m_nSocket = INVALID_SOCKET;

		m_eWorkStats = WORK_CLOSE;

		while (m_eWorkStats != WORK_EXIT)
		{
			std::unique_lock<std::mutex> lock(m_mutexRecv);
			m_cvRecv.notify_one();

			std::chrono::milliseconds milTime(1);
			std::this_thread::sleep_for(milTime);

		}	
	}
	return true;
}

void CYTCPServer::_AddClient(const SOCKET nSocket, const sockaddr_in sClientAddr)
{
	printf("index : %d welcome [%d]: %s\n", m_vTCPClient.size(), (int)nSocket, inet_ntoa(sClientAddr.sin_addr));
	TCPClient* pTCPClient = new TCPClient;
	pTCPClient->m_nFD = (int)nSocket;
	{
		std::unique_lock<std::mutex> lock(m_mVectorSocket);
		m_vTCPClient.push_back(pTCPClient);
	}
	{
		std::lock_guard<std::mutex> taskLock(m_mVectorTask);
		std::vector<CYServerTask*>::iterator iterMin = m_vServerTask.begin();
		std::vector<CYServerTask*>::iterator iter = m_vServerTask.begin();
		while (iter != m_vServerTask.end())
		{
			iterMin = (*iterMin)->GetClientNum() <= (*iter)->GetClientNum() ? iterMin : iter;
			iter++;
		}

		if (iterMin != m_vServerTask.end())
		{
			(*iterMin)->AddClient(pTCPClient);
		}
	}
}

void CYTCPServer::_CloseClient(const TCPClient* pClient)
{
	std::unique_lock<std::mutex> lock(m_mVectorSocket);
	std::vector<TCPClient*>::iterator iter = m_vTCPClient.begin();
	while(iter != m_vTCPClient.end())
	{
		if(*iter == pClient)
		{
			printf("client exit [%d]\n", (int)pClient->m_nFD);
			delete pClient;
			iter = m_vTCPClient.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

void CYTCPServer::_OnMonitorLink()
{
	//select
	fd_set	fdRead;
	fd_set	fdExp;

	while (true)
	{
		if (!m_bWork)
		{
			m_eWorkStats = WORK_WAITSINGAL;
			std::unique_lock<std::mutex> lock(m_mutexRecv);
			m_cvRecv.wait(lock);
			if (WORK_CLOSE == m_eWorkStats)
			{
				break;
			}
			else
			{
				m_eWorkStats = WORK_RUNING;
			}
		}
		else
		{
			FD_ZERO(&fdRead);
			FD_ZERO(&fdExp);

			FD_SET(m_nSocket, &fdRead);
			FD_SET(m_nSocket, &fdExp);

			struct timeval sWaitTime = {0, 1000};
			int nCount = select((int)m_nSocket + 1, &fdRead, nullptr, &fdExp, &sWaitTime);

			if (nCount <= 0)
			{
				continue;
			}

			if (FD_ISSET(m_nSocket, &fdRead))
			{
				FD_CLR(m_nSocket, &fdRead);
				sockaddr_in sClientAddr;
				int nAddrLen = sizeof(sockaddr_in);
				SOCKET nSocket = accept(m_nSocket, (sockaddr*)&sClientAddr, (socklen_t *)&nAddrLen);

				if (INVALID_SOCKET != nSocket)
				{
					_AddClient(nSocket, sClientAddr);
					
				}
			}

		}
	}
	m_eWorkStats = WORK_EXIT;
}

bool CreatYTCPServer(IYTCPServer** ppIYTCPServer)
{
	CYTCPServer* pYTCPServer = new CYTCPServer();
	*ppIYTCPServer = pYTCPServer;
	return true;
}