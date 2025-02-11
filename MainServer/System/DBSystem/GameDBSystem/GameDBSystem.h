#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#include "../DBBaseSystem/DBBaseSystem.h"

namespace MAINSERVER {
	class CGameDBSystem : public CDBBaseSystem {
	public:
		CGameDBSystem(const std::string& sHostName, const std::string& sDBName, const std::string& sUserName, const std::string& sPassword, const uint16_t iMaxPoolConnection, const std::chrono::milliseconds& durationForDBThread, const std::function<void(struct FMessageData*)>& dbRequestProcessedCallback);

	private:
		std::shared_ptr<void> GetUserInformation(void* const pRequestData);

		std::shared_ptr<void> GetUserAttendanceInformation(void* const pRequestData);
		std::shared_ptr<void> AttendanceCheckApply(void* const pRequestData);

		std::shared_ptr<void> GetMailsByPage(void* const pRequestData);
		std::shared_ptr<void> ConfirmMailByMailID(void* const pRequestData);
		std::shared_ptr<void> MailReceive(void* const pRequestData);

		std::shared_ptr<void> CheckExpiredMailsCallback(void* const pRequestData);

	private:
		bool UpdateUserAttendanceInformation(SQLHSTMT& hSTMT, SQLHDBC& hDBC, const SQLINTEGER iUUID, const uint32_t iEventKey, const SQL_DATE_STRUCT& updatedAttendanceDate, const SQLSMALLINT& iUpdatedAttendanceCount);
		bool MergeItemsIntoInventory(SQLHSTMT& hSTMT, SQLHDBC& hDBC, const SQLINTEGER iUUID, const std::vector<FItemInformation>& attendanceRewardList);

		bool GetUserUnconfirmedMails(SQLHSTMT& hSTMT, int32_t& iNumOfUnconfirmedMails);
		bool GetUserInventory(SQLHSTMT& hSTMT, std::vector<FItemInformation>& inventoryList);

		bool SendMailToUser(SQLHSTMT& hSTMT, const SQLINTEGER iUUID, const FMailStructure& mailInfo);
		bool SendMailToUser(SQLHSTMT& hSTMT, const SQLINTEGER iUUID, const std::vector<FMailStructure>& mailInfos);

	};
}