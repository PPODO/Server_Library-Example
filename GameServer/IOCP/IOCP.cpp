#include "IOCP.h"
#include <Functions/Log/Log.hpp>

GAMESERVER::CIOCP::CIOCP()
	: IOCP(m_packetProcessor, std::thread::hardware_concurrency() / 2), m_durationForTransmissionThread(std::chrono::milliseconds(250))
	, m_inGameSystem(std::bind(&CIOCP::AddNewTransmitData, this, std::placeholders::_1), std::chrono::milliseconds(25)), m_bIOCPRunState(true), m_bTransmissonThreadRunState(true) {

	m_packetProcessor.emplace(FlatInGamePacket::PacketType::PacketType_Join, std::bind(&CIOCP::PlayerJoinPacketProcessing, this, std::placeholders::_1));
	m_packetProcessor.emplace(FlatInGamePacket::PacketType::PacketType_Ready, std::bind(&CIOCP::PlayerReadyPakcetProcessing, this, std::placeholders::_1));
	m_packetProcessor.emplace(FlatInGamePacket::PacketType::PacketType_GameData, std::bind(&CIOCP::PlayerGameDataProcessing, this, std::placeholders::_1));

	m_pTransmissionProcessingQueue = std::make_unique<TRANSMITQUEUE>();
	m_pTransmissionStockQueue = std::make_unique<TRANSMITQUEUE>();

	m_transmissionThread = std::thread(std::bind(&CIOCP::TransmissionWorkerThread, this));
}

bool GAMESERVER::CIOCP::Initialize(const EPROTOCOLTYPE protocolType, SERVER::FUNCTIONS::SOCKETADDRESS::SocketAddress& bindAddress) {
	if (IOCP::Initialize(protocolType, bindAddress)) {
		EnableAckCheck(true);

		return true;
	}
	return false;
}

void GAMESERVER::CIOCP::Run() {
	IOCP::Run();
	
	m_inGameSystem.Update();
}

void GAMESERVER::CIOCP::Destroy() {
	m_bTransmissonThreadRunState = false;
	if (m_transmissionThread.joinable())
		m_transmissionThread.join();

	IOCP::Destroy();
}


void GAMESERVER::CIOCP::AddNewTransmitData(FTransmitQueueData* pNewTransmitData) {
	if (pNewTransmitData) {
		SERVER::FUNCTIONS::CRITICALSECTION::CriticalSectionGuard lck(m_csForTransmissionQueue);
		m_pTransmissionStockQueue->Push(pNewTransmitData);
	}
}

void GAMESERVER::CIOCP::TransmissionWorkerThread() {
	while (m_bTransmissonThreadRunState) {
		std::unique_lock<std::mutex> lck(m_csForTransmissionThread);
		if (m_cvForTransmissionThread.wait_for(lck, m_durationForTransmissionThread, [&]() { return !m_pTransmissionProcessingQueue->IsEmpty(); })) {
			lck.unlock();

			FTransmitQueueData* pQueueData = nullptr;
			if (m_pTransmissionProcessingQueue->Pop(pQueueData)) {
				if (pQueueData->m_bReliable)
					this->GetServerInstance()->SendToReliable(pQueueData->m_peerInformation, pQueueData->m_packetStructure);
				else
					this->GetServerInstance()->SendToUnReliable(pQueueData->m_peerInformation, pQueueData->m_packetStructure);

				delete pQueueData;
			}
		}
		else if (m_pTransmissionProcessingQueue->IsEmpty() && !m_pTransmissionStockQueue->IsEmpty()) {
			SERVER::FUNCTIONS::CRITICALSECTION::CriticalSectionGuard lck(m_csForTransmissionQueue);
			m_pTransmissionProcessingQueue.swap(m_pTransmissionStockQueue);
		}
	}
}

void GAMESERVER::CIOCP::PlayerJoinPacketProcessing(SERVER::NETWORK::PACKET::PacketQueueData* const pPacketData) {
	using namespace SERVER::FUNCTIONS::LOG;
	using namespace SERVER::FUNCTIONS::UTIL;

	if (auto pConnection = static_cast<CONNECTION*>(pPacketData->m_pOwner)) {
		if (auto pUserJoinPacket = FlatInGamePacket::Game::GetJoinGamePacket(pPacketData->m_packetData->m_sPacketData)) {
			int16_t iUserUniqueNumber = 0;
			auto pJoinUserInfo = pUserJoinPacket->joined_users()->Get(0);
			if (m_inGameSystem.JoinUser(pJoinUserInfo->uuid(), pJoinUserInfo->user_name()->c_str(), pConnection->m_peerInformation, iUserUniqueNumber))
				Log::WriteLog(L"User [%ls], [%d] Joined!", MBToUni(pJoinUserInfo->user_name()->c_str()).c_str(), pJoinUserInfo->uuid());
		}
	}
}

void GAMESERVER::CIOCP::PlayerReadyPakcetProcessing(SERVER::NETWORK::PACKET::PacketQueueData* const pPacketData) {
	using namespace SERVER::FUNCTIONS::LOG;
	using namespace SERVER::FUNCTIONS::UTIL;

	if (auto pConnection = static_cast<CONNECTION*>(pPacketData->m_pOwner)) {
		if (auto pReadyPacket = FlatInGamePacket::Game::GetPlayerReadyPacket(pPacketData->m_packetData->m_sPacketData)) {
			const FVector vLocation(pReadyPacket->init_vehicle_information()->location()->x(), pReadyPacket->init_vehicle_information()->location()->y(), pReadyPacket->init_vehicle_information()->location()->z());
			const FVector vRotation(pReadyPacket->init_vehicle_information()->rotation()->x(), pReadyPacket->init_vehicle_information()->rotation()->y(), pReadyPacket->init_vehicle_information()->rotation()->z());
			const FVector vVelocity(pReadyPacket->init_vehicle_information()->velocity()->x(), pReadyPacket->init_vehicle_information()->velocity()->y(), pReadyPacket->init_vehicle_information()->velocity()->z());

			if (m_inGameSystem.UpdatePlayerReadyState(pReadyPacket->uuid(), true, vLocation, vRotation, vVelocity))
				Log::WriteLog(L"User [%d] Is Ready!", pReadyPacket->uuid());
			else
				Log::WriteLog(L"User [%d] Can't Find Information!", pReadyPacket->uuid());
		}
	}
}

void GAMESERVER::CIOCP::PlayerGameDataProcessing(SERVER::NETWORK::PACKET::PacketQueueData* const pPacketData) {
	using namespace SERVER::FUNCTIONS::LOG;
	using namespace SERVER::FUNCTIONS::UTIL;

	if (auto pConnection = static_cast<CONNECTION*>(pPacketData->m_pOwner)) {
		if (auto pPlayerGameData = FlatInGamePacket::Game::GetPlayerGameDataPacket(pPacketData->m_packetData->m_sPacketData)) {
			const FVector vLocation(pPlayerGameData->vehicle_transform()->location()->x(), pPlayerGameData->vehicle_transform()->location()->y(), pPlayerGameData->vehicle_transform()->location()->z());
			const FVector vRotation(pPlayerGameData->vehicle_transform()->rotation()->x(), pPlayerGameData->vehicle_transform()->rotation()->y(), pPlayerGameData->vehicle_transform()->rotation()->z());
			const FVector vVelocity(pPlayerGameData->vehicle_transform()->velocity()->x(), pPlayerGameData->vehicle_transform()->velocity()->y(), pPlayerGameData->vehicle_transform()->velocity()->z());

			if (!m_inGameSystem.UpdatePlayerVehicleTransform(pPlayerGameData->uuid(), vLocation, vRotation, vVelocity, pPlayerGameData->vehicle_transform()->horizontal()))
				Log::WriteLog(L"User [%d] Not Exist!", pPlayerGameData->uuid());
		}
	}
}
