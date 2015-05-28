#include "Win32Sockets.h"

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <ppl.h>
#include <ppltasks.h>

Win32Sockets::Win32Sockets()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
		throw std::runtime_error("Error at WSAStartup");
}


Win32Sockets::~Win32Sockets()
{
	WSACleanup();
}

void Win32Sockets::CheckKAV()
{
	auto retCode = 0;
	SOCKET listenerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	assert(0 == WSAGetLastError());

	auto localHostIp4 = "127.0.0.1";
	auto localPortListener = 5467;

	sockaddr_in listenerAddr;
	listenerAddr.sin_family = AF_INET;
	listenerAddr.sin_addr.S_un.S_addr = inet_addr(localHostIp4);
	listenerAddr.sin_port = htons(localPortListener);
	assert((retCode = bind(listenerSocket, (const sockaddr *)&listenerAddr, sizeof(listenerAddr))) == 0);
	assert((retCode = listen(listenerSocket, 1)) == 0);
	OutputDebugStringW(L"Listener started\n");

	auto acceptTask = concurrency::create_task([listenerSocket]()
	{
		OutputDebugStringW(L"Accept started\n");
		sockaddr_in acceptedAddr;
		int acceptedLen = sizeof(acceptedAddr);
		auto acceptedSocket = accept(listenerSocket, (sockaddr *)&acceptedAddr, &acceptedLen);
		assert(acceptedSocket != INVALID_SOCKET);	// <---- KAV prevent accept operation
		OutputDebugStringW(L"Accept established\n");
		concurrency::wait(5000);
		closesocket(acceptedSocket);
		OutputDebugStringW(L"Accept finished\n");
	});

	SOCKET connectedSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	assert(0 == WSAGetLastError());
	OutputDebugStringW(L"Connect started\n");
	assert((retCode = connect(connectedSocket, (const sockaddr *)&listenerAddr, sizeof(listenerAddr))) == 0);
	OutputDebugStringW(L"Connect established\n");

	acceptTask.wait();

	assert((retCode = closesocket(connectedSocket)) == 0);
	assert((retCode = closesocket(listenerSocket)) == 0);
}
