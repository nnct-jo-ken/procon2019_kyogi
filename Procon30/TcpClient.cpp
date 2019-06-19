#include "TcpClient.h"

TcpClient::TcpClient()
{
	WSAStartup(MAKEWORD(2, 0), &wsaData);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	server.sin_family = AF_INET;
	server.sin_port = htons(2525);
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
}

void TcpClient::tcp_connect()
{
	connect(sock, (struct sockaddr*)&server, sizeof(server));
}

std::string TcpClient::tcp_recv()
{
	char buf[8192];
	memset(buf, 0, 8192);
	recv(sock, buf, sizeof(buf), 0);
	return std::string(buf);
}

void TcpClient::tcp_send(std::string str)
{
	send(sock, str.c_str(), str.length(), 0);
}