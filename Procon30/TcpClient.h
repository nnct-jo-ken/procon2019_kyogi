#pragma once
#include <WinSock2.h>
#include <string>

class TcpClient
{
public:
	WSADATA wsaData;
	struct sockaddr_in server;
	SOCKET sock;

	TcpClient();
	void tcp_connect();
	std::string tcp_recv();
	void tcp_send(std::string);
	void tcp_close();
};

