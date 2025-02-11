#include "Client.h"
#include <Network/Packet/Serialization/serialization.hpp>
#include "../../Shared/Util/Util.h"

CClient::CClient() : EventSelect(2, m_packetProcessor) {
	m_packetProcessor.emplace(FlatInGamePacket::PacketType::PacketType_Join, std::bind(&CClient::JoinTest, this, std::placeholders::_1));
}

bool CClient::Initialize(const EPROTOCOLTYPE protocolType, SERVER::FUNCTIONS::SOCKETADDRESS::SocketAddress& serverAddress) {
	if (EventSelect::Initialize(protocolType, serverAddress)) {

		return true;
	}
	return false;
}

void CClient::Run() {
	EventSelect::Run();

	flatbuffers::FlatBufferBuilder builder(4096);
	std::vector<flatbuffers::Offset<FlatInGamePacket::Game::UserSpawnInfo>> usersInfo;
	usersInfo.push_back(FlatInGamePacket::Game::CreateUserSpawnInfo(builder, 10, builder.CreateString("asdasdasd")));

	SendTo(true, CreateJoinInGamePacket(builder, usersInfo));
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void CClient::Destroy() {
	EventSelect::Destroy();

}

void CClient::JoinTest(SERVER::NETWORK::PACKET::PacketQueueData* const pQueueData) {
	if (auto pPacket = FlatInGamePacket::Game::GetJoinGamePacket(pQueueData->m_packetData->m_sPacketData)) {
		for (auto iterator = pPacket->joined_users()->begin(); pPacket->joined_users()->cend() != iterator; ++iterator)
			SERVER::FUNCTIONS::LOG::Log::WriteLog(L"[%d], [%ls], [%d]", iterator->uuid(), SERVER::FUNCTIONS::UTIL::MBToUni(iterator->user_name()->c_str()).c_str(), iterator->spawn_index());

	}
}
