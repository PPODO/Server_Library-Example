#include "GameDBSystem.h"

#define SQL_VCHAR_DEFAULT_LENGTH 50
#define MAIL_REWARDS_JSON_LENGTH 512

using namespace MAINSERVER;

CGameDBSystem::CGameDBSystem(const std::string& sHostName, const std::string& sDBName, const std::string& sUserName, const std::string& sPassword, const uint16_t iMaxPoolConnection, const std::chrono::milliseconds& durationForDBThread, const std::function<void(struct FMessageData*)>& dbRequestProcessedCallback) 
	: CDBBaseSystem(sHostName, sDBName, sUserName, sPassword, iMaxPoolConnection, durationForDBThread, dbRequestProcessedCallback) {

	m_requestProcessor.emplace(FlatPacket::PacketType::PacketType_DB_User_Information, std::bind(&CGameDBSystem::GetUserInformation, this, std::placeholders::_1));

	m_requestProcessor.emplace(FlatPacket::PacketType::PacketType_DB_Attendance_Sheets, std::bind(&CGameDBSystem::GetUserAttendanceInformation, this, std::placeholders::_1));
	m_requestProcessor.emplace(FlatPacket::PacketType::PacketType_DB_Attendance_Check, std::bind(&CGameDBSystem::AttendanceCheckApply, this, std::placeholders::_1));

	m_requestProcessor.emplace(FlatPacket::PacketType::PacketType_DB_MailsInfo, std::bind(&CGameDBSystem::GetMailsByPage, this, std::placeholders::_1));
	m_requestProcessor.emplace(FlatPacket::PacketType::PacketType_DB_MailConfirm, std::bind(&CGameDBSystem::ConfirmMailByMailID, this, std::placeholders::_1));
	m_requestProcessor.emplace(FlatPacket::PacketType::PacketType_DB_MailReceive, std::bind(&CGameDBSystem::MailReceive, this, std::placeholders::_1));

	m_requestProcessor.emplace(FlatPacket::PacketType::PacketType_DB_CheckExpiredMails, std::bind(&CGameDBSystem::CheckExpiredMailsCallback, this, std::placeholders::_1));
}

std::shared_ptr<void> MAINSERVER::CGameDBSystem::GetUserInformation(void* const pRequestData) {
	using namespace SERVER::FUNCTIONS::UTIL;

	if (auto pUserInformationResult = static_cast<FDBUesrInformationRequest* const>(pRequestData)) {
		auto pSTMT = m_sqlPool.GetConnection()->AllocSTMT();

		SQLPrepare(*pSTMT, (SQLWCHAR*)L"{CALL get_user_information (?)}", SQL_NTS);

		SQLBindParameter(*pSTMT, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pUserInformationResult->m_iUUID, sizeof(int32_t), NULL);

		if (SQLExecute(*pSTMT) != SQL_SUCCESS)
			GetMSSQLErrorMessage(SQL_HANDLE_STMT, *pSTMT);
		else {
			if (GetUserUnconfirmedMails(*pSTMT, pUserInformationResult->m_iNumOfUnconfirmedMails) &&
				GetUserInventory(*pSTMT, pUserInformationResult->m_userInventoryInformation))
				pUserInformationResult->m_requestResult = FlatPacket::RequestMessageType::RequestMessageType_Succeeded;
		}
		return std::shared_ptr<void>(pUserInformationResult);
	}
	return nullptr;
}

std::shared_ptr<void> CGameDBSystem::GetUserAttendanceInformation(void* const pRequestData) {
	using namespace SERVER::FUNCTIONS::UTIL;

	if (auto pAttendanceSheetsRequest = static_cast<FDBAttendanceSheetsRequest* const>(pRequestData)) {
		auto pSTMT = m_sqlPool.GetConnection()->AllocSTMT();

		SQLPrepare(*pSTMT, (SQLWCHAR*)L"{CALL get_user_attendance_info (?)}", SQL_NTS);

		SQLBindParameter(*pSTMT, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pAttendanceSheetsRequest->m_iUUID, sizeof(int32_t), NULL);

		if (SQLExecute(*pSTMT) != SQL_SUCCESS)
			GetMSSQLErrorMessage(SQL_HANDLE_STMT, *pSTMT);
		else {
			while (SQLFetch(*pSTMT) == SQL_SUCCESS) {
				FUserAttendanceInformation userAttendanceInformation;

				SQLGetData(*pSTMT, 1, SQL_C_LONG, &userAttendanceInformation.m_iEventKey, sizeof(userAttendanceInformation.m_iEventKey), NULL);
				SQLGetData(*pSTMT, 2, SQL_C_SHORT, &userAttendanceInformation.m_iAttendanceCount, sizeof(userAttendanceInformation.m_iAttendanceCount), NULL);
				SQLGetData(*pSTMT, 3, SQL_C_DATE, &userAttendanceInformation.m_lastAttendanceDate, sizeof(userAttendanceInformation.m_lastAttendanceDate), NULL);

				pAttendanceSheetsRequest->m_userAttendanceInformations.push_back(userAttendanceInformation);
			}
			pAttendanceSheetsRequest->m_requestResult = FlatPacket::RequestMessageType_Succeeded;
		}
		return std::shared_ptr<void>(pAttendanceSheetsRequest);
	}
	return nullptr;
}

std::shared_ptr<void> CGameDBSystem::AttendanceCheckApply(void* const pRequestData) {
	using namespace SERVER::FUNCTIONS::UTIL;

	if (auto pAttendanceCheckApplyRequest = static_cast<FDBAttendanceApplyRequest* const>(pRequestData)) {
		auto pDBConnection = m_sqlPool.GetConnection();
		auto pSTMT = pDBConnection->AllocSTMT();
		auto hDBC = pDBConnection->GetHDBC();

		SQLSetConnectAttr(hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);

		if (UpdateUserAttendanceInformation(*pSTMT, hDBC, pAttendanceCheckApplyRequest->m_iUUID, pAttendanceCheckApplyRequest->m_iEventKey, pAttendanceCheckApplyRequest->m_updatedAttendanceDate, pAttendanceCheckApplyRequest->m_iUpdatedAttendanceCount) &&
			MergeItemsIntoInventory(*pSTMT, hDBC, pAttendanceCheckApplyRequest->m_iUUID, pAttendanceCheckApplyRequest->m_attendanceRewards) &&
			SendMailToUser(*pSTMT, pAttendanceCheckApplyRequest->m_iUUID, pAttendanceCheckApplyRequest->m_attendanceMailRewards)) {
			SQLEndTran(SQL_HANDLE_DBC, hDBC, SQL_COMMIT);
			pAttendanceCheckApplyRequest->m_requestResult = FlatPacket::RequestMessageType_Succeeded;
		}
		else
			SQLEndTran(SQL_HANDLE_DBC, hDBC, SQL_ROLLBACK);

		SQLSetConnectAttr(hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
		return std::shared_ptr<void>(pAttendanceCheckApplyRequest);
	}
	return nullptr;
}

std::shared_ptr<void> MAINSERVER::CGameDBSystem::GetMailsByPage(void* const pRequestData) {
	using namespace SERVER::FUNCTIONS::UTIL;

	if (auto pGetMailsByPage = static_cast<FDBGetMailsRequest* const>(pRequestData)) {
		auto pSTMT = m_sqlPool.GetConnection()->AllocSTMT();

		SQLPrepare(*pSTMT, (SQLWCHAR*)L"{call get_mails_by_page (?, ?, ?, ?)}", SQL_NTS);

		SQLBindParameter(*pSTMT, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pGetMailsByPage->m_iUUID, sizeof(SQLINTEGER), NULL);
		SQLBindParameter(*pSTMT, 2, SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, 0, 0, &pGetMailsByPage->m_iPage, sizeof(SQLSMALLINT), NULL);
		SQLBindParameter(*pSTMT, 3, SQL_PARAM_OUTPUT, SQL_C_SHORT, SQL_SMALLINT, 0, 0, &pGetMailsByPage->m_iRowCountLimit, sizeof(SQLSMALLINT), NULL);
		SQLBindParameter(*pSTMT, 4, SQL_PARAM_OUTPUT, SQL_C_SHORT, SQL_SMALLINT, 0, 0, &pGetMailsByPage->m_iTotalMailCount, sizeof(SQLSMALLINT), NULL);

		if (SQLExecute(*pSTMT) != SQL_SUCCESS)
			GetMSSQLErrorMessage(SQL_HANDLE_STMT, *pSTMT);
		else {
			while (SQLFetch(*pSTMT) == SQL_SUCCESS) {
				SQLBIGINT iMailID = 0;
				SQLCHAR sMailName[SQL_VCHAR_DEFAULT_LENGTH] = { "\0" };
				SQL_TIMESTAMP_STRUCT receivedDate = {};
				bool bIsChecked = false;
				SQLCHAR sRewardsJson[MAIL_REWARDS_JSON_LENGTH] = { "\0" };

				SQLGetData(*pSTMT, 1, SQL_C_SBIGINT, &iMailID, sizeof(iMailID), NULL);
				SQLGetData(*pSTMT, 2, SQL_C_CHAR, sMailName, SQL_VCHAR_DEFAULT_LENGTH, NULL);
				SQLGetData(*pSTMT, 3, SQL_C_TIMESTAMP, &receivedDate, sizeof(receivedDate), NULL);
				SQLGetData(*pSTMT, 4, SQL_C_BIT, &bIsChecked, sizeof(bIsChecked), NULL);
				SQLGetData(*pSTMT, 5, SQL_C_CHAR, sRewardsJson, MAIL_REWARDS_JSON_LENGTH, NULL);

				Json::Value itemsValue;
				Json::Reader jsonReader;
				if (jsonReader.parse((char*)sRewardsJson, itemsValue)) {
					std::vector<FItemInformation> attachedItems;
					attachedItems.reserve(itemsValue.size());

					for (auto& iterator : itemsValue) {
						const auto& item = FItemInformation::ReadFromJson(iterator);
						attachedItems.emplace_back(item.m_iItemID, item.m_iItemCount);
					}
					pGetMailsByPage->m_mailInformations.emplace_back(iMailID, (char*)sMailName, receivedDate, GetTimeLeftUntilExpiration(receivedDate, 7), attachedItems, bIsChecked);
				}
			}
			pGetMailsByPage->m_requestResult = FlatPacket::RequestMessageType::RequestMessageType_Succeeded;
		}
		return std::shared_ptr<void>(pGetMailsByPage);
	}
	return nullptr;
}

std::shared_ptr<void> MAINSERVER::CGameDBSystem::ConfirmMailByMailID(void* const pRequestData) {
	using namespace SERVER::FUNCTIONS::UTIL;
	using namespace SERVER::FUNCTIONS::LOG;

	if (auto pConfirmMail = static_cast<FDBConfirmMailRequest* const>(pRequestData)) {
		auto pDBConnection = m_sqlPool.GetConnection();
		auto pSTMT = pDBConnection->AllocSTMT();
		auto hDBC = pDBConnection->GetHDBC();

		SQLPrepare(*pSTMT, (SQLWCHAR*)L"{call confirm_mail (?)}", SQL_NTS);
		SQLBindParameter(*pSTMT, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_BIGINT, 0, 0, const_cast<SQLBIGINT*>(&pConfirmMail->m_iMailID), sizeof(SQLBIGINT), NULL);

		SQLSetConnectAttr(hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);

		if (SQLExecute(*pSTMT) != SQL_SUCCESS) {
			GetMSSQLErrorMessage(SQL_HANDLE_STMT, *pSTMT);

			SQLEndTran(SQL_HANDLE_DBC, hDBC, SQL_ROLLBACK);
			SQLSetConnectAttr(hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);

			Log::WriteLog(L"Client [%d] Mail Verification Failed!", pConfirmMail->m_iUUID);
		}
		else {
			pConfirmMail->m_requestResult = FlatPacket::RequestMessageType::RequestMessageType_Succeeded;

			SQLEndTran(SQL_HANDLE_DBC, hDBC, SQL_COMMIT);
			Log::WriteLog(L"Client [%d] Mail Verification Succeeded!", pConfirmMail->m_iUUID);
		}
		SQLSetConnectAttr(hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);

		return std::shared_ptr<void>(pConfirmMail);
	}
	return nullptr;
}

std::shared_ptr<void> CGameDBSystem::MailReceive(void* const pRequestData) {
	using namespace SERVER::FUNCTIONS::UTIL;
	using namespace SERVER::FUNCTIONS::LOG;

	if (auto pReceiveMailsRequest = static_cast<FDBReceiveMailsRequest* const>(pRequestData)) {
		auto pDBConnection = m_sqlPool.GetConnection();
		auto pSTMT = pDBConnection->AllocSTMT();
		auto hDBC = pDBConnection->GetHDBC();

		SQLPrepare(*pSTMT, (SQLWCHAR*)L"{call get_receive_mail_rewards (?, ?, ?)}", SQL_NTS);

		SQLBindParameter(*pSTMT, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pReceiveMailsRequest->m_iUUID, sizeof(SQLINTEGER), NULL);
		SQLBindParameter(*pSTMT, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_BIGINT, 0, 0, &pReceiveMailsRequest->m_iMailID, sizeof(SQLBIGINT), NULL);
		SQLBindParameter(*pSTMT, 3, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT, 0, 0, &pReceiveMailsRequest->m_bReceiveAll, sizeof(bool), NULL);

		SQLSetConnectAttr(hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);
		if (SQLExecute(*pSTMT) != SQL_SUCCESS) {
			GetMSSQLErrorMessage(SQL_HANDLE_STMT, *pSTMT);
			Log::WriteLog(L"Client [%d] Receive Mails Failed!", pReceiveMailsRequest->m_iUUID);

			SQLEndTran(SQL_HANDLE_DBC, hDBC, SQL_ROLLBACK);
			SQLSetConnectAttr(hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
		}
		else {
			while (SQLFetch(*pSTMT) == SQL_SUCCESS) {
				SQLINTEGER iItemID = 0;
				SQLBIGINT iItemCount = 0;

				SQLGetData(*pSTMT, 1, SQL_C_LONG, &iItemID, sizeof(iItemID), NULL);
				SQLGetData(*pSTMT, 2, SQL_C_SBIGINT, &iItemCount, sizeof(iItemCount), NULL);

				pReceiveMailsRequest->m_receivedItems.emplace_back(iItemID, iItemCount);
			}
			SQLFreeStmt(*pSTMT, SQL_CLOSE);

			if (pReceiveMailsRequest->m_receivedItems.size() <= 0) {
				Log::WriteLog(L"Client [%d] No Mail Received! Request Failed!", pReceiveMailsRequest->m_iUUID);
			}
			else if(MergeItemsIntoInventory(*pSTMT, hDBC, pReceiveMailsRequest->m_iUUID, pReceiveMailsRequest->m_receivedItems)) {
				pReceiveMailsRequest->m_requestResult = FlatPacket::RequestMessageType_Succeeded;

				Log::WriteLog(L"Client [%d] Mail Received Successfully!", pReceiveMailsRequest->m_iUUID);
			}
			SQLEndTran(SQL_HANDLE_DBC, hDBC, SQL_COMMIT);
		}
		SQLSetConnectAttr(hDBC, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);

		return std::shared_ptr<void>(pReceiveMailsRequest);
	}
	return nullptr;
}

std::shared_ptr<void> CGameDBSystem::CheckExpiredMailsCallback(void* const pRequestData) {
	using namespace SERVER::FUNCTIONS::UTIL;
	using namespace SERVER::FUNCTIONS::LOG;

	if (auto pCheckExpiredMails = static_cast<FDBCheckExpiredMailsRequest* const>(pRequestData)) {
		if (auto pConnection = m_sqlPool.GetConnection()) {
			const size_t iDateStructLength = sizeof(SQL_TIMESTAMP_STRUCT) * 2;
			const auto currentTimestamp = SystemTimeToSQLTimeStampStruct(GetCurrentDate());
			auto pSTMT = pConnection->AllocSTMT();

			SQLPrepare(*pSTMT, (SQLWCHAR*)L"{call check_expired_mails (?)}", SQL_NTS);

			SQLBindParameter(*pSTMT, 1, SQL_PARAM_INPUT, SQL_C_TYPE_TIMESTAMP, SQL_VARCHAR, iDateStructLength, 0, const_cast<SQL_TIMESTAMP_STRUCT*>(&currentTimestamp), iDateStructLength, NULL);

			auto ret = SQLExecute(*pSTMT);
			if (ret != SQL_NO_DATA) {
				if (ret != SQL_SUCCESS) {
					GetMSSQLErrorMessage(SQL_HANDLE_STMT, *pSTMT);
					Log::WriteLog(L"Server : Failed To Delete Expired Mail!");
				}
				else if (SQLFetch(*pSTMT) == SQL_SUCCESS) {
					SQLINTEGER iTotalDeletedCount = 0;
					SQLGetData(*pSTMT, 1, SQL_C_LONG, &iTotalDeletedCount, sizeof(iTotalDeletedCount), NULL);
					Log::WriteLog(L"Server : Successfully Deleted Expired Mail! Count : [%d]", iTotalDeletedCount);
				}
			}
		}
		delete pCheckExpiredMails;
	}
	return nullptr;
}


bool CGameDBSystem::UpdateUserAttendanceInformation(SQLHSTMT& hSTMT, SQLHDBC& hDBC, const SQLINTEGER iUUID, const uint32_t iEventKey, const SQL_DATE_STRUCT& updatedAttendanceDate, const SQLSMALLINT& iUpdatedAttendanceCount) {
	using namespace SERVER::FUNCTIONS::UTIL;
	using namespace SERVER::FUNCTIONS::LOG;
	const size_t iDateStructLength = sizeof(SQL_DATE_STRUCT) * 2;

	SQLPrepare(hSTMT, (SQLWCHAR*)L"{call update_attendance_information (?, ?, ?, ?)}", SQL_NTS);

	SQLBindParameter(hSTMT, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, const_cast<SQLINTEGER*>(&iUUID), sizeof(SQLINTEGER), NULL);
	SQLBindParameter(hSTMT, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_BIGINT, 0, 0, const_cast<uint32_t*>(&iEventKey), sizeof(SQLBIGINT), NULL);
	SQLBindParameter(hSTMT, 3, SQL_PARAM_INPUT, SQL_C_TYPE_DATE, SQL_VARCHAR, iDateStructLength, 0, const_cast<SQL_DATE_STRUCT*>(&updatedAttendanceDate), iDateStructLength, NULL);
	SQLBindParameter(hSTMT, 4, SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, 0, 0, const_cast<SQLSMALLINT*>(&iUpdatedAttendanceCount), sizeof(SQLSMALLINT), NULL);

	if (SQLExecute(hSTMT) != SQL_SUCCESS) {
		GetMSSQLErrorMessage(SQL_HANDLE_STMT, hSTMT);
		SQLEndTran(SQL_HANDLE_DBC, hDBC, SQL_ROLLBACK);
		Log::WriteLog(L"Client [%d] Attendance Information Update Failed!", iUUID);

		return false;
	}
	SQLFreeStmt(hSTMT, SQL_CLOSE);
	Log::WriteLog(L"Client [%d] Attendance Information Update Successful!", iUUID);

	return true;
}

bool CGameDBSystem::MergeItemsIntoInventory(SQLHSTMT& hSTMT, SQLHDBC& hDBC, const SQLINTEGER iUUID, const std::vector<FItemInformation>& attendanceRewardList) {
	using namespace SERVER::FUNCTIONS::UTIL;
	using namespace SERVER::FUNCTIONS::LOG;

	for (auto iterator : attendanceRewardList) {
		const SQLINTEGER iItemID = iterator.m_iItemID;
		const SQLBIGINT iItemCount = iterator.m_iItemCount;

		SQLPrepare(hSTMT, (SQLWCHAR*)L"{call merge_items_into_inventory (?, ?, ?)}", SQL_NTS);

		SQLBindParameter(hSTMT, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, const_cast<SQLINTEGER*>(&iUUID), sizeof(SQLINTEGER), NULL);
		SQLBindParameter(hSTMT, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, const_cast<SQLINTEGER*>(&iItemID), sizeof(SQLINTEGER), NULL);
		SQLBindParameter(hSTMT, 3, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, const_cast<SQLBIGINT*>(&iItemCount), sizeof(SQLINTEGER), NULL);

		if (SQLExecute(hSTMT) != SQL_SUCCESS) {
			GetMSSQLErrorMessage(SQL_HANDLE_STMT, hSTMT);
			SQLEndTran(SQL_HANDLE_DBC, hDBC, SQL_ROLLBACK);
			Log::WriteLog(L"Client [%d] Attendance Rewards Will Not Be Merged Into The Inventory!", iUUID);

			return false;
		}
		SQLFreeStmt(hSTMT, SQL_CLOSE);
	}
	Log::WriteLog(L"Client [%d] Attendance Rewards Have Been Merged Into The Inventory!", iUUID);
	return true;
}

bool CGameDBSystem::GetUserUnconfirmedMails(SQLHSTMT& hSTMT, int32_t& iNumOfUnconfirmedMails) {
	if (SQLFetch(hSTMT) == SQL_SUCCESS) {
		SQLGetData(hSTMT, 1, SQL_C_LONG, &iNumOfUnconfirmedMails, sizeof(iNumOfUnconfirmedMails), NULL);
		return true;
	}
	return false;
}

bool CGameDBSystem::GetUserInventory(SQLHSTMT& hSTMT, std::vector<FItemInformation>& inventoryList) {
	if (SQLMoreResults(hSTMT) == SQL_SUCCESS) {
		while (SQLFetch(hSTMT) == SQL_SUCCESS) {
			SQLINTEGER iItemID = 0;
			SQLBIGINT iItemCount = 0;

			SQLGetData(hSTMT, 1, SQL_C_LONG, &iItemID, sizeof(iItemID), NULL);
			SQLGetData(hSTMT, 2, SQL_C_SBIGINT, &iItemCount, sizeof(iItemCount), NULL);

			inventoryList.emplace_back(iItemID, iItemCount);
		}
		return true;
	}
	return false;
}

bool CGameDBSystem::SendMailToUser(SQLHSTMT& hSTMT, const SQLINTEGER iUUID, const FMailStructure& mailInfo) {
	using namespace SERVER::FUNCTIONS::UTIL;
	using namespace SERVER::FUNCTIONS::LOG;

	const size_t iDateStructLength = sizeof(SQL_TIMESTAMP_STRUCT) * 2;

	const auto& sJsonString = mailInfo.AttchedRewardsToJsonString();

	SQLPrepare(hSTMT, (SQLWCHAR*)L"{call send_mail_to_user (?, ?, ?, ?)}", SQL_NTS);

	SQLBindParameter(hSTMT, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, const_cast<SQLINTEGER*>(&iUUID), sizeof(SQLSMALLINT), NULL);
	SQLBindParameter(hSTMT, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, mailInfo.m_sMailName.length(), 0, const_cast<char*>(mailInfo.m_sMailName.c_str()), 0, NULL);
	SQLBindParameter(hSTMT, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, sJsonString.length(), 0, const_cast<char*>(sJsonString.c_str()), 0, NULL);
	SQLBindParameter(hSTMT, 4, SQL_PARAM_INPUT, SQL_C_TYPE_TIMESTAMP, SQL_VARCHAR, iDateStructLength, 0, const_cast<SQL_TIMESTAMP_STRUCT*>(&mailInfo.m_receivedDate), iDateStructLength, NULL);

	if (SQLExecute(hSTMT) != SQL_SUCCESS) {
		GetMSSQLErrorMessage(SQL_HANDLE_STMT, hSTMT);
		Log::WriteLog(L"Client [%d] Attendance Rewards Will Be Not Be Mailed!", iUUID);

		return false;
	}
	SQLFreeStmt(hSTMT, SQL_CLOSE);
	Log::WriteLog(L"Client [%d] Attendance Rewards Have Been Mailed!", iUUID);

	return true;
}

bool CGameDBSystem::SendMailToUser(SQLHSTMT& hSTMT, const SQLINTEGER iUUID, const std::vector<FMailStructure>& mailInfos) {
	for (const auto& it : mailInfos) {
		if (!SendMailToUser(hSTMT, iUUID, it))
			return false;
	}
	return true;
}
