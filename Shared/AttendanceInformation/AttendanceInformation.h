#pragma once
#define NOMINMAX
#define _WINSOCKAPI_

#include <Windows.h>
#include <vector>
#include <json/json.h>
#include "../Util/Util.h"

struct FItemInformation {
public:
	int m_iItemID;
	int m_iItemCount;

public:
	FItemInformation() : m_iItemID(), m_iItemCount() {};
	FItemInformation(const int iItemID, const int iItemCount) : m_iItemID(iItemID), m_iItemCount(iItemCount) {};

public:
	static FItemInformation ReadFromJson(Json::Value& root) {
		FItemInformation newInfo;

		newInfo.m_iItemID = root["item_id"].asInt();
		newInfo.m_iItemCount = root["item_count"].asInt();

		return newInfo;
	}

	void WriteToJson(Json::Value& root) const {
		root["item_id"] = m_iItemID;
		root["item_count"] = m_iItemCount;
	}

	std::string ToJsonString() const {
		Json::Value root;
		WriteToJson(root);

		Json::FastWriter writer;
		return RemoveEscapeCharactersFromJsonString(writer.write(root));
	}

};

struct FAttendanceRewardInformation {
public:
	bool m_bIsSpecialReward;
	bool m_bSendToMail;
	std::vector<FItemInformation> m_rewards;

public:
	FAttendanceRewardInformation() : m_bIsSpecialReward(), m_bSendToMail(), m_rewards() {};
	FAttendanceRewardInformation(const FItemInformation& item) : m_bIsSpecialReward(), m_bSendToMail() {
		m_rewards.push_back(item);
	};

	static FAttendanceRewardInformation ReadFromJson(Json::Value& root) {
		FAttendanceRewardInformation newInfo;

		newInfo.m_bIsSpecialReward = root["Is_Special_Reward"].asBool();
		newInfo.m_bSendToMail = root["Send_To_Mail"].asBool();
		for (auto& iterator : root["listOfItems"])
			newInfo.m_rewards.push_back(FItemInformation::ReadFromJson(iterator));

		return newInfo;
	}

	void WriteToJson(Json::Value& root) const {
		for (auto& iterator : m_rewards) {
			Json::Value itemValue;
			iterator.WriteToJson(itemValue);

			root["listOfItems"].append(itemValue);
		}
		root["Is_Special_Reward"] = m_bIsSpecialReward;
		root["Send_To_Mail"] = m_bSendToMail;
	}

	std::string ToJsonString() const {
		Json::Value root;
		{
			Json::Value itemsValue;
			for (auto& iterator : m_rewards)
				iterator.WriteToJson(itemsValue);
			root.append(itemsValue);
		}

		Json::FastWriter writer;
		return RemoveEscapeCharactersFromJsonString(writer.write(root));
	}

};

struct FAttendanceInfo {
public:
	int16_t m_iEventType;
	int16_t m_iEventID;
	SYSTEMTIME m_startDate;
	SYSTEMTIME m_endDate;
	std::string m_sEventSheetName;
	std::vector<FAttendanceRewardInformation> m_DayOfTheWeekRewardInformation;

public:
	uint16_t m_iCountOfSpecialReward;
	uint16_t m_iStartDaySpecialReward;
	uint16_t m_iMaxAttendanceDay;

private:
	__forceinline std::string GetDateToString(const SYSTEMTIME& systemTime) {
		return std::string(std::to_string(systemTime.wYear) + "-" + std::to_string(systemTime.wMonth) + "-" + std::to_string(systemTime.wDay));
	}

	static __forceinline void ExtractSystemTimeFromString(const std::string& str, SYSTEMTIME& date) {
		auto iYearOffset = str.find('-');
		date.wYear = std::atoi(str.substr(0, iYearOffset).c_str());
		auto iMonthOffset = str.find('-', iYearOffset + 1);
		date.wMonth = std::atoi(str.substr(iYearOffset + 1, iMonthOffset).c_str());
		date.wDay = std::atoi(str.substr(iMonthOffset + 1).c_str());
	}

public:
	FAttendanceInfo() : m_iEventType(), m_iEventID(), m_sEventSheetName(), m_iCountOfSpecialReward(), m_iStartDaySpecialReward(), m_iMaxAttendanceDay() {
		ZeroMemory(&m_startDate, sizeof(SYSTEMTIME));
		ZeroMemory(&m_endDate, sizeof(SYSTEMTIME));
	};

	FAttendanceInfo(const int16_t iEventType, const int16_t iEventID, const SYSTEMTIME& startDate, const SYSTEMTIME& endDate, const std::string& sEventSheetName) :
		m_iEventType(iEventType), m_iEventID(iEventID), m_startDate(startDate), m_endDate(endDate), m_sEventSheetName(sEventSheetName), m_iCountOfSpecialReward(), m_iStartDaySpecialReward(), m_iMaxAttendanceDay() {
	}

public:
	static FAttendanceInfo ReadFromJson(Json::Value& root) {
		FAttendanceInfo newEvent;

		newEvent.m_iEventType = root["Event_Type"].asInt();
		newEvent.m_iEventID = root["Event_ID"].asInt();
		ExtractSystemTimeFromString(root["Event_Start_Date"].asCString(), newEvent.m_startDate);
		ExtractSystemTimeFromString(root["Event_End_Date"].asCString(), newEvent.m_endDate);
		newEvent.m_sEventSheetName = root["Sheet_Name"].asCString();

		for (auto& iterator : root["AttendanceDatas"]) {
			auto attendanceRewardInfo = FAttendanceRewardInformation::ReadFromJson(iterator);

			if (attendanceRewardInfo.m_bIsSpecialReward)
				newEvent.m_iCountOfSpecialReward++;

			newEvent.m_DayOfTheWeekRewardInformation.push_back(attendanceRewardInfo);
		}

		newEvent.m_iMaxAttendanceDay = newEvent.m_DayOfTheWeekRewardInformation.size();
		newEvent.m_iStartDaySpecialReward = newEvent.m_iMaxAttendanceDay - newEvent.m_iCountOfSpecialReward;

		return newEvent;
	}

	void WriteToJson(Json::Value& root) {
		root["Event_Type"] = m_iEventType;
		root["Event_ID"] = m_iEventID;
		root["Event_Start_Date"] = GetDateToString(m_startDate);
		root["Event_End_Date"] = GetDateToString(m_endDate);
		root["Sheet_Name"] = m_sEventSheetName;

		Json::Value attendanceRewards;
		for (auto& iterator : m_DayOfTheWeekRewardInformation) {
			Json::Value rewardValue;
			iterator.WriteToJson(rewardValue);

			attendanceRewards.append(rewardValue);
		}
		root["AttendanceDatas"] = attendanceRewards;
	}
};

struct FUserAttendanceInformation {
public:
	uint32_t m_iEventKey;
	SQL_DATE_STRUCT m_lastAttendanceDate;
	int16_t m_iAttendanceCount;

public:
	FUserAttendanceInformation() : m_iEventKey(), m_lastAttendanceDate(), m_iAttendanceCount() {};
	FUserAttendanceInformation(const uint32_t iEventKey, const SQL_DATE_STRUCT& lastAttendanceDate, const int16_t iAttendanceCount) : m_iEventKey(iEventKey), m_lastAttendanceDate(lastAttendanceDate), m_iAttendanceCount(iAttendanceCount) {};

};