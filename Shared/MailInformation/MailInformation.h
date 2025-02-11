#pragma once
#include "../AttendanceInformation/AttendanceInformation.h"

struct FMailStructure {
public:
	int64_t m_iMailID;
	std::string m_sMailName;
	SQL_TIMESTAMP_STRUCT m_receivedDate;
	std::string m_sRemainTime;
	std::vector<FItemInformation> m_attachedRewards;
	bool m_bIsCheck;

public:
	FMailStructure() : m_iMailID(), m_sMailName(), m_receivedDate(), m_attachedRewards(), m_bIsCheck() {};

	FMailStructure(const std::string& sMailName, const SQL_TIMESTAMP_STRUCT& receivedDate) : m_iMailID(), m_sMailName(sMailName), m_receivedDate(receivedDate), m_bIsCheck() {}

	FMailStructure(const std::string& sMailName, const SQL_TIMESTAMP_STRUCT& receivedDate, const std::vector<FItemInformation>& attachedItem) : m_iMailID(), m_sMailName(sMailName), m_receivedDate(receivedDate), m_attachedRewards(attachedItem), m_bIsCheck() {}

	FMailStructure(const int64_t iMailID, const std::string& sMailName, const SQL_TIMESTAMP_STRUCT& receivedDate, const std::string& sRemainTime, const std::vector<FItemInformation>& attachedRewards, const bool bIsCheck) 
		: m_iMailID(iMailID), m_sMailName(sMailName), m_receivedDate(receivedDate), m_sRemainTime(sRemainTime), m_attachedRewards(attachedRewards), m_bIsCheck(bIsCheck) {};

public:
	__forceinline std::string AttchedRewardsToJsonString() const {
		Json::Value root;
		{
			for (auto& iterator : m_attachedRewards) {
				Json::Value itemsValue;
				iterator.WriteToJson(itemsValue);
				root.append(itemsValue);
			}
		}
		Json::FastWriter writer;
		return RemoveEscapeCharactersFromJsonString(writer.write(root));
	}

};