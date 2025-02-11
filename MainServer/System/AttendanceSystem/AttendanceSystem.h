#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#include "../../../Shared/AttendanceInformation/AttendanceInformation.h"
#include "../../../Shared/MailInformation/MailInformation.h"
#include <concurrent_vector.h>

namespace MAINSERVER {
	class CAttendanceSystem {
		typedef concurrency::concurrent_vector<FAttendanceInfo> AttendanceEvents;
	private:
		AttendanceEvents m_attendanceEvents;

	private:
		bool GetAttendanceInformationByEventKey(const uint32_t iEventKey, FAttendanceInfo& outputInfo) const;

		short GetAttendanceCountByEventType(const uint16_t iEventType, const uint16_t iLastDayOfEvent, const FAttendanceInfo& attendanceInfo, const uint16_t iCurrentIndex) const;

		bool CheckDailyAttendanceDateOver(const SQL_DATE_STRUCT& currentDate, const SQL_DATE_STRUCT& lastAttendanceDate) const;

	public:
		CAttendanceSystem();

		void Initialize(const std::string& sAttendanceJsonFilePath);

	public:
		bool GetAttendanceDetailByEventType(const uint16_t iEventType, std::vector<FUserAttendanceInformation>& dbUserAttendanceInfo, flatbuffers::FlatBufferBuilder& builder, std::vector<flatbuffers::Offset<FlatPacket::AttendancePacket::AttendanceDetail>>& outputAttendanceInformation) const;

		bool AttendanceCheck(const uint32_t iEventKey, const uint8_t iAttendanceType, const SQL_DATE_STRUCT& lastAttendanceDate, const int16_t  iCurrentAttendanceCount, int16_t& iOutputUpdatedAttendanceCount, SQL_DATE_STRUCT& outputUpdatedAttendanceDate, std::string& sOutputErrorMessage, std::vector<FItemInformation>& outputRewards, std::vector<FMailStructure>& outputMailRewards) const;

	public:
		bool IsValidAttendanceEvent(const uint32_t iEventKey) const;
		bool IsValidAttendanceEvent(const FAttendanceInfo& attendanceInfo) const;
		bool IsValidAttendanceEvent(const SYSTEMTIME& startDate, const SYSTEMTIME& endDate) const;


	};
}