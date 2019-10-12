#include "TCP_Server.h"

void TCP_Server::init(u_short port)
{
	WSAStartup(MAKEWORD(1, 1), &wsaData);
	sock0 = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(sock0, (struct sockaddr*) & addr, sizeof(addr));
}

void TCP_Server::tcp_open()
{
	listen(sock0, 0);
	len = sizeof(client);
}

bool TCP_Server::tcp_accept()
{
	sock = accept(sock0, (SOCKADDR*)& client, &len);
	return true;
}

void TCP_Server::tcp_send(char *buf, int buflen)
{
	send(sock, buf, buflen, 0);
}

int TCP_Server::tcp_recv(char* buf, int buflen)
{
	memset(buf, 0, buflen);
	return recv(sock, buf, buflen, 0);
}

void TCP_Server::tcp_close()
{
	closesocket(sock);
	WSACleanup();
}