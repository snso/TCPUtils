#pragma once
#include "IYTCPClient.h"


#ifdef _WIN32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#else
#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)

#endif

class CYTCPClient : public IYTCPClient
{
public:
	CYTCPClient();
	~CYTCPClient();


public:
	
	///≥ı ºªØsocket
	virtual bool Init();

	virtual bool Connect(const char* sIP, const int nPort);

	virtual bool Close();

	virtual bool SendData(const char* pData, const int nLen);

	virtual bool RecvData(char** pData, int* nLen);

private:

	SOCKET		m_nSocket;


};
