#include "InGameSystem.h"
#include "../../IOCP/IOCP.h"

flatbuffers::Offset<FlatInGamePacket::Game::vector> GAMESERVER::CInGameSystem::CreateVector(flatbuffers::FlatBufferBuilder& builder, const FVector& vector) {
	return FlatInGamePacket::Game::Createvector(builder, vector.m_fX, vector.m_fY, vector.m_fZ);
}

flatbuffers::Offset<FlatInGamePacket::Game::VehicleInformation> GAMESERVER::CInGameSystem::CreateVehicleInformation(flatbuffers::FlatBufferBuilder& builder, const FVector& location, const FVector& rotation, const FVector& velocity, const float fHorizontal) {
	return FlatInGamePacket::Game::CreateVehicleInformation(builder, CreateVector(builder, location), CreateVector(builder, rotation), CreateVector(builder, velocity), fHorizontal);
}

bool GAMESERVER::CInGameSystem::CreateGameDataPacket(flatbuffers::FlatBufferBuilder& builder, std::vector<flatbuffers::Offset<FlatInGamePacket::Game::GameData>>& outputFlatPacket) {
	if (m_joinedUserInformation.size() <= 0)
		return false;

	outputFlatPacket.reserve(m_joinedUserInformation.size());

	for (const auto& iterator : m_joinedUserInformation) {
		if (iterator.m_bIsReady) {
			outputFlatPacket.push_back(FlatInGamePacket::Game::CreateGameData(builder, iterator.m_iUUID, CreateVehicleInformation(builder, iterator.m_vLastLocation, iterator.m_vLastRotation, iterator.m_vLastVelocity, iterator.m_fHorizontal)));
		}
	}

	return true;
}

void GAMESERVER::CInGameSystem::UpdatePlayerVehicleTransform(FInGameUserInformation& inGameUserInformation, const FVector& vLocation, const FVector& vRotation, const FVector& vVelocity, const float fHorizontal) {
	inGameUserInformation.m_vLastLocation = vLocation;
	inGameUserInformation.m_vLastRotation = vRotation;
	inGameUserInformation.m_vLastVelocity = vVelocity;
	inGameUserInformation.m_fHorizontal = fHorizontal;
}

GAMESERVER::CInGameSystem::CInGameSystem(const std::function<void(FTransmitQueueData*)>& callbackFunc, const std::chrono::milliseconds& updateDuration) : m_broadcastCallbackFunc(callbackFunc), m_updateDuration(updateDuration) {
	m_lastUpdateTime = std::chrono::system_clock::now();
}

void GAMESERVER::CInGameSystem::Update() {
	auto currentTimePoint = std::chrono::system_clock::now();
	if (std::chrono::duration_cast<std::chrono::milliseconds>((currentTimePoint - m_lastUpdateTime)) >= m_updateDuration) {
		if (auto flatBuffer = std::make_unique<FFlatBuffer>()) {
			std::vector<flatbuffers::Offset<FlatInGamePacket::Game::GameData>> usersGameData;
			if (CreateGameDataPacket(flatBuffer->m_flatBuffer, usersGameData))
				Broadcast(::CreateGameDataPacket(flatBuffer->m_flatBuffer, usersGameData), false);
		}
		m_lastUpdateTime = currentTimePoint;
	}
}

bool GAMESERVER::CInGameSystem::JoinUser(const int32_t iUUID, const std::string& sUserName, const SERVER::NETWORK::PROTOCOL::UDP::PeerInfo& peerInfo, int16_t& iOutputUserNumber) {
	const auto& findResult = std::find_if(m_joinedUserInformation.begin(), m_joinedUserInformation.end(), [&](const FInGameUserInformation& val) {
		return val.m_iUUID == iUUID;
		});
	if (findResult != m_joinedUserInformation.cend())
		return false;

	const int16_t iUserUniqueNumber = m_joinedUserInformation.size() + 1;
	if (auto flatBuffer = std::make_unique<FFlatBuffer>()) {
		std::vector<flatbuffers::Offset<FlatInGamePacket::Game::UserSpawnInfo>> usersSpawnInformation;

		usersSpawnInformation.push_back(FlatInGamePacket::Game::CreateUserSpawnInfoDirect(flatBuffer->m_flatBuffer,  iUUID, sUserName.c_str(), iUserUniqueNumber));

		Broadcast(CreateJoinInGamePacket(flatBuffer->m_flatBuffer, usersSpawnInformation), true);

		m_joinedUserInformation.emplace_back(iUUID, sUserName, iUserUniqueNumber, peerInfo);
		iOutputUserNumber = iUserUniqueNumber;

		flatBuffer->m_flatBuffer.Clear();
		usersSpawnInformation.clear();
		usersSpawnInformation.reserve(m_joinedUserInformation.size());
		for (const auto& iterator : m_joinedUserInformation)
			usersSpawnInformation.push_back(FlatInGamePacket::Game::CreateUserSpawnInfoDirect(flatBuffer->m_flatBuffer, iterator.m_iUUID, iterator.m_sUserName.c_str(), iterator.m_iUniqueNumber));

		m_broadcastCallbackFunc(new FTransmitQueueData(peerInfo, CreateJoinInGamePacket(flatBuffer->m_flatBuffer, usersSpawnInformation), true));
		return true;
	}
	return false;
}

bool GAMESERVER::CInGameSystem::UpdatePlayerReadyState(const int32_t iUUID, const bool bIsReady, const FVector& vLocation, const FVector& vRotation, const FVector& vVelocity) {
	const auto& findResult = std::find_if(m_joinedUserInformation.begin(), m_joinedUserInformation.end(), [&](const FInGameUserInformation& val) {
		return val.m_iUUID == iUUID;
	});
	if (findResult != m_joinedUserInformation.cend()) {
		findResult->m_bIsReady = bIsReady;
		UpdatePlayerVehicleTransform(*findResult, vLocation, vRotation, vVelocity, 0.f);
		return true;
	}
	return false;
}

bool GAMESERVER::CInGameSystem::UpdatePlayerVehicleTransform(const int32_t iUUID, const FVector& vLocation, const FVector& vRotation, const FVector& vVelocity, const float fHorizontal) {
	const auto& findResult = std::find_if(m_joinedUserInformation.begin(), m_joinedUserInformation.end(), [&](const FInGameUserInformation& val) {
		return val.m_iUUID == iUUID;
	});
	if (findResult != m_joinedUserInformation.cend()) {
		UpdatePlayerVehicleTransform(*findResult, vLocation, vRotation, vVelocity, fHorizontal);
		return true;
	}
	return false;
}

void GAMESERVER::CInGameSystem::Broadcast(const SERVER::NETWORK::PACKET::PACKET_STRUCT& sendPacket, const bool bReliable) {
	for (const auto& iterator : m_joinedUserInformation)
		m_broadcastCallbackFunc(new FTransmitQueueData(iterator.m_userPeerInformation, sendPacket, bReliable));
}
