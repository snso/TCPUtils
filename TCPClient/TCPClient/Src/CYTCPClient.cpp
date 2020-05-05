
#include "CYTCPClient.h"
#include <stdio.h>


#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif // _WIN32

CYTCPClient::CYTCPClient()
	: m_nSocket(INVALID_SOCKET)
{
}

CYTCPClient::~CYTCPClient()
{
}

///≥ı ºªØsocket
bool CYTCPClient::Init()
{
	bool bRet = false;
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
#endif // _WIN32
	
	m_nSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_nSocket != INVALID_SOCKET)
	{
		bRet = true;
	}
	else
	{
		bRet = false;
	}

	return bRet;
}

bool CYTCPClient::Connect(const char* sIP, const int nPort)
{
	if (m_nSocket == INVALID_SOCKET)
	{
		return false;
	}

	bool bRet = true;
	sockaddr_in sSockAddr;
	sSockAddr.sin_family = AF_INET;
	sSockAddr.sin_port = htons(nPort);//host to net unsigned short
#ifdef _WIN32
	sSockAddr.sin_addr.S_un.S_addr = inet_addr(sIP);
#else
	sSockAddr.sin_addr.s_addr = inet_addr(sIP);
#endif // _WIN32

	int nRet = connect(m_nSocket, (sockaddr*)&sSockAddr, sizeof(sockaddr_in));

	if (SOCKET_ERROR != nRet)
	{
		bRet = true;
	}
	else
	{
		bRet = false;
	}

	return bRet;
}
 
bool CYTCPClient::Close()
{
	if (INVALID_SOCKET != m_nSocket)
	{
#ifdef _WIN32
		closesocket(m_nSocket);
#else
		close(m_nSocket);
#endif
		m_nSocket = INVALID_SOCKET;
	}
	return true;
}

bool CYTCPClient::SendData(const char* pData, const int nLen)
{
	if (INVALID_SOCKET == m_nSocket)
	{
		return false;
	}
	int nCount = send(m_nSocket, pData, nLen, 0);
	if (nCount <= 0)
	{
		return false;
	}
	return true;
}

bool CYTCPClient::RecvData(char** pData, int* nLen)
{
	if (INVALID_SOCKET == m_nSocket)
	{
		return false;
	}
	int nCount = recv(m_nSocket, *pData, 1024, 0);
	if (nCount <= 0)
	{
		return false;
	}

	*nLen = nCount;
	return true;
}

bool CreatYTCPClient(IYTCPClient** ppIYTCPClient)
{
	CYTCPClient* pYTCPClient = new CYTCPClient;
	*ppIYTCPClient = pYTCPClient;
	return true;
}