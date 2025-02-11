#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#pragma comment(lib, "Rpcrt4.lib")
#include <rpc.h>
#include "../../../Shared/SessionInformation/SessionInformation.h"
#include <Network/Packet/BasePacket.hpp>

namespace MAINSERVER {
	struct FTransmitQueueData;

	class CSessionSystem {
		static const size_t NUM_OF_ONE_PAGE_SESSION_LISTINGS = 5;
	private:
		std::vector<FSessionStructure> m_sessionInformation;

		std::string m_sGameServerExeFilePath;

	private:
		uint16_t GenerateSessionID();

	public:
		bool Initialize(const std::string& sGameServerExeFilePath);

	public:
		bool CreateNewSession(const std::string& sSessionName, const FSessionUserInformation& sessionUserInfo, const int32_t iMaxUserCount, FSessionStructure& outputSessionInformation);
		bool JoinSession(int32_t iSessionID, const FSessionUserInformation& sessionUserInfo, FSessionStructure& outputSessionInformation);
		void FindSession(const int32_t iRequestedPage, int32_t& iMaxPages, std::vector<FSessionStructure>& outputSessionLists);

		bool QuitSession(const int32_t iSessionID, const int32_t iUUID);

		bool ChangeReadyState(const int32_t iSessionID, const int32_t iUUID, const bool bNewState);
		bool CanStartGame(const int32_t iSessionID, const int32_t iUUID);

		void BroadcastToSession(const int32_t iSessionID, const SERVER::NETWORK::PACKET::PACKET_STRUCT& sendPacket, const std::function<void(FTransmitQueueData*)>& callbackFunc);

		bool CreateGameServer(int16_t& iOutputServerPortNumber);

	public:
		void GetUserConnectionsBySessionID(const int32_t iSessionID, std::vector<void*>& joinedUserIOCPConnections);

	};
}