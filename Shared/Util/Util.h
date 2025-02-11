#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#include <flatbuffers/flatbuffers.h>
#include <Network/Packet/Serialization/serialization.hpp>
#include <sql.h>
#include <sqltypes.h>
#include <regex>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../Packet/data_define_generated.h"
#include "../Packet/account_packet_generated.h"
#include "../Packet/attendance_data_define_generated.h"
#include "../Packet/attendance_check_packet_generated.h"
#include "../Packet/attendance_sheet_packet_generated.h"
#include "../Packet/error_packet_generated.h"
#include "../Packet/check_mail_generated.h"
#include "../Packet/mail_info_packet_generated.h"
#include "../Packet/receive_mail_generated.h"
#include "../Packet/json_download_notify_generated.h"
#include "../Packet/json_download_packet_generated.h"
#include "../Packet/user_information_packet_generated.h"
#include "../Packet/session_data_define_generated.h"
#include "../Packet/find_session_packet_generated.h"
#include "../Packet/create_session_packet_generated.h"
#include "../Packet/session_info_generated.h"
#include "../Packet/join_session_packet_generated.h"
#include "../Packet/session_user_request_generated.h"
#include "../Packet/session_game_start_packet_generated.h"
#include "../Packet/in_game_data_define_generated.h"
#include "../Packet/join_game_packet_generated.h"
#include "../Packet/vehicle_information_generated.h"
#include "../Packet/ready_packet_generated.h"
#include "../Packet/user_game_data_packet_generated.h"
#include "../Packet/player_game_data_packet_generated.h"
#include "../SessionInformation/SessionInformation.h"

using namespace SERVER::NETWORK::PACKET::UTIL::SERIALIZATION;

enum EEventType : uint8_t {
	EEID_None,
	EEID_Daily,
	EEID_New,
	EEID_Special
};

static SYSTEMTIME GetCurrentDate() {
	SYSTEMTIME sysTime;
	auto localTime = boost::posix_time::second_clock::local_time();

	sysTime.wYear = localTime.date().year();
	sysTime.wMonth = localTime.date().month();
	sysTime.wDay = localTime.date().day();
	sysTime.wHour = localTime.time_of_day().hours();
	sysTime.wMinute = localTime.time_of_day().minutes();
	sysTime.wSecond = localTime.time_of_day().seconds();

	return sysTime;
}

static std::string RemoveEscapeCharactersFromJsonString(const std::string& jsonString) {
	std::regex escapeCharacters("[(\n\t)]");

	return std::regex_replace(jsonString, escapeCharacters, "");
}

static SYSTEMTIME operator-(const SYSTEMTIME& lhs, const uint16_t& iDays) {
	const boost::posix_time::ptime newDate(boost::gregorian::date(lhs.wYear, lhs.wMonth, lhs.wDay) - boost::gregorian::days(iDays));
	SYSTEMTIME sysTime;

	sysTime.wYear = newDate.date().year();
	sysTime.wMonth = newDate.date().month();
	sysTime.wDay = newDate.date().day();

	sysTime.wHour = lhs.wHour;
	sysTime.wMinute = lhs.wMinute;
	sysTime.wSecond = lhs.wSecond;

	return sysTime;
}

static bool operator==(const SYSTEMTIME& lhs, const SQL_DATE_STRUCT& rhs) {
	return boost::gregorian::date(lhs.wYear, lhs.wMonth, lhs.wDay) == boost::gregorian::date(rhs.year, rhs.month, rhs.day);

	return (lhs.wYear + lhs.wMonth + lhs.wDay) - (rhs.year + rhs.month + rhs.day);
}

static short GetLastDayOfMonth(const unsigned short& iYear, const unsigned short& iMonth) {
	return boost::gregorian::gregorian_calendar::end_of_month_day(iYear, iMonth);
}

static std::string GetTimeLeftUntilExpiration(const SQL_TIMESTAMP_STRUCT& receivedDateTime, const int16_t iDays) {
	const auto& currentSystemTime = ::GetCurrentDate();
	boost::posix_time::ptime expiredDate(boost::gregorian::date(receivedDateTime.year, receivedDateTime.month, receivedDateTime.day) + boost::gregorian::days(iDays), boost::posix_time::time_duration(receivedDateTime.hour - 1, 0, 0));
	boost::posix_time::ptime currentDate(boost::gregorian::date(currentSystemTime.wYear, currentSystemTime.wMonth, currentSystemTime.wDay), boost::posix_time::time_duration(currentSystemTime.wHour, 0, 0));

	const auto& remainDate = (expiredDate - currentDate);
	return std::to_string(remainDate.hours() / 24) + "Day " + std::to_string(remainDate.hours() % 24) + "Hour";
}

static uint32_t GetEventKey(const uint16_t iEventType, const uint16_t iEventID) {
	return ((iEventType << (sizeof(uint16_t) * 8)) | iEventID);
}

static uint16_t GetEventTypeFromEventKey(const uint32_t iEventKey) {
	return static_cast<uint16_t>(iEventKey >> (sizeof(uint16_t) * 8));
}

static uint16_t GetEventIDFromEventKey(const uint32_t iEventKey) {
	return static_cast<uint16_t>(iEventKey);
}

static std::string SQLDateTimeToString(const SQL_DATE_STRUCT& date) {
	return std::to_string(date.year) + "-" + std::to_string(date.month) + "-" + std::to_string(date.day);
}

static std::string SQLTimeStampToString(const SQL_TIMESTAMP_STRUCT& date) {
	return std::to_string(date.year) + "-" + std::to_string(date.month) + "-" + std::to_string(date.day) + ":" + std::to_string(date.hour) + "-" + std::to_string(date.minute) + "-" + std::to_string(date.second);
}

static std::string SQLTimeStampToSQLDateString(const SQL_TIMESTAMP_STRUCT& date) {
	return std::to_string(date.year) + "-" + std::to_string(date.month) + "-" + std::to_string(date.day);
}

static SQL_DATE_STRUCT StringToSQLDate(const std::string& str) {
	SQL_DATE_STRUCT ret;

	auto iYearOffset = str.find('-');
	ret.year = std::atoi(str.substr(0, iYearOffset).c_str());
	auto iMonthOffset = str.find('-', iYearOffset + 1);
	ret.month = std::atoi(str.substr(iYearOffset + 1, iMonthOffset).c_str());
	ret.day = std::atoi(str.substr(iMonthOffset + 1).c_str());

	return ret;
}

static SQL_TIMESTAMP_STRUCT StringToSQLTimeStamp(const std::string& str) {
	SQL_TIMESTAMP_STRUCT ret;

	auto iYearOffset = str.find('-');
	ret.year = std::atoi(str.substr(0, iYearOffset).c_str());
	auto iMonthOffset = str.find('-', iYearOffset + 1);
	ret.month = std::atoi(str.substr(iYearOffset + 1, iMonthOffset).c_str());
	ret.day = std::atoi(str.substr(iMonthOffset + 1).c_str());

	return ret;
}

static SQL_DATE_STRUCT SystemTimeToSQLDateStruct(const SYSTEMTIME& sysTime) {
	return SQL_DATE_STRUCT{ static_cast<short>(sysTime.wYear), sysTime.wMonth,sysTime.wDay };
}

static SQL_TIMESTAMP_STRUCT SystemTimeToSQLTimeStampStruct(const SYSTEMTIME& sysTime) {
	SQL_TIMESTAMP_STRUCT ret;
	ret.year = sysTime.wYear;
	ret.month = sysTime.wMonth;
	ret.day = sysTime.wDay;
	ret.hour = sysTime.wHour;
	ret.minute = sysTime.wMinute;
	ret.second = sysTime.wSecond;
	ret.fraction = 0;

	return ret;
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateAccountPacket(flatbuffers::FlatBufferBuilder& builder, const FlatPacket::PacketType packetType, const int16_t iRequestResult = 0, const std::string& sUserID = {}, const std::string& sUserPWD = {}, const std::string& sUserName = {}, const int32_t iUUID = 0, const bool bIsAlreadyLoggedIn = false) {
	return Serialize<FlatPacket::AccountPacket::AccountPacket>(builder, packetType, FlatPacket::AccountPacket::CreateAccountPacket(builder, iRequestResult, builder.CreateString(sUserID), builder.CreateString(sUserPWD), builder.CreateString(sUserName), iUUID, bIsAlreadyLoggedIn));
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateJsonDownloadNotificationPacket(flatbuffers::FlatBufferBuilder& builder, const uint8_t iPacketType) {
	return Serialize<FlatPacket::JsonPacket::JsonDownloadNotifyPacket>(builder, iPacketType, FlatPacket::JsonPacket::CreateJsonDownloadNotifyPacket(builder));
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateAttendanceSheetsPacket(flatbuffers::FlatBufferBuilder& builder, const int16_t iRequestResult = 0, const int32_t iUUID = 0, const int16_t iRequestedEventType = 0, const std::vector<flatbuffers::Offset<FlatPacket::AttendancePacket::AttendanceDetail>>& attendanceDetails = {}) {
	return Serialize<FlatPacket::AttendancePacket::AttendanceSheetsPacket>(builder, FlatPacket::PacketType::PacketType_Attendance_Sheets, FlatPacket::AttendancePacket::CreateAttendanceSheetsPacket(builder, iRequestResult, iUUID, iRequestedEventType, builder.CreateVector(attendanceDetails)));
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateAttendanceCheckPacket(flatbuffers::FlatBufferBuilder& builder, const int16_t iRequestResult = 0, const uint8_t iAttendanceType = 0, const int32_t iEventKey = 0, const int32_t iUUID = 0, const int32_t iUpdatedAttendanceCount = 0) {
	return Serialize<FlatPacket::AttendancePacket::AttendanceCheckPacket>(builder, FlatPacket::PacketType::PacketType_Attendance_Check, FlatPacket::AttendancePacket::CreateAttendanceCheckPacket(builder, iRequestResult, iAttendanceType, iEventKey, iUUID, iUpdatedAttendanceCount));
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateErrorPacket(flatbuffers::FlatBufferBuilder& builder, const std::string& sErrorMessage, const bool bCriticalError) {
	return Serialize<FlatPacket::Error::ErrorMessagePacket>(builder, FlatPacket::PacketType::PacketType_Error, FlatPacket::Error::CreateErrorMessagePacket(builder, builder.CreateString(sErrorMessage), bCriticalError));
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateMailsInfoPacket(flatbuffers::FlatBufferBuilder& builder, const int16_t iRequestResult = 0, const int32_t iUUID = 0, const int16_t iRowCountLimit = 0, const int16_t iTotalMailCount = 0, const int16_t iPage = 0, const std::vector<flatbuffers::Offset<FlatPacket::MailPacket::mail>>& mails = {}) {
	return Serialize<FlatPacket::MailPacket::ReceivedMailInformationPacket>(builder, FlatPacket::PacketType::PacketType_MailsInfo, FlatPacket::MailPacket::CreateReceivedMailInformationPacket(builder, iRequestResult, iUUID, iRowCountLimit, iTotalMailCount, iPage, builder.CreateVector(mails)));
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateConfirmMailPacket(flatbuffers::FlatBufferBuilder& builder, const int16_t iRequestResult = 0, const int32_t iUUID = 0, const int64_t iMailID = 0) {
	return Serialize<FlatPacket::MailPacket::CheckMail>(builder, FlatPacket::PacketType::PacketType_MailConfirm, FlatPacket::MailPacket::CreateCheckMail(builder, iRequestResult, iUUID, iMailID));
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateMailReceivePacket(flatbuffers::FlatBufferBuilder& builder, const int32_t iUUID = 0, const int16_t iPage = 0, const int64_t iMailID = 0, const bool bReceiveAll = false, const std::vector<flatbuffers::Offset<FlatPacket::item>>& receivedItems = {}) {
	return Serialize<FlatPacket::MailPacket::MailReceive>(builder, FlatPacket::PacketType::PacketType_MailReceive, FlatPacket::MailPacket::CreateMailReceive(builder, iUUID, iPage, iMailID, bReceiveAll, builder.CreateVector(receivedItems)));
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateUserInformationPacket(flatbuffers::FlatBufferBuilder& builder, const int16_t iRequestResult = 0, const int32_t iUUID = 9, const int32_t iNumOfUnconfirmedMails = 0, const std::vector<flatbuffers::Offset<FlatPacket::item>>& inventory = {}) {
	return Serialize<FlatPacket::UserPacket::UserInformationPacket>(builder, FlatPacket::PacketType::PacketType_User_Information, FlatPacket::UserPacket::CreateUserInformationPacket(builder, iRequestResult, iUUID, iNumOfUnconfirmedMails, builder.CreateVector(inventory)));
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateJoinSessionPacket(flatbuffers::FlatBufferBuilder& builder, FSessionStructure& sessionInfoOffsetData, const int32_t iSessionID = 0, const int32_t iUUID = 0) {
	return Serialize<FlatPacket::SessionPacket::JoinSessionPacket>(builder, FlatPacket::PacketType::PacketType_JoinSession, FlatPacket::SessionPacket::CreateJoinSessionPacket(builder, iSessionID, iUUID, sessionInfoOffsetData.CreateSessionInformationOffsetData(builder)));
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateFindSessionPacket(flatbuffers::FlatBufferBuilder& builder, const int32_t iRequestedPage = 0, const int32_t iMaxPage = 0, const std::vector<flatbuffers::Offset<FlatPacket::SessionPacket::session_information>>& sessionsList = {}) {
	return Serialize<FlatPacket::SessionPacket::FindSessionPacket>(builder, FlatPacket::PacketType::PacketType_FindSession, FlatPacket::SessionPacket::CreateFindSessionPacket(builder, iRequestedPage, iMaxPage, builder.CreateVector(sessionsList)));
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateSessionRequestPacket(flatbuffers::FlatBufferBuilder& builder, const int32_t iSessionID = 0, const int32_t iUUID = 0, const int16_t iRequest = 0, const char* const sMessage = "") {
	return Serialize<FlatPacket::SessionPacket::SessionUserRequestPacket>(builder, FlatPacket::PacketType::PacketType_SessionRequest, FlatPacket::SessionPacket::CreateSessionUserRequestPacket(builder, iSessionID, iUUID, iRequest, builder.CreateString(sMessage)));
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateGameStartPacket(flatbuffers::FlatBufferBuilder& builder, const std::string& sServerIPAddress, const int16_t iPortNumber) {
	return Serialize<FlatPacket::SessionPacket::SessionGameStartPacket>(builder, FlatPacket::PacketType::PacketType_GameStart, FlatPacket::SessionPacket::CreateSessionGameStartPacket(builder, builder.CreateString(sServerIPAddress.c_str()), iPortNumber));
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateJoinInGamePacket(flatbuffers::FlatBufferBuilder& builder, const std::vector<flatbuffers::Offset<FlatInGamePacket::Game::UserSpawnInfo>>& usersSpawnInfo) {
	return Serialize<FlatInGamePacket::Game::JoinGamePacket>(builder, FlatInGamePacket::PacketType::PacketType_Join, FlatInGamePacket::Game::CreateJoinGamePacket(builder, builder.CreateVector(usersSpawnInfo)));
}

static SERVER::NETWORK::PACKET::PACKET_STRUCT CreateGameDataPacket(flatbuffers::FlatBufferBuilder& builder, const std::vector<flatbuffers::Offset<FlatInGamePacket::Game::GameData>>& usersGameData) {
	return Serialize<FlatInGamePacket::Game::GameDataPacket>(builder, FlatInGamePacket::PacketType::PacketType_GameData, FlatInGamePacket::Game::CreateGameDataPacket(builder, builder.CreateVector(usersGameData)));
}