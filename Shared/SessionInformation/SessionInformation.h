#pragma once
#include <vector>
#include <string>
#include "../Packet/session_data_define_generated.h"
#include "../Packet/session_info_generated.h"

struct FSessionUserInformation {
public:
	void* m_pIOCPConnection;

	int32_t m_iUUID;
	std::string m_sUserName;

	bool m_bIsHost;
	bool m_bReadyState;

public:
	FSessionUserInformation() : m_pIOCPConnection(nullptr), m_iUUID(), m_sUserName(), m_bReadyState(), m_bIsHost(false) {};
	FSessionUserInformation(void* const pIOCPConnection, const int32_t iUUID, const std::string& sUserName, const bool bIsHost) : m_pIOCPConnection(pIOCPConnection), m_iUUID(iUUID), m_sUserName(sUserName), m_bIsHost(bIsHost), m_bReadyState() {};

};

struct FSessionStructure {
public:
	int32_t m_iSessionID;
	std::string m_sSessionName;
	std::vector<FSessionUserInformation> m_joinedUserInformation;
	int32_t m_iMaxSessionUsersCount;

public:
	FSessionStructure() : m_iSessionID(), m_sSessionName(), m_iMaxSessionUsersCount() {};
	FSessionStructure(const int32_t iSessionID, const std::string& sSessionName, const int32_t iMaxSessionUsersCount) : m_iSessionID(iSessionID), m_sSessionName(sSessionName), m_iMaxSessionUsersCount(iMaxSessionUsersCount) {};

public:
	flatbuffers::Offset<FlatPacket::SessionPacket::session_information> CreateSessionInformationOffsetData(flatbuffers::FlatBufferBuilder& builder) {
		std::vector<flatbuffers::Offset<FlatPacket::SessionPacket::session_user_info>> cachedUserInfo;
		cachedUserInfo.reserve(this->m_joinedUserInformation.size());
		for (const auto& iterator : this->m_joinedUserInformation)
			cachedUserInfo.push_back(FlatPacket::SessionPacket::Createsession_user_info(builder, iterator.m_iUUID, builder.CreateString(iterator.m_sUserName), iterator.m_bIsHost, iterator.m_bReadyState));

		return FlatPacket::SessionPacket::Createsession_information(builder, this->m_iSessionID, builder.CreateString(this->m_sSessionName), cachedUserInfo.size(), m_iMaxSessionUsersCount, builder.CreateVector(cachedUserInfo));
	}

};