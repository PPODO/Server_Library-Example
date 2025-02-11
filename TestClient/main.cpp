#include "EventSelect/Client.h"

int main() {
	WSADATA winsockData;

	WSAStartup(MAKEWORD(2, 2), &winsockData);

	auto hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in address;
	
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_family = AF_INET;
	address.sin_port = htons(3560);

	sendto(hSocket, "Hello World", strlen("Hello World"), 0, reinterpret_cast<sockaddr*>(&address), sizeof(address));

	closesocket(hSocket);
	WSACleanup();
	return 0;
}