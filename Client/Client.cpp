#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define NOMINMAX


#include <iostream>
#include <WinSock2.h>
#include <conio.h>
#include "rapidjson/Document.h"
#include "rapidjson/writer.h"
#include "rapidjson/StringBuffer.h"


#pragma comment(lib, "ws2_32")

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = PF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerSockAddr.sin_port = htons(30303);

	connect(ServerSock, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	int X = 0;
	int Y = 0;
	std::cout << "*";
	while (true)
	{
		char Input;
		if (_kbhit())
		{
			system("cls");
			Input = _getch();
		}
		else
		{
			continue;
		}
		rapidjson::Document d;

		d.SetObject();
		d.AddMember("Input", Input, d.GetAllocator());

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		d.Accept(writer);

		int PacketSize = (int)buffer.GetSize();
		PacketSize = htonl(PacketSize);
		int SentBytes = send(ServerSock, (char*)&PacketSize, sizeof(PacketSize), 0);
		SentBytes = send(ServerSock, buffer.GetString(), (int)buffer.GetSize(), 0);

		char RecvBuffer[65535] = { 0, };
		int RecvBytes = recv(ServerSock, (char*)&PacketSize, sizeof(PacketSize), MSG_WAITALL);
		PacketSize = ntohl(PacketSize);
		RecvBytes = recv(ServerSock, RecvBuffer, PacketSize, MSG_WAITALL);

		d.RemoveAllMembers();
		d.SetObject();
		d.Parse(RecvBuffer);

		X = d["X"].GetInt();
		Y = d["Y"].GetInt();
		COORD Cur;
		Cur.X = X;
		Cur.Y = Y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
		std::cout << "*";
	}

	closesocket(ServerSock);

	WSACleanup();

	return 0;
}