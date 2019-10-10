#pragma once
#include <WinSock2.h>

class TCP_Client
{
private:
	WSADATA wsaData;
	struct sockaddr_in server;
	SOCKET sock;

public:
	TCP_Client();
	void tcp_connect(u_short port);
	int tcp_recv(char *buf);
};

