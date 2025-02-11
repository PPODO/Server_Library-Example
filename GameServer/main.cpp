#include "IOCP/IOCP.h"

int main() {
	GAMESERVER::CIOCP iocpInstance;

	SocketAddress bindAddress("127.0.0.1", 3560);
	if (iocpInstance.Initialize(EPROTOCOLTYPE::EPT_UDP, bindAddress)) {
		while (iocpInstance.GetIOCPRunState())
			iocpInstance.Run();

		iocpInstance.Destroy();
	}
	return 0;
}