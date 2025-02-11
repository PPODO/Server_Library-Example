#include "AttendanceSystem.h"
#include <fstream>

using namespace MAINSERVER;

CAttendanceSystem::CAttendanceSystem() {
}

void CAttendanceSystem::Initialize(const std::string& sAttendanceJsonFilePath) {
	std::fstream attendanceJsonFileStream(sAttendanceJsonFilePath, std::ios::in);
	Json::Value attendanceJsonRoot;
	Json::Reader jsonReader;
	if (attendanceJsonFileStream.is_open() && jsonReader.parse(attendanceJsonFileStream, attendanceJsonRoot)) {
		for (auto& iterator : attendanceJsonRoot) {
			auto newAttendanceInfo = FAttendanceInfo::ReadFromJson(iterator);

			m_attendanceEvents.push_back(newAttendanceInfo);
		}
		attendanceJsonFileStream.close();
	}
}

bool CAttendanceSystem::GetAttendanceInformationByEventKey(const uint32_t iEventKey, FAttendanceInfo& outputInfo) const {
	const auto& eventIterator = std::find_if(m_attendanceEvents.begin(), m_attendanceEvents.end(), [&](const FAttendanceInfo& attendanceInfo) {
		return ::GetEventKey(attendanceInfo.m_iEventType, attendanceInfo.m_iEventID) == iEventKey; });
	if (eventIterator != m_attendanceEvents.cend()) {
		outputInfo = *eventIterator;
		return true;
	}
	return false;
}

short CAttendanceSystem::GetAttendanceCountByEventType(const uint16_t iEventType, const uint16_t iLastDayOfEvent, const FAttendanceInfo& attendanceInfo, const uint16_t iCurrentIndex) const {
	if (iEventType != EEventType::EEID_Daily) return iCurrentIndex;

	const short iLastDayOfNormalReward = iLastDayOfEvent - attendanceInfo.m_iCountOfSpecialReward;

	short iAttendanceCount = 0;
	if (iCurrentIndex <= iLastDayOfNormalReward) {
		iAttendanceCount = iCurrentIndex;
		if (iCurrentIndex > attendanceInfo.m_iStartDaySpecialReward)
			iAttendanceCount = iCurrentIndex % attendanceInfo.m_iStartDaySpecialReward;
	}
	else
		iAttendanceCount = (iCurrentIndex % iLastDayOfNormalReward) + attendanceInfo.m_iStartDaySpecialReward;

	return iAttendanceCount;
}

bool CAttendanceSystem::CheckDailyAttendanceDateOver(const SQL_DATE_STRUCT& currentDate, const SQL_DATE_STRUCT& lastAttendanceDate) const {
	if ((currentDate.year - lastAttendanceDate.year) > 0 || (currentDate.month - lastAttendanceDate.month) > 0)
		return true;
	return false;
}



bool CAttendanceSystem::IsValidAttendanceEvent(const uint32_t iEventKey) const {
	FAttendanceInfo attendanceInfo;
	if (!GetAttendanceInformationByEventKey(iEventKey, attendanceInfo)) return false;

	return IsValidAttendanceEvent(attendanceInfo.m_startDate, attendanceInfo.m_endDate);
}

bool MAINSERVER::CAttendanceSystem::IsValidAttendanceEvent(const FAttendanceInfo& attendanceInfo) const {
	return IsValidAttendanceEvent(attendanceInfo.m_startDate, attendanceInfo.m_endDate);
}

bool CAttendanceSystem::IsValidAttendanceEvent(const SYSTEMTIME& startDate, const SYSTEMTIME& endDate) const {
	const auto currentServerDate = GetCurrentDate();

	const boost::gregorian::date currentDate(currentServerDate.wYear, currentServerDate.wMonth, currentServerDate.wDay);
	const boost::gregorian::date boostStartDate(startDate.wYear, startDate.wMonth, startDate.wDay);
	const boost::gregorian::date boostEndDate(endDate.wYear, endDate.wMonth, endDate.wDay);

	return currentDate >= boostStartDate && currentDate <= boostEndDate;
}

bool CAttendanceSystem::GetAttendanceDetailByEventType(const uint16_t iEventType, std::vector<FUserAttendanceInformation>& dbUserAttendanceInfo, flatbuffers::FlatBufferBuilder& builder, std::vector<flatbuffers::Offset<FlatPacket::AttendancePacket::AttendanceDetail>>& outputAttendanceInformation) const {
	const auto currentServerDate = ::SystemTimeToSQLDateStruct(GetCurrentDate());

	for (const auto& iterator : m_attendanceEvents) {
		uint16_t iLastDayOfEvent = 0;
		uint16_t iCurrentAttendanceCount = 0;

		switch (iEventType) {
		case EEventType::EEID_Daily:
			iLastDayOfEvent = GetLastDayOfMonth(currentServerDate.year, currentServerDate.month);
			break;
		case EEventType::EEID_Special:
			iLastDayOfEvent = iterator.m_iMaxAttendanceDay;
			break;
		default:
			return false;
		}

		if (iterator.m_iEventType == iEventType) {
			const auto& userAttFindResult = std::find_if(dbUserAttendanceInfo.begin(), dbUserAttendanceInfo.end(), [&](const FUserAttendanceInformation& userAttInfo) {
				return userAttInfo.m_iEventKey == ::GetEventKey(iterator.m_iEventType, iterator.m_iEventID);
			});

			if (userAttFindResult != dbUserAttendanceInfo.cend()) {
				if (iEventType == EEventType::EEID_Daily &&
					CheckDailyAttendanceDateOver(currentServerDate, userAttFindResult->m_lastAttendanceDate))
						iCurrentAttendanceCount = userAttFindResult->m_iAttendanceCount = 0;
				else
					iCurrentAttendanceCount = userAttFindResult->m_iAttendanceCount;
			}

			std::vector<flatbuffers::Offset<FlatPacket::AttendancePacket::attendance_reward>> attendanceRewardInfo;
			attendanceRewardInfo.reserve(iLastDayOfEvent); 

			for (short i = 1; i <= iLastDayOfEvent; i++) {
				const auto& attendanceReward = iterator.m_DayOfTheWeekRewardInformation[GetAttendanceCountByEventType(iEventType, iLastDayOfEvent, iterator, i) - 1];

				std::vector<flatbuffers::Offset<FlatPacket::item>> items;
				items.reserve(attendanceReward.m_rewards.size());
				for (const auto& iterator : attendanceReward.m_rewards)
					items.push_back(FlatPacket::Createitem(builder, iterator.m_iItemID, iterator.m_iItemCount));

				attendanceRewardInfo.push_back(FlatPacket::AttendancePacket::Createattendance_reward(builder, i, builder.CreateVector(items)));
			}
			
			outputAttendanceInformation.push_back(FlatPacket::AttendancePacket::CreateAttendanceDetailDirect(builder, iterator.m_iEventType, iterator.m_iEventID, iterator.m_sEventSheetName.c_str(), iCurrentAttendanceCount, iLastDayOfEvent, &attendanceRewardInfo));
		}
	}
	return true;
}

bool CAttendanceSystem::AttendanceCheck(const uint32_t iEventKey, const uint8_t iAttendanceType, const SQL_DATE_STRUCT& lastAttendanceDate, const int16_t iCurrentAttendanceCount, int16_t& iOutputUpdatedAttendanceCount, SQL_DATE_STRUCT& outputUpdatedAttendanceDate, std::string& sOutputErrorMessage, std::vector<FItemInformation>& outputInventoryRewards, std::vector<FMailStructure>& outputMailRewards) const {
	using namespace FlatPacket::AttendancePacket;

	const auto currentServerDate = GetCurrentDate();
	const bool bIsSupplement = (iAttendanceType & AttendanceCheckMessageType::AttendanceCheckMessageType_NextDayAttendance) | (iAttendanceType & AttendanceCheckMessageType::AttendanceCheckMessageType_LeftAllDayAttendance);

	if (!bIsSupplement && (currentServerDate == lastAttendanceDate)) {
		sOutputErrorMessage = "You Already Attendance!\n";
		return false;
	}

	FAttendanceInfo attendanceInfo;
	if (!GetAttendanceInformationByEventKey(iEventKey, attendanceInfo)) return false;

	const uint16_t iEventType = GetEventTypeFromEventKey(iEventKey);
	const bool bIsAllSupplement = (iAttendanceType & AttendanceCheckMessageType::AttendanceCheckMessageType_LeftAllDayAttendance);
	const short iLastDayOfMonth = GetLastDayOfMonth(currentServerDate.wYear, currentServerDate.wMonth);
	const std::string sAttendanceRewardMailName = "Attendance Reward";
	const SQL_TIMESTAMP_STRUCT attendanceRewardMailReceivedDate = SystemTimeToSQLTimeStampStruct(GetCurrentDate());

	short iStartAttendanceCount = iCurrentAttendanceCount + 1;
	short iEndAttendanceCount = bIsAllSupplement ? iLastDayOfMonth : iStartAttendanceCount;

	if (((iStartAttendanceCount >= iLastDayOfMonth) && iEventType == EEventType::EEID_Daily) ||
		(iEndAttendanceCount >= attendanceInfo.m_iMaxAttendanceDay) && iEventType == EEventType::EEID_Special) {
		sOutputErrorMessage = "You Can't Attendance Check!\n";
		return false;
	}
	iOutputUpdatedAttendanceCount = iEndAttendanceCount;

	for (short i = iStartAttendanceCount; i <= iEndAttendanceCount; i++) {
		const auto& rewardInformation = attendanceInfo.m_DayOfTheWeekRewardInformation[GetAttendanceCountByEventType(iEventType, iLastDayOfMonth, attendanceInfo, i) - 1];

		if (rewardInformation.m_bSendToMail) {
			outputMailRewards.emplace_back(sAttendanceRewardMailName, attendanceRewardMailReceivedDate, rewardInformation.m_rewards);
			continue;
		}

		for (auto rewardIterator : rewardInformation.m_rewards) {
			auto findIterator = std::find_if(outputInventoryRewards.begin(), outputInventoryRewards.end(), [&](const FItemInformation& lhs) { return lhs.m_iItemID == rewardIterator.m_iItemID; });

			if (findIterator != outputInventoryRewards.cend()) {
				findIterator->m_iItemCount += rewardIterator.m_iItemCount;
				continue;
			}
			outputInventoryRewards.emplace_back(rewardIterator.m_iItemID, rewardIterator.m_iItemCount);
		}
	}

	outputUpdatedAttendanceDate.year = currentServerDate.wYear;
	outputUpdatedAttendanceDate.month = currentServerDate.wMonth;
	outputUpdatedAttendanceDate.day = currentServerDate.wDay;
	return true;
}