#pragma once
#include <winsock2.h>
class TCP_Server
{
public:
	WSADATA			wsaData;
	SOCKET			sock0;
	SOCKADDR_IN		addr;
	SOCKADDR_IN		client;
	int				len;
	SOCKET			sock;
	fd_set			fds, readfds;

	void init(u_short port);
	void tcp_open();
	bool tcp_accept();
	void tcp_send(char *buf, int buflen);
	int tcp_recv(char *buf, int buflen);
	void tcp_close();
};

