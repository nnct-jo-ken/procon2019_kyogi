#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TCP_Client.h"

TCP_Client::TCP_Client()
{
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	sock = socket(AF_INET, SOCK_STREAM, 0);
}

void TCP_Client::tcp_connect(u_short port)
{
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	connect(sock, (struct sockaddr*) & server, sizeof(server));
}

int TCP_Client::tcp_recv(char* buf)
{
	memset(buf, 0, sizeof(buf));
	return recv(sock, buf, sizeof(buf), 0);
}