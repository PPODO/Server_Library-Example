#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#include <vector>
#include <string>
#include <functional>
#include <Network/NetworkProtocol/UDPSocket.hpp>
#include <chrono>
#include "../../../Shared/Util/Util.h"

namespace GAMESERVER {
	struct FTransmitQueueData;

	struct FFlatBuffer : SERVER::FUNCTIONS::MEMORYMANAGER::CMemoryManager<FFlatBuffer> {
	public:
		flatbuffers::FlatBufferBuilder m_flatBuffer;

	public:
		FFlatBuffer() : m_flatBuffer(SERVER::NETWORK::PACKET::PACKET_STRUCT::BUFFER_LENGTH) {};

	};

	struct FVector {
	public:
		float m_fX;
		float m_fY;
		float m_fZ;

	public:
		FVector(const float fX = 0.f, const float fY = 0.f, const float fZ = 0.f) : m_fX(fX), m_fY(fY), m_fZ(fZ) {};

	};

	struct FInGameUserInformation {
	public:
		const int32_t m_iUUID;
		const std::string m_sUserName;
		const int16_t m_iUniqueNumber;
		const SERVER::NETWORK::PROTOCOL::UDP::PeerInfo m_userPeerInformation;

		bool m_bIsReady;

		FVector m_vLastLocation;
		FVector m_vLastRotation;
		FVector m_vLastVelocity;

		float m_fHorizontal;

	public:
		FInGameUserInformation(const int32_t iUUID, const std::string& sUserName, const int16_t iUniqueNumber, const SERVER::NETWORK::PROTOCOL::UDP::PeerInfo& peerInfo) : m_iUUID(iUUID), m_sUserName(sUserName), m_iUniqueNumber(iUniqueNumber), m_userPeerInformation(peerInfo), m_bIsReady(false), m_fHorizontal() {};

	};

	class CInGameSystem {
	private:
		const std::function<void(FTransmitQueueData*)> m_broadcastCallbackFunc;
		const std::chrono::milliseconds m_updateDuration;
	private:
		std::vector<FInGameUserInformation> m_joinedUserInformation;

		std::chrono::system_clock::time_point m_lastUpdateTime;

	private:
		flatbuffers::Offset<FlatInGamePacket::Game::vector> CreateVector(flatbuffers::FlatBufferBuilder& builder, const FVector& vector);
		flatbuffers::Offset<FlatInGamePacket::Game::VehicleInformation> CreateVehicleInformation(flatbuffers::FlatBufferBuilder& builder, const FVector& location, const FVector& rotation, const FVector& velocity, const float fHorizontal);
		bool CreateGameDataPacket(flatbuffers::FlatBufferBuilder& builder, std::vector<flatbuffers::Offset<FlatInGamePacket::Game::GameData>>& outputFlatPacket);

		void UpdatePlayerVehicleTransform(FInGameUserInformation& inGameUserInformation, const FVector& vLocation, const FVector& vRotation, const FVector& vVelocity, const float fHorizontal);

	public:
		CInGameSystem(const std::function<void(FTransmitQueueData*)>& callbackFunc, const std::chrono::milliseconds& updateDuration);

	public:
		void Update();

	public:
		bool JoinUser(const int32_t iUUID, const std::string& sUserName, const SERVER::NETWORK::PROTOCOL::UDP::PeerInfo& peerInfo, int16_t& iOutputUserNumber);
		
		bool UpdatePlayerReadyState(const int32_t iUUID, const bool bIsReady, const FVector& vLocation, const FVector& vRotation, const FVector& vVelocity);
		bool UpdatePlayerVehicleTransform(const int32_t iUUID, const FVector& vLocation, const FVector& vRotation, const FVector& vVelocity, const float fHorizontal);

	public:
		void Broadcast(const SERVER::NETWORK::PACKET::PACKET_STRUCT& sendPacket, const bool bReliable);

	};
}