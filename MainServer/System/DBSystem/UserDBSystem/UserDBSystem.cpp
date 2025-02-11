#include "UserDBSystem.h"
#include "../../../MessageProcessor/MessageProcessor.h"

#define SQL_VCHAR_DEFAULT_LENGTH 50
#define SQL_PWD_NVARCHAR_LENGTH 100

using namespace MAINSERVER;

CUserDBSystem::CUserDBSystem(const std::string& sHostName, const std::string& sDBName, const std::string& sUserName, const std::string& sPassword, const uint16_t iMaxPoolConnection, const std::chrono::milliseconds& durationForDBThread, const std::function<void(FMessageData*)>& dbRequestProcessedCallback)
	: CDBBaseSystem(sHostName, sDBName, sUserName, sPassword, iMaxPoolConnection, durationForDBThread, dbRequestProcessedCallback) {

	m_requestProcessor.emplace(FlatPacket::PacketType::PacketType_DB_Sign_In, std::bind(&CUserDBSystem::SignInRequest, this, std::placeholders::_1));
	m_requestProcessor.emplace(FlatPacket::PacketType::PacketType_DB_Sign_Up, std::bind(&CUserDBSystem::SignUpRequest, this, std::placeholders::_1));
}

std::shared_ptr<void> CUserDBSystem::SignInRequest(void* const pRequestData) {
	using namespace SERVER::FUNCTIONS::UTIL;

	if (auto pSignInRequest = static_cast<FDBAccountRequest* const>(pRequestData)) {
		auto pSTMT = m_sqlPool.GetConnection()->AllocSTMT();
		SQLINTEGER iRetValue = 0;

		SQLPrepare(*pSTMT, (SQLWCHAR*)L"{? = CALL login (?, ?, ?, ?)}", SQL_NTS);

		SQLBindParameter(*pSTMT, 1, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &iRetValue, sizeof(SQLINTEGER), NULL);
		SQLBindParameter(*pSTMT, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SQL_VCHAR_DEFAULT_LENGTH, 0, reinterpret_cast<SQLCHAR*>(const_cast<char*>(pSignInRequest->m_sUserID)), MAX_NVARCHAR_LENGTH, NULL);
		SQLBindParameter(*pSTMT, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SQL_PWD_NVARCHAR_LENGTH, 0, reinterpret_cast<SQLCHAR*>(const_cast<char*>(pSignInRequest->m_sUserPassword)), SQL_PWD_NVARCHAR_LENGTH, NULL);
		SQLBindParameter(*pSTMT, 4, SQL_PARAM_OUTPUT, SQL_C_CHAR, SQL_VARCHAR, SQL_VCHAR_DEFAULT_LENGTH, 0, reinterpret_cast<SQLCHAR*>(const_cast<char*>(pSignInRequest->m_sUserName)), MAX_NVARCHAR_LENGTH, NULL);
		SQLBindParameter(*pSTMT, 5, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pSignInRequest->m_iUUID, sizeof(int32_t), NULL);


		if (SQLExecute(*pSTMT) != SQL_SUCCESS)
			GetMSSQLErrorMessage(SQL_HANDLE_STMT, *pSTMT);
		else {
			if (SignInErrorHandling(iRetValue, pSignInRequest))
				pSignInRequest->m_requestResult = FlatPacket::RequestMessageType_Succeeded;

			SQLFreeStmt(*pSTMT, SQL_CLOSE);
		}
		return std::shared_ptr<void>(pSignInRequest);
	}
	return nullptr;
}

std::shared_ptr<void> CUserDBSystem::SignUpRequest(void* const pRequestData) {
	using namespace SERVER::FUNCTIONS::UTIL;

	if (auto pSignUpRequest = static_cast<FDBAccountRequest* const>(pRequestData)) {
		auto pSTMT = m_sqlPool.GetConnection()->AllocSTMT();
		SQLINTEGER iRetValue = 0;

		SQLPrepare(*pSTMT, (SQLWCHAR*)L"{? = CALL create_account (?, ?, ?, ?)}", SQL_NTS);

		SQLBindParameter(*pSTMT, 1, SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &iRetValue, sizeof(SQLINTEGER), NULL);
		SQLBindParameter(*pSTMT, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SQL_VCHAR_DEFAULT_LENGTH, 0, reinterpret_cast<SQLCHAR*>(const_cast<char*>(pSignUpRequest->m_sUserID)), MAX_NVARCHAR_LENGTH, NULL);
		SQLBindParameter(*pSTMT, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SQL_PWD_NVARCHAR_LENGTH, 0, reinterpret_cast<SQLCHAR*>(const_cast<char*>(pSignUpRequest->m_sUserPassword)), SQL_PWD_NVARCHAR_LENGTH, NULL);
		SQLBindParameter(*pSTMT, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, SQL_VCHAR_DEFAULT_LENGTH, 0, reinterpret_cast<SQLCHAR*>(const_cast<char*>(pSignUpRequest->m_sUserName)), MAX_NVARCHAR_LENGTH, NULL);
		SQLBindParameter(*pSTMT, 5, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pSignUpRequest->m_iUUID, sizeof(int32_t), NULL);

		if (SQLExecute(*pSTMT) != SQL_SUCCESS)
			GetMSSQLErrorMessage(SQL_HANDLE_STMT, *pSTMT);
		else {
			if (SignUpErrorHandling(iRetValue, pSignUpRequest))
				pSignUpRequest->m_requestResult = FlatPacket::RequestMessageType_Succeeded;

			SQLFreeStmt(*pSTMT, SQL_CLOSE);
		}
		return std::shared_ptr<void>(pSignUpRequest);
	}
	return nullptr;
}

bool CUserDBSystem::SignInErrorHandling(const SQLINTEGER iQueryReturnValue, FDBBaseQueueData* const pBaseQueueData) {
	switch (iQueryReturnValue) {
	case 1:
		return true;
	case -2:
		pBaseQueueData->SetErrorMessage("Wrong User ID!");
		break;
	case -3:
		pBaseQueueData->SetErrorMessage("Wrong Password!");
		break;
	default:
		pBaseQueueData->SetErrorMessage("Unknown Error!");
		break;
	}
	return false;
}

bool CUserDBSystem::SignUpErrorHandling(const SQLINTEGER iQueryReturnValue, FDBBaseQueueData* const pBaseQueueData) {
	switch (iQueryReturnValue) {
	case 1:
		return true;
	case -1:
		pBaseQueueData->SetErrorMessage("Exist User ID!");
		break;
	case -2:
		pBaseQueueData->SetErrorMessage("Exist User Name!");
		break;
	default:
		pBaseQueueData->SetErrorMessage("Unknown Error!");
		break;
	}
	return false;
}