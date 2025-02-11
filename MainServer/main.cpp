#include "IOCP/IOCP.h"
#include <Functions/Minidump/Minidump.hpp>

int main() {
	std::string sConfigJsonFilePath = "C:\\Users\\Move_Daegu24\\source\\repos\\Racing-Game-Server\\Shared\\Json\\Config.json";

	MAINSERVER::CIOCP MainServer(sConfigJsonFilePath);
	SocketAddress bindAddress("127.0.0.1", 3550);
	if (MainServer.Initialize(EPROTOCOLTYPE::EPT_TCP, bindAddress)) {
		while (MainServer.GetIOCPRunState())
			MainServer.Run();

		MainServer.Destroy();
	}
	return 0;
}