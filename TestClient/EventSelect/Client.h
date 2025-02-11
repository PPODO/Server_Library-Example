#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#include <NetworkModel/EventSelect/EventSelect.hpp>

using namespace SERVER::NETWORKMODEL::BASEMODEL;
using namespace SERVER::NETWORKMODEL::EVENTSELECT;

class CClient : public EventSelect {
private:
	PACKETPROCESSOR m_packetProcessor;

public:
	CClient();

public:
	virtual bool Initialize(const EPROTOCOLTYPE protocolType, SERVER::FUNCTIONS::SOCKETADDRESS::SocketAddress& serverAddress) override final;
	virtual void Run() override final;
	virtual void Destroy() override final;

private:
	void JoinTest(SERVER::NETWORK::PACKET::PacketQueueData* const pQueueData);

};