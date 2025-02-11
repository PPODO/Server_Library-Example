#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#include <flatbuffers/flatbuffers.h>
#include "../../../../Shared/Packet/data_define_generated.h"
#include <Functions/CircularQueue/CircularQueue.hpp>
#include <sqltypes.h>
#include "../../../../Shared/AttendanceInformation/AttendanceInformation.h"
#include "../../../../Shared/MailInformation/MailInformation.h"

using namespace SERVER::FUNCTIONS::CIRCULARQUEUE::QUEUEDATA;

#define MAX_NVARCHAR_LENGTH 50
#define MAX_PASSWORD_LENGTH 100
#define MAX_ERROR_MSG_LENGTH 100

enum EDBType : uint8_t {
	EDT_None,
	EDT_UserDB,
	EDT_GameDB,
	EDT_LogDB
};


struct FDBBaseQueueData {
public:
	EDBType m_dbType;

	FlatPacket::RequestMessageType m_requestResult;
	FlatPacket::PacketType m_packetType;
	char m_sErrorMessage[MAX_ERROR_MSG_LENGTH];

	void* m_pRequestedClientConnection;

public:
	FDBBaseQueueData(const EDBType dbType, const FlatPacket::PacketType packetType, void* pRequestedClientConnection) : m_requestResult(FlatPacket::RequestMessageType_Failed), m_packetType(packetType), m_pRequestedClientConnection(pRequestedClientConnection), m_dbType(dbType) {
		ZeroMemory(m_sErrorMessage, MAX_ERROR_MSG_LENGTH);
	};

public:
	void SetErrorMessage(const char* const sErrorMessage) {
		CopyMemory(m_sErrorMessage, sErrorMessage, MAX_ERROR_MSG_LENGTH);
	}

};

template<typename T>
struct FDBRequestBaseQueueData : public SERVER::FUNCTIONS::CIRCULARQUEUE::QUEUEDATA::BaseData<T>, public FDBBaseQueueData {
public:
	FDBRequestBaseQueueData(const EDBType dbType, const FlatPacket::PacketType packetType, void* pRequestedClientConnection) : FDBBaseQueueData(dbType, packetType, pRequestedClientConnection) {};
};

struct FDBAccountRequest : public FDBRequestBaseQueueData<FDBAccountRequest> {
public:
	char m_sUserID[MAX_NVARCHAR_LENGTH];
	char m_sUserPassword[MAX_PASSWORD_LENGTH];

	char m_sUserName[MAX_NVARCHAR_LENGTH];
	int32_t m_iUUID;
	bool m_bIsAlreadyLoggedIn;

public:
	FDBAccountRequest() : FDBRequestBaseQueueData<FDBAccountRequest>(EDBType::EDT_UserDB, FlatPacket::PacketType_None, nullptr), m_iUUID(), m_sUserName(), m_sUserID(), m_sUserPassword(), m_bIsAlreadyLoggedIn() {};
	
	FDBAccountRequest(const FlatPacket::PacketType packetType, void* pRequestedClientConnection, const char* const sUserID, const char* const sUserPassword) : FDBRequestBaseQueueData<FDBAccountRequest>(EDBType::EDT_UserDB, packetType, pRequestedClientConnection), m_iUUID(), m_sUserName(), m_bIsAlreadyLoggedIn() {
		CopyMemory(m_sUserID, sUserID, MAX_NVARCHAR_LENGTH);
		CopyMemory(m_sUserPassword, sUserPassword, MAX_PASSWORD_LENGTH);
	};

	FDBAccountRequest(const FlatPacket::PacketType packetType, const int32_t iUUID, void* pRequestedClientConnection, const char* const sUserID, const char* const sUserPassword, const char* const sUserName) : FDBRequestBaseQueueData<FDBAccountRequest>(EDBType::EDT_UserDB, packetType, pRequestedClientConnection), m_iUUID(iUUID), m_bIsAlreadyLoggedIn() {
		CopyMemory(m_sUserID, sUserID, MAX_NVARCHAR_LENGTH);
		CopyMemory(m_sUserPassword, sUserPassword, MAX_PASSWORD_LENGTH);
		CopyMemory(m_sUserName, sUserName, MAX_NVARCHAR_LENGTH);
	};

};


struct FDBUesrInformationRequest : public FDBRequestBaseQueueData<FDBUesrInformationRequest> {
public:
	int32_t m_iUUID;
	int32_t m_iNumOfUnconfirmedMails;

	std::vector<FItemInformation> m_userInventoryInformation;

public:
	FDBUesrInformationRequest() : FDBRequestBaseQueueData<FDBUesrInformationRequest>(EDBType::EDT_GameDB, FlatPacket::PacketType_None, nullptr), m_iUUID(), m_iNumOfUnconfirmedMails() {};

	FDBUesrInformationRequest(void* pRequestedClientConnection, const int32_t iUUID) : FDBRequestBaseQueueData<FDBUesrInformationRequest>(EDBType::EDT_GameDB, FlatPacket::PacketType::PacketType_DB_User_Information, pRequestedClientConnection), m_iUUID(iUUID), m_iNumOfUnconfirmedMails() {};

};


struct FDBAttendanceSheetsRequest : public FDBRequestBaseQueueData<FDBAttendanceSheetsRequest> {
public:
	int32_t m_iUUID;
	int16_t m_iRequestedEventType;

	std::vector<FUserAttendanceInformation> m_userAttendanceInformations;

public:
	FDBAttendanceSheetsRequest() : FDBRequestBaseQueueData<FDBAttendanceSheetsRequest>(EDBType::EDT_GameDB, FlatPacket::PacketType::PacketType_DB_Attendance_Sheets, nullptr), m_iUUID(), m_iRequestedEventType(), m_userAttendanceInformations() {};
	FDBAttendanceSheetsRequest(void* pRequestedClientConnection, const int32_t iUUID, const int16_t iRequestedEventType) 
		: FDBRequestBaseQueueData<FDBAttendanceSheetsRequest>(EDBType::EDT_GameDB, FlatPacket::PacketType::PacketType_DB_Attendance_Sheets, pRequestedClientConnection)
		, m_iUUID(iUUID), m_iRequestedEventType(iRequestedEventType), m_userAttendanceInformations() {};

};


struct FDBAttendanceApplyRequest : public FDBRequestBaseQueueData<FDBAttendanceApplyRequest> {
public:
	uint8_t m_attendanceType;
	int32_t m_iUUID;
	uint32_t m_iEventKey;
	SQL_DATE_STRUCT m_updatedAttendanceDate;
	int16_t m_iUpdatedAttendanceCount;

	std::vector<FItemInformation> m_attendanceRewards;
	std::vector<FMailStructure> m_attendanceMailRewards;

public:
	FDBAttendanceApplyRequest()
		: FDBRequestBaseQueueData<FDBAttendanceApplyRequest>(EDBType::EDT_GameDB, FlatPacket::PacketType::PacketType_DB_Attendance_Check, nullptr), m_iUUID(), m_iEventKey(), m_updatedAttendanceDate(), m_attendanceType(), m_iUpdatedAttendanceCount(), m_attendanceRewards(), m_attendanceMailRewards() {};

	FDBAttendanceApplyRequest(void* pRequestedClientConnection, const int32_t iUUID, const uint32_t iEventKey, const uint8_t attendanceType)
		: FDBRequestBaseQueueData<FDBAttendanceApplyRequest>(EDBType::EDT_GameDB, FlatPacket::PacketType::PacketType_DB_Attendance_Check, pRequestedClientConnection), m_iUUID(iUUID), m_iEventKey(iEventKey), m_attendanceType(attendanceType)
		, m_updatedAttendanceDate(), m_iUpdatedAttendanceCount(), m_attendanceRewards(), m_attendanceMailRewards() {
	};

	FDBAttendanceApplyRequest(void* pRequestedClientConnection, const int32_t iUUID, const uint32_t iEventKey, const uint8_t attendanceType, const SQL_DATE_STRUCT& updatedAttendanceDate, const int16_t iUpdatedAttendanceCount, const std::vector<FItemInformation>& attendanceRewards, const std::vector<FMailStructure>& attendanceMailRewards)
		: FDBRequestBaseQueueData<FDBAttendanceApplyRequest>(EDBType::EDT_GameDB, FlatPacket::PacketType::PacketType_DB_Attendance_Check, pRequestedClientConnection), m_iUUID(iUUID), m_iEventKey(iEventKey), m_attendanceType(attendanceType)
		, m_updatedAttendanceDate(updatedAttendanceDate), m_iUpdatedAttendanceCount(iUpdatedAttendanceCount)
		, m_attendanceRewards(attendanceRewards), m_attendanceMailRewards(attendanceMailRewards) {
	};

};


struct FDBGetMailsRequest : public FDBRequestBaseQueueData<FDBGetMailsRequest> {
public:
	int32_t m_iUUID;
	int16_t m_iPage;

	int16_t m_iRowCountLimit;
	int16_t m_iTotalMailCount;
	std::vector<FMailStructure> m_mailInformations;

public:
	FDBGetMailsRequest() : FDBRequestBaseQueueData<FDBGetMailsRequest>(EDBType::EDT_GameDB, FlatPacket::PacketType::PacketType_DB_MailsInfo, nullptr), m_iUUID(), m_iPage(), m_iRowCountLimit(), m_iTotalMailCount(), m_mailInformations() {};

	FDBGetMailsRequest(void* pRequestedClientConnection, const int32_t iUUID, const int16_t iPage)
		: FDBRequestBaseQueueData<FDBGetMailsRequest>(EDBType::EDT_GameDB, FlatPacket::PacketType::PacketType_DB_MailsInfo, pRequestedClientConnection),
		m_iUUID(iUUID), m_iPage(iPage), m_iRowCountLimit(), m_iTotalMailCount(), m_mailInformations() {
	};

};


struct FDBConfirmMailRequest : public FDBRequestBaseQueueData<FDBConfirmMailRequest> {
public:
	int32_t m_iUUID;
	int64_t m_iMailID;

public:
	FDBConfirmMailRequest() : FDBRequestBaseQueueData<FDBConfirmMailRequest>(EDBType::EDT_GameDB, FlatPacket::PacketType::PacketType_DB_MailConfirm, nullptr), m_iUUID(), m_iMailID() {};
	FDBConfirmMailRequest(void* pRequestedClientConnection, const int32_t iUUID, const int64_t iMailID)
		: FDBRequestBaseQueueData<FDBConfirmMailRequest>(EDBType::EDT_GameDB, FlatPacket::PacketType::PacketType_DB_MailConfirm, pRequestedClientConnection), m_iUUID(iUUID), m_iMailID(iMailID) {};

};


struct FDBReceiveMailsRequest : public FDBRequestBaseQueueData<FDBReceiveMailsRequest> {
public:
	int32_t m_iUUID;
	int64_t m_iMailID;
	bool m_bReceiveAll;

	std::vector<FItemInformation> m_receivedItems;

public:
	FDBReceiveMailsRequest() : FDBRequestBaseQueueData<FDBReceiveMailsRequest>(EDBType::EDT_GameDB, FlatPacket::PacketType::PacketType_DB_MailReceive, nullptr), m_iUUID(), m_iMailID(), m_bReceiveAll(), m_receivedItems() {};

	FDBReceiveMailsRequest(void* pRequestedClientConnection, const int32_t iUUID, const int64_t iMailID, const bool bReceiveAll)
		: FDBRequestBaseQueueData<FDBReceiveMailsRequest>(EDBType::EDT_GameDB, FlatPacket::PacketType::PacketType_DB_MailReceive, pRequestedClientConnection), m_iUUID(iUUID), m_iMailID(iMailID), m_bReceiveAll(bReceiveAll), m_receivedItems() {};

};


struct FDBCheckExpiredMailsRequest : public FDBRequestBaseQueueData<FDBCheckExpiredMailsRequest> {
public:
	FDBCheckExpiredMailsRequest() : FDBRequestBaseQueueData<FDBCheckExpiredMailsRequest>(EDBType::EDT_GameDB, FlatPacket::PacketType_DB_CheckExpiredMails, nullptr) {};
};