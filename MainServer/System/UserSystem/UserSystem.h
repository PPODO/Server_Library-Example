#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#include <unordered_map>
#include <NetworkModel/IOCP/IOCP.hpp>
#include "../AttendanceSystem/AttendanceSystem.h"
#include "../DBSystem/DBUtil/DBUtil.h"

namespace MAINSERVER {
	struct FUserInformation {
		typedef std::vector<FUserAttendanceInformation> ATTENDANCE_INFO_TYPE;
	public:
		SERVER::NETWORKMODEL::IOCP::CONNECTION* m_pConnection;

		std::string m_sUserName;

		ATTENDANCE_INFO_TYPE m_attendanceInformation;
		std::vector<FItemInformation> m_inventoryInformation;

		int32_t iSessionIDParticipated;

	public:
		FUserInformation() : m_pConnection(), m_attendanceInformation(), m_inventoryInformation(), m_sUserName(), iSessionIDParticipated() {};
		FUserInformation(SERVER::NETWORKMODEL::IOCP::CONNECTION* const pConnection, const std::string& sUserName) : m_pConnection(pConnection), m_attendanceInformation(), m_inventoryInformation(), m_sUserName(sUserName), iSessionIDParticipated() {};

	public:
		void UpdateAttendanceInformation(const std::vector<FUserAttendanceInformation>& attendanceInfo) {
			for (const auto& iterator : attendanceInfo) {
				const auto& findResult = std::find_if(m_attendanceInformation.begin(), m_attendanceInformation.end(), [&](const FUserAttendanceInformation& value) {
					return value.m_iEventKey == iterator.m_iEventKey;
				});

				if (findResult != m_attendanceInformation.cend())
					*findResult = iterator;
				else
					m_attendanceInformation.emplace_back(iterator);
			}
		}

	};

	class CUserSystem {
	private:
		std::unordered_map<SERVER::NETWORKMODEL::IOCP::CONNECTION*, int32_t> m_connectionInformation;
		std::unordered_map<int32_t, FUserInformation> m_loggedInUserInformation;

	private:
		void AddItemToInventory(FUserInformation& userInfo, const std::vector<FItemInformation>& itemInformation);

	public:
		CUserSystem();

	public:
		bool AddNewUser(SERVER::NETWORKMODEL::IOCP::CONNECTION* const pConnection, const int32_t iUUID, const FUserInformation& userInfo);
		bool DeleteUser(const int32_t iUUID);

		void UpdateUserAttendanceInformation(const int32_t iUUID, const std::vector<FUserAttendanceInformation>& attendanceInfo);
		void UpdateUserInventoryInformation(const int32_t iUUID, const std::vector<FItemInformation>& itemInformation);

		bool ApplyAttendanceCheckResult(const int32_t iUUID, const uint32_t iEventKey, const SQL_DATE_STRUCT& updatedAttendanceDate, const int16_t iUpdatedAttendanceCount, const std::vector<FItemInformation>& itemInformation);
		bool AddItemToInventory(const int32_t iUUID, const std::vector<FItemInformation>& itemInformation);

		bool CalculateCostRequest(const int32_t iUUID, const int32_t iItemID, const int32_t iItemCount);

		bool SetParticipatingSessionID(const int32_t iUUID, const int32_t iSessionID);

	public:
		bool GetUserAttendanceInformationByUUIDAndEventKey(const int32_t iUUID, const uint32_t iEventKey, FUserAttendanceInformation& outputResult);

		bool GetUserInformationByUUID(const int32_t iUUID, FUserInformation& outputUserInfo) const;

		int32_t GetUUIDByConnection(SERVER::NETWORKMODEL::IOCP::CONNECTION* const pConnection);
		int32_t GetParticipatingSessionIDByUUID(const int32_t iUUID) const;

	};
}