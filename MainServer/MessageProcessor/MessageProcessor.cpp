#include "MessageProcessor.h"
#include "../IOCP/IOCP.h"
#include <Functions/Log/Log.hpp>

using namespace MAINSERVER;

CMessageProcessor::CMessageProcessor(const std::chrono::milliseconds& durationForProcessingThread, const std::function<void(FTransmitQueueData*)>& messageProcessedCallback)
	: m_databaseManager(durationForProcessingThread, std::bind(&CMessageProcessor::AddNewMessage, this, std::placeholders::_1)), m_durationForProcessingThread(durationForProcessingThread)
	, m_messageProcessedCallback(messageProcessedCallback), m_bMessageProcessingThreadRunState(true) {

	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_Sign_In, std::bind(&CMessageProcessor::SignInProcessing, this, std::placeholders::_1));
	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_Sign_Up, std::bind(&CMessageProcessor::SignUpProcessing, this, std::placeholders::_1));

	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_User_Information, std::bind(&CMessageProcessor::UserInformationProcessing, this, std::placeholders::_1));

	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_Attendance_Sheets, std::bind(&CMessageProcessor::AttendanceSheetsProcessing, this, std::placeholders::_1));
	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_Attendance_Check, std::bind(&CMessageProcessor::AttendanceCheckProcessing, this, std::placeholders::_1));

	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_MailsInfo, std::bind(&CMessageProcessor::GetMailsByPageProcessing, this, std::placeholders::_1));
	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_MailConfirm, std::bind(&CMessageProcessor::ConfirmMailByMailIDProcessing, this, std::placeholders::_1));
	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_MailReceive, std::bind(&CMessageProcessor::MailReceiveProcessing, this, std::placeholders::_1));

	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_CreateSession, std::bind(&CMessageProcessor::CreateSessionProcessing, this, std::placeholders::_1));
	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_FindSession, std::bind(&CMessageProcessor::FindSessionProcessing, this, std::placeholders::_1));
	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_JoinSession, std::bind(&CMessageProcessor::JoinSessionProcessing, this, std::placeholders::_1));
	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_SessionRequest, std::bind(&CMessageProcessor::UserSessionRequestProcessing, this, std::placeholders::_1));

	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_Disconnect, std::bind(&CMessageProcessor::ClientDisconnectProcessing, this, std::placeholders::_1));


	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_DB_Sign_In, std::bind(&CMessageProcessor::DBSignInProcessed, this, std::placeholders::_1));
	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_DB_Sign_Up, std::bind(&CMessageProcessor::DBSignUpProcessed, this, std::placeholders::_1));

	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_DB_User_Information, std::bind(&CMessageProcessor::DBUserInformationProcessed, this, std::placeholders::_1));

	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_DB_Attendance_Sheets, std::bind(&CMessageProcessor::DBAttendanceSheetsProcessed, this, std::placeholders::_1));
	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_DB_Attendance_Check, std::bind(&CMessageProcessor::DBAttendanceCheckProcessed, this, std::placeholders::_1));

	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_DB_MailsInfo, std::bind(&CMessageProcessor::DBGetMailsByPageProcessed, this, std::placeholders::_1));
	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_DB_MailConfirm, std::bind(&CMessageProcessor::DBConfirmMailByMailIDProcessed, this, std::placeholders::_1));
	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_DB_MailReceive, std::bind(&CMessageProcessor::DBMailReceiveProcessed, this, std::placeholders::_1));


	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_JsonDownloadStartPacket, std::bind(&CMessageProcessor::JsonDownloadStartProcessing, this, std::placeholders::_1));
	m_messageProcessor.emplace(FlatPacket::PacketType::PacketType_JsonDownloadingPacket, std::bind(&CMessageProcessor::JsonDataDownloadProcessing, this, std::placeholders::_1));


	m_pMessageProcessingQueue = std::make_unique<MESSAGEQUEUE>();
	m_pMessageStockQueue = std::make_unique<MESSAGEQUEUE>();

	m_messageProcessingThread = std::thread(std::bind(&CMessageProcessor::MessageProcessingWorkerThread, this));
}

CMessageProcessor::~CMessageProcessor() {
	m_bMessageProcessingThreadRunState = false;
	if (m_messageProcessingThread.joinable())
		m_messageProcessingThread.join();
}

bool CMessageProcessor::Initialize(const std::string& sAttendanceJsonFilePath, const std::string& sGameServerExeFilePath) {
	m_timerSystem.AddNewCallbackToOnTimeTimer([&]() {
		m_databaseManager.AddNewDBRequestData(new FDBCheckExpiredMailsRequest);
	});
	
	m_attendanceSystem.Initialize(sAttendanceJsonFilePath);
	m_jsonDownloadSystem.Initialize(sAttendanceJsonFilePath);
	m_sessionSystem.Initialize(sGameServerExeFilePath);

	return true;
}

void CMessageProcessor::AddNewMessage(FMessageData* pNewMessageData) {
	SERVER::FUNCTIONS::CRITICALSECTION::CriticalSectionGuard lck(m_csForMessageQueue);
	m_pMessageStockQueue->Push(pNewMessageData);
}

void CMessageProcessor::MessageProcessingWorkerThread() {
	while (m_bMessageProcessingThreadRunState) {
		std::unique_lock<std::mutex> lck(m_csForProcessingThread);
		if (m_cvForProcessingThread.wait_for(lck, m_durationForProcessingThread, [&]() { return !m_pMessageProcessingQueue->IsEmpty(); })) {
			lck.unlock();

			FMessageData* pMessageData = nullptr;
			if (m_pMessageProcessingQueue->Pop(pMessageData)) {
				if (auto ret = MessageProcessing(pMessageData))
					m_messageProcessedCallback(ret);

				delete pMessageData;
			}
		}
		else if (m_pMessageProcessingQueue->IsEmpty() && !m_pMessageStockQueue->IsEmpty()) {
			SERVER::FUNCTIONS::CRITICALSECTION::CriticalSectionGuard lck(m_csForMessageQueue);
			m_pMessageProcessingQueue.swap(m_pMessageStockQueue);
		}
	}
}

FTransmitQueueData* CMessageProcessor::MessageProcessing(const FMessageData* const  pMessageData) {
	if (pMessageData) {
		const auto& findResult = m_messageProcessor.find(pMessageData->m_messageType);
		if (findResult != m_messageProcessor.cend())
			return findResult->second(pMessageData);
	}
	return nullptr;
}

FTransmitQueueData* CMessageProcessor::SignInProcessing(const FMessageData* const  pMessageData) {
	if (auto pPacketStruct = std::static_pointer_cast<PACKET_STRUCT, void>(pMessageData->m_pMessage)) {
		if (auto pSignInPacket = FlatPacket::AccountPacket::GetAccountPacket(pPacketStruct->m_sPacketData))
			m_databaseManager.AddNewDBRequestData(new FDBAccountRequest(FlatPacket::PacketType::PacketType_DB_Sign_In, pMessageData->m_pRequestedClientConnection, pSignInPacket->user_id()->c_str(), pSignInPacket->user_pwd()->c_str()));
	}
	return nullptr;
}

FTransmitQueueData* CMessageProcessor::SignUpProcessing(const FMessageData* const  pMessageData) {
	if (auto pPacketStruct = std::static_pointer_cast<PACKET_STRUCT, void>(pMessageData->m_pMessage)) {
		if (auto pSignUpPacket = FlatPacket::AccountPacket::GetAccountPacket(pPacketStruct->m_sPacketData))
			m_databaseManager.AddNewDBRequestData(new FDBAccountRequest(FlatPacket::PacketType::PacketType_DB_Sign_Up, m_systemInstance.GenerateUUID(), pMessageData->m_pRequestedClientConnection, pSignUpPacket->user_id()->c_str(), pSignUpPacket->user_pwd()->c_str(), pSignUpPacket->user_name()->c_str()));
	}
	return nullptr;
}

FTransmitQueueData* CMessageProcessor::UserInformationProcessing(const FMessageData* const pMessageData) {
	if (auto pPacketStruct = std::static_pointer_cast<PACKET_STRUCT, void>(pMessageData->m_pMessage)) {
		if (auto pUserInfoPacket = FlatPacket::UserPacket::GetUserInformationPacket(pPacketStruct->m_sPacketData))
			m_databaseManager.AddNewDBRequestData(new FDBUesrInformationRequest(pMessageData->m_pRequestedClientConnection, pUserInfoPacket->uuid()));
	}
	return nullptr;
}


FTransmitQueueData* CMessageProcessor::AttendanceSheetsProcessing(const FMessageData* const pMessageData) {
	using namespace FlatPacket;
	using namespace SERVER::FUNCTIONS::UTIL;

	auto flatBuffer = std::make_unique<FFlatBuffer>();
	RequestMessageType requestResultMessageType = RequestMessageType::RequestMessageType_Failed;
	uint16_t iRequestEventType = 0;
	std::vector<flatbuffers::Offset<FlatPacket::AttendancePacket::AttendanceDetail>> attendanceDetails;

	if (auto pPacketStruct = std::static_pointer_cast<PACKET_STRUCT, void>(pMessageData->m_pMessage)) {
		if (auto pAttendanceSheetPacket = FlatPacket::AttendancePacket::GetAttendanceSheetsPacket(pPacketStruct->m_sPacketData))
			m_databaseManager.AddNewDBRequestData(new FDBAttendanceSheetsRequest(pMessageData->m_pRequestedClientConnection, pAttendanceSheetPacket->uuid(), pAttendanceSheetPacket->requested_event_type()));
	}
	return nullptr;
}

FTransmitQueueData* CMessageProcessor::AttendanceCheckProcessing(const FMessageData* const pMessageData) {
	using namespace FlatPacket;

	if (auto pPacketStruct = std::static_pointer_cast<PACKET_STRUCT, void>(pMessageData->m_pMessage)) {
		if (auto pAttendanceCheckPacket = AttendancePacket::GetAttendanceCheckPacket(pPacketStruct->m_sPacketData)) {
			const auto iUUID = pAttendanceCheckPacket->uuid();
			const auto iEventKey = pAttendanceCheckPacket->event_key();
			const auto attendanceType = pAttendanceCheckPacket->attendance_type();

			FUserAttendanceInformation userAttendanceInformation;
			int16_t iUpdatedAttendanceCount = 0, iCurrentAttendanceCount = 0;
			SQL_DATE_STRUCT updatedAttendanceDate, currentAttendanceDate;
			std::string sErrorMessage = "Not ~~";
			std::vector<FItemInformation> inventoryItems;
			std::vector<FMailStructure> attendanceMailRewards;

			if (m_userSystem.CalculateCostRequest(iUUID, pAttendanceCheckPacket->cost_item_id(), pAttendanceCheckPacket->cost_item_count())) {
				if (m_userSystem.GetUserAttendanceInformationByUUIDAndEventKey(iUUID, iEventKey, userAttendanceInformation)) {
					currentAttendanceDate = userAttendanceInformation.m_lastAttendanceDate;
					iCurrentAttendanceCount = userAttendanceInformation.m_iAttendanceCount;
				}
				else
					currentAttendanceDate = SystemTimeToSQLDateStruct(GetCurrentDate() - 2);

				if (m_attendanceSystem.AttendanceCheck(iEventKey, attendanceType, currentAttendanceDate, iCurrentAttendanceCount, iUpdatedAttendanceCount, updatedAttendanceDate, sErrorMessage, inventoryItems, attendanceMailRewards)) {
					m_databaseManager.AddNewDBRequestData(new FDBAttendanceApplyRequest(pMessageData->m_pRequestedClientConnection, pAttendanceCheckPacket->uuid(), pAttendanceCheckPacket->event_key(), pAttendanceCheckPacket->attendance_type(), updatedAttendanceDate, iUpdatedAttendanceCount, inventoryItems, attendanceMailRewards));
					return nullptr;
				}
			}
			auto flatBuffer = std::make_unique<FFlatBuffer>();
			SERVER::FUNCTIONS::LOG::Log::WriteLog(L"Client [%d] Attendance Check Reqeust Failed!", iUUID);

			return new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateErrorPacket(flatBuffer->m_flatBuffer, sErrorMessage, false));
		}
	}
	return nullptr;
}

FTransmitQueueData* CMessageProcessor::GetMailsByPageProcessing(const FMessageData* const pMessageData) {
	using namespace FlatPacket;

	if (auto pPacketStruct = std::static_pointer_cast<PACKET_STRUCT, void>(pMessageData->m_pMessage)) {
		if (auto pGetMailsByPagePacket = FlatPacket::MailPacket::GetReceivedMailInformationPacket(pPacketStruct->m_sPacketData))
			m_databaseManager.AddNewDBRequestData(new FDBGetMailsRequest(pMessageData->m_pRequestedClientConnection, pGetMailsByPagePacket->uuid(), pGetMailsByPagePacket->page()));
	}
	return nullptr;
}

FTransmitQueueData* CMessageProcessor::ConfirmMailByMailIDProcessing(const FMessageData* const pMessageData) {
	using namespace FlatPacket;

	if (auto pPacketStruct = std::static_pointer_cast<PACKET_STRUCT, void>(pMessageData->m_pMessage)) {
		if (auto pConfirmMailByMailIDPacket = FlatPacket::MailPacket::GetCheckMail(pPacketStruct->m_sPacketData))
			m_databaseManager.AddNewDBRequestData(new FDBConfirmMailRequest(pMessageData->m_pRequestedClientConnection, pConfirmMailByMailIDPacket->uuid(), pConfirmMailByMailIDPacket->mail_id()));
	}
	return nullptr;
}

FTransmitQueueData* CMessageProcessor::MailReceiveProcessing(const FMessageData* const pMessageData) {
	if (auto pPacketStruct = std::static_pointer_cast<PACKET_STRUCT, void>(pMessageData->m_pMessage)) {
		if (auto pReceiveMailPacket = FlatPacket::MailPacket::GetMailReceive(pPacketStruct->m_sPacketData))
			m_databaseManager.AddNewDBRequestData(new FDBReceiveMailsRequest(pMessageData->m_pRequestedClientConnection, pReceiveMailPacket->uuid(), pReceiveMailPacket->mail_id(), pReceiveMailPacket->receive_all()));
	}
	return nullptr;
}


FTransmitQueueData* CMessageProcessor::CreateSessionProcessing(const FMessageData* const pMessageData) {
	using namespace SERVER::FUNCTIONS::UTIL;
	using namespace SERVER::FUNCTIONS::LOG;

	FTransmitQueueData* pNewTransmitQueueData = nullptr;

	if (auto pPacketStruct = std::static_pointer_cast<PACKET_STRUCT, void>(pMessageData->m_pMessage)) {
		if (auto pCreateSessionPacket = FlatPacket::SessionPacket::GetCreateSession(pPacketStruct->m_sPacketData)) {
			auto flatBuffer = std::make_unique<FFlatBuffer>();
			FUserInformation requestedUserInformation;
			FSessionStructure createdSessionInfo;

			if (m_userSystem.GetUserInformationByUUID(pCreateSessionPacket->requested_user_uuid(), requestedUserInformation) &&
				m_sessionSystem.CreateNewSession(pCreateSessionPacket->session_name()->c_str(), FSessionUserInformation(pMessageData->m_pRequestedClientConnection, pCreateSessionPacket->requested_user_uuid(), requestedUserInformation.m_sUserName, true), pCreateSessionPacket->max_users_count(), createdSessionInfo) &&
				m_userSystem.SetParticipatingSessionID(pCreateSessionPacket->requested_user_uuid(), createdSessionInfo.m_iSessionID)) {
				Log::WriteLog(L"Client [%d] Create New Session Request Successful!, Session Name [%ls]", pCreateSessionPacket->requested_user_uuid(), MBToUni(pCreateSessionPacket->session_name()->c_str()).c_str());

				pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, ::CreateJoinSessionPacket(flatBuffer->m_flatBuffer, createdSessionInfo));
			}
			else {
				Log::WriteLog(L"Client [%d] Create New Session Request Failed!", pCreateSessionPacket->requested_user_uuid());

				pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateErrorPacket(flatBuffer->m_flatBuffer, "Already Logged In!", false));
			}
		}
	}
	return pNewTransmitQueueData;
}

FTransmitQueueData* CMessageProcessor::FindSessionProcessing(const FMessageData* const pMessageData) {
	using namespace SERVER::FUNCTIONS::UTIL;
	using namespace SERVER::FUNCTIONS::LOG;

	FTransmitQueueData* pNewTransmitQueueData = nullptr;
	if (auto pPacketStruct = std::static_pointer_cast<PACKET_STRUCT, void>(pMessageData->m_pMessage)) {
		if (auto pFindSessionPacket = FlatPacket::SessionPacket::GetFindSessionPacket(pPacketStruct->m_sPacketData)) {
			auto flatBuffer = std::make_unique<FFlatBuffer>();
			std::vector<FSessionStructure> cachedSessionList;
			int32_t iMaxPages = 0;

			m_sessionSystem.FindSession(pFindSessionPacket->page(), iMaxPages, cachedSessionList);

			std::vector<flatbuffers::Offset<FlatPacket::SessionPacket::session_information>> sessionList;
			sessionList.reserve(cachedSessionList.size());

			for (auto iterator : cachedSessionList)
				sessionList.push_back(FlatPacket::SessionPacket::Createsession_information(flatBuffer->m_flatBuffer, iterator.m_iSessionID, flatBuffer->m_flatBuffer.CreateString(iterator.m_sSessionName), iterator.m_joinedUserInformation.size(), iterator.m_iMaxSessionUsersCount));

			pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateFindSessionPacket(flatBuffer->m_flatBuffer, pFindSessionPacket->page(), iMaxPages, sessionList));
		}
	}
	return pNewTransmitQueueData;
}

FTransmitQueueData* CMessageProcessor::JoinSessionProcessing(const FMessageData* const pMessageData) {
	using namespace SERVER::FUNCTIONS::UTIL;
	using namespace SERVER::FUNCTIONS::LOG;

	FTransmitQueueData* pNewTransmitQueueData = nullptr;
	if (auto pPacketStruct = std::static_pointer_cast<PACKET_STRUCT, void>(pMessageData->m_pMessage)) {
		if (auto pJoinSessionRequestPacket = FlatPacket::SessionPacket::GetJoinSessionPacket(pPacketStruct->m_sPacketData)) {
			auto flatBuffer = std::make_unique<FFlatBuffer>();
			FSessionStructure joinedSessionInfo;
			FUserInformation loggedInUserInfo;

			if (m_userSystem.GetUserInformationByUUID(pJoinSessionRequestPacket->uuid(), loggedInUserInfo) &&
				m_sessionSystem.JoinSession(pJoinSessionRequestPacket->session_id(), FSessionUserInformation(pMessageData->m_pRequestedClientConnection, pJoinSessionRequestPacket->uuid(), loggedInUserInfo.m_sUserName, false), joinedSessionInfo) &&
				m_userSystem.SetParticipatingSessionID(pJoinSessionRequestPacket->uuid(), pJoinSessionRequestPacket->session_id())) {
				auto sendPacket = CreateJoinSessionPacket(flatBuffer->m_flatBuffer, joinedSessionInfo, pJoinSessionRequestPacket->session_id(), pJoinSessionRequestPacket->uuid());

				std::vector<void*> joinedSessionUsersConnection;
				m_sessionSystem.GetUserConnectionsBySessionID(pJoinSessionRequestPacket->session_id(), joinedSessionUsersConnection);

				for (const auto& iterator : joinedSessionUsersConnection)
					m_messageProcessedCallback(new FTransmitQueueData(static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(iterator), sendPacket));
			}
			else
				pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateErrorPacket(flatBuffer->m_flatBuffer, "Join Session Failure!", false));
		}
	}
	return pNewTransmitQueueData;
}

FTransmitQueueData* CMessageProcessor::UserSessionRequestProcessing(const FMessageData* const pMessageData) {
	using namespace SERVER::FUNCTIONS::UTIL;
	using namespace SERVER::FUNCTIONS::LOG;

	FTransmitQueueData* pNewTransmitQueueData = nullptr;
	if (auto pPacketStruct = std::static_pointer_cast<PACKET_STRUCT, void>(pMessageData->m_pMessage)) {
		if (auto pSessionRequestPacket = FlatPacket::SessionPacket::GetSessionUserRequestPacket(pPacketStruct->m_sPacketData)) {
			const int32_t iSessionID = pSessionRequestPacket->session_id();
			const int32_t iUUID = pSessionRequestPacket->uuid();

			auto flatBuffer = std::make_unique<FFlatBuffer>();
			auto sendPacket = CreateSessionRequestPacket(flatBuffer->m_flatBuffer, iSessionID, iUUID, pSessionRequestPacket->request(), pSessionRequestPacket->message()->c_str());
			bool bResult = false;

			switch (pSessionRequestPacket->request()) {
			case FlatPacket::SessionPacket::SessionRequestType::SessionRequestType_ESRT_Ready:
				bResult = m_sessionSystem.ChangeReadyState(iSessionID, iUUID, true);
				break;
			case FlatPacket::SessionPacket::SessionRequestType::SessionRequestType_ESRT_NotReady:
				bResult = m_sessionSystem.ChangeReadyState(iSessionID, iUUID, false);
				break;
			case FlatPacket::SessionPacket::SessionRequestType::SessionRequestType_ESRT_Quit:
				bResult = m_sessionSystem.QuitSession(iSessionID, iUUID) &&
						  m_userSystem.SetParticipatingSessionID(iUUID, 0);
				pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, sendPacket);
				break;
			case FlatPacket::SessionPacket::SessionRequestType::SessionRequestType_ESRT_Start:
				bResult = m_sessionSystem.CanStartGame(iSessionID, iUUID);
				if (!bResult) pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateErrorPacket(flatBuffer->m_flatBuffer, "Cannot Start Game!", false));
				else {
					int16_t iServerPortNumber;
					if (m_sessionSystem.CreateGameServer(iServerPortNumber)) {
						m_sessionSystem.BroadcastToSession(iSessionID, CreateGameStartPacket(flatBuffer->m_flatBuffer, "127.0.0.1", iServerPortNumber), m_messageProcessedCallback);
						return nullptr;
					}
				}
				break;
			case FlatPacket::SessionPacket::SessionRequestType::SessionRequestType_ESRT_Chat:
				bResult = true;
				break;
			}

			if (bResult)
				m_sessionSystem.BroadcastToSession(iSessionID, sendPacket, m_messageProcessedCallback);
		}
	}
	return pNewTransmitQueueData;
}


FTransmitQueueData* CMessageProcessor::ClientDisconnectProcessing(const FMessageData* const pMessageData) {
	using namespace SERVER::FUNCTIONS::LOG;
	if (pMessageData->m_pRequestedClientConnection) {
		const auto iUUID = m_userSystem.GetUUIDByConnection(pMessageData->m_pRequestedClientConnection);
		if (iUUID > 0) {
			const auto iSessionID = m_userSystem.GetParticipatingSessionIDByUUID(iUUID);
			if (iSessionID > 0) {
				auto flatBuffer = std::make_unique<FFlatBuffer>();

				if (m_sessionSystem.QuitSession(iSessionID, iUUID))
					m_sessionSystem.BroadcastToSession(iSessionID, CreateSessionRequestPacket(flatBuffer->m_flatBuffer, iSessionID, iUUID, FlatPacket::SessionPacket::SessionRequestType::SessionRequestType_ESRT_Quit), m_messageProcessedCallback);
			}
			if (m_userSystem.DeleteUser(iUUID))
				Log::WriteLog(L"Client [%d] Disconnected!", iUUID);
		}
	}
	return nullptr;
}



FTransmitQueueData* CMessageProcessor::DBSignInProcessed(const FMessageData* const  pMessageData) {
	using namespace SERVER::FUNCTIONS::UTIL;
	using namespace SERVER::FUNCTIONS::LOG;

	FTransmitQueueData* pNewTransmitQueueData = nullptr;
	if (auto pSignInDBResult = std::static_pointer_cast<FDBAccountRequest, void>(pMessageData->m_pMessage)) {
		auto flatBuffer = std::make_unique<FFlatBuffer>();

		if (pSignInDBResult->m_requestResult == FlatPacket::RequestMessageType::RequestMessageType_Succeeded) {
			if (m_userSystem.AddNewUser(pMessageData->m_pRequestedClientConnection, pSignInDBResult->m_iUUID, FUserInformation(pMessageData->m_pRequestedClientConnection, pSignInDBResult->m_sUserName))) {
				Log::WriteLog(L"Client [%ls], [%d] Sign In Request Successful!", MBToUni(pSignInDBResult->m_sUserName).c_str(), pSignInDBResult->m_iUUID);

				pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection
					, CreateAccountPacket(flatBuffer->m_flatBuffer, FlatPacket::PacketType::PacketType_Sign_In, FlatPacket::RequestMessageType::RequestMessageType_Succeeded
						, "", "", pSignInDBResult->m_sUserName, pSignInDBResult->m_iUUID, true));
			}
			else {
				Log::WriteLog(L"Client [%d] Sign In Request Failed!", pSignInDBResult->m_iUUID);
				pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateErrorPacket(flatBuffer->m_flatBuffer, "Already Logged In!", false));
			}
		}
		else {
			Log::WriteLog(L"Client [%d] Sign In Request Failed!", pSignInDBResult->m_iUUID);
			pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateErrorPacket(flatBuffer->m_flatBuffer, pSignInDBResult->m_sErrorMessage, false));
		}
	}
	return pNewTransmitQueueData;
}

FTransmitQueueData* CMessageProcessor::DBSignUpProcessed(const FMessageData* const  pMessageData) {
	using namespace SERVER::FUNCTIONS::UTIL;
	using namespace SERVER::FUNCTIONS::LOG;

	FTransmitQueueData* pNewTransmitQueueData = nullptr;
	if (auto pSignUpDBResult = std::static_pointer_cast<FDBAccountRequest, void>(pMessageData->m_pMessage)) {
		auto flatBuffer = std::make_unique<FFlatBuffer>();

		if (pSignUpDBResult->m_requestResult == FlatPacket::RequestMessageType::RequestMessageType_Succeeded) {
			if (m_userSystem.AddNewUser(pMessageData->m_pRequestedClientConnection, pSignUpDBResult->m_iUUID, FUserInformation(pMessageData->m_pRequestedClientConnection, pSignUpDBResult->m_sUserName))) {
				Log::WriteLog(L"Client [%ls], [%d] Sign Up Request Successful!", MBToUni(pSignUpDBResult->m_sUserName).c_str(), pSignUpDBResult->m_iUUID);

				pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection
					, CreateAccountPacket(flatBuffer->m_flatBuffer, FlatPacket::PacketType::PacketType_Sign_Up, FlatPacket::RequestMessageType::RequestMessageType_Succeeded
						, "", "", pSignUpDBResult->m_sUserName, pSignUpDBResult->m_iUUID, true));
			}
		}
		else {
			Log::WriteLog(L"Client [%d] Sign Up Request Failed!", pSignUpDBResult->m_iUUID);
			pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateErrorPacket(flatBuffer->m_flatBuffer, pSignUpDBResult->m_sErrorMessage, false));
		}
	}
	return pNewTransmitQueueData;
}

FTransmitQueueData* CMessageProcessor::DBUserInformationProcessed(const FMessageData* const pMessageData) {
	using namespace SERVER::FUNCTIONS::UTIL;
	using namespace SERVER::FUNCTIONS::LOG;

	FTransmitQueueData* pNewTransmitQueueData = nullptr;
	if (auto pUserInfoResult = std::static_pointer_cast<FDBUesrInformationRequest, void>(pMessageData->m_pMessage)) {
		auto flatBuffer = std::make_unique<FFlatBuffer>();

		if (pUserInfoResult->m_requestResult == FlatPacket::RequestMessageType::RequestMessageType_Succeeded) {
			m_userSystem.UpdateUserInventoryInformation(pUserInfoResult->m_iUUID, pUserInfoResult->m_userInventoryInformation);

			std::vector<flatbuffers::Offset<FlatPacket::item>> inventoryInfo;
			inventoryInfo.reserve(pUserInfoResult->m_userInventoryInformation.size());
			for (const auto& iterator : pUserInfoResult->m_userInventoryInformation)
				inventoryInfo.push_back(FlatPacket::Createitem(flatBuffer->m_flatBuffer, iterator.m_iItemID, iterator.m_iItemCount));

			pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, 
					CreateUserInformationPacket(flatBuffer->m_flatBuffer, FlatPacket::RequestMessageType::RequestMessageType_Succeeded, pUserInfoResult->m_iUUID, pUserInfoResult->m_iNumOfUnconfirmedMails, inventoryInfo));

			Log::WriteLog(L"Client [%d] Get User Information Request Successful!", pUserInfoResult->m_iUUID);
		}
		else {
			pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateErrorPacket(flatBuffer->m_flatBuffer, pUserInfoResult->m_sErrorMessage, true));

			Log::WriteLog(L"Client [%d] Get User Information Request Failed!", pUserInfoResult->m_iUUID);
		}
	}
	return pNewTransmitQueueData;
}


FTransmitQueueData* CMessageProcessor::DBAttendanceSheetsProcessed(const FMessageData* const pMessageData) {
	using namespace SERVER::FUNCTIONS::UTIL;
	using namespace SERVER::FUNCTIONS::LOG;

	FTransmitQueueData* pNewTransmitQueueData = nullptr;
	if (auto pAttendanceSheetsDBResult = std::static_pointer_cast<FDBAttendanceSheetsRequest, void>(pMessageData->m_pMessage)) {
		auto flatBuffer = std::make_unique<FFlatBuffer>();

		if (pAttendanceSheetsDBResult->m_requestResult == FlatPacket::RequestMessageType::RequestMessageType_Succeeded) {
			std::vector<flatbuffers::Offset<FlatPacket::AttendancePacket::AttendanceDetail>> attendanceDetails;
			m_attendanceSystem.GetAttendanceDetailByEventType(pAttendanceSheetsDBResult->m_iRequestedEventType, pAttendanceSheetsDBResult->m_userAttendanceInformations, flatBuffer->m_flatBuffer, attendanceDetails);

			m_userSystem.UpdateUserAttendanceInformation(pAttendanceSheetsDBResult->m_iUUID, pAttendanceSheetsDBResult->m_userAttendanceInformations);

			pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection
				, CreateAttendanceSheetsPacket(flatBuffer->m_flatBuffer, FlatPacket::RequestMessageType::RequestMessageType_Succeeded, pAttendanceSheetsDBResult->m_iUUID, pAttendanceSheetsDBResult->m_iRequestedEventType, attendanceDetails));

			Log::WriteLog(L"Client [%d] Attendance Information Request Was Successful!", pAttendanceSheetsDBResult->m_iUUID);
		}
		else {
			pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateErrorPacket(flatBuffer->m_flatBuffer, "Attendance Information Request Failed!", false));
			Log::WriteLog(L"Client [%d] Attendance Information Request Failed!", pAttendanceSheetsDBResult->m_iUUID);
		}
	}
	return pNewTransmitQueueData;
}

FTransmitQueueData* CMessageProcessor::DBAttendanceCheckProcessed(const FMessageData* const pMessageData) {
	using namespace FlatPacket;
	using namespace SERVER::FUNCTIONS::UTIL;

	FTransmitQueueData* pNewTransmitQueueData = nullptr;
	if (auto pAttendanceApplyDBResult = std::static_pointer_cast<FDBAttendanceApplyRequest, void>(pMessageData->m_pMessage)) {
		auto flatBuffer = std::make_unique<FFlatBuffer>();

		if (pAttendanceApplyDBResult->m_requestResult == RequestMessageType::RequestMessageType_Succeeded) {
			if (m_userSystem.ApplyAttendanceCheckResult(pAttendanceApplyDBResult->m_iUUID, pAttendanceApplyDBResult->m_iEventKey, pAttendanceApplyDBResult->m_updatedAttendanceDate, pAttendanceApplyDBResult->m_iUpdatedAttendanceCount, pAttendanceApplyDBResult->m_attendanceRewards)) {
				pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection,
					CreateAttendanceCheckPacket(flatBuffer->m_flatBuffer, RequestMessageType::RequestMessageType_Succeeded, pAttendanceApplyDBResult->m_attendanceType
						, pAttendanceApplyDBResult->m_iEventKey, pAttendanceApplyDBResult->m_iUUID
						, pAttendanceApplyDBResult->m_iUpdatedAttendanceCount));
			}
		}
		else
			pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateErrorPacket(flatBuffer->m_flatBuffer, pAttendanceApplyDBResult->m_sErrorMessage, false));
	}
	return pNewTransmitQueueData;
}

FTransmitQueueData* CMessageProcessor::DBGetMailsByPageProcessed(const FMessageData* const pMessageData) {
	using namespace FlatPacket;
	using namespace SERVER::FUNCTIONS::UTIL;

	FTransmitQueueData* pNewTransmitQueueData = nullptr;

	if (auto pGetMailsByPageDBResult = std::static_pointer_cast<FDBGetMailsRequest, void>(pMessageData->m_pMessage)) {
		auto flatBuffer = std::make_unique<FFlatBuffer>();

		if (pGetMailsByPageDBResult->m_requestResult == RequestMessageType::RequestMessageType_Succeeded) {
			std::vector<flatbuffers::Offset<MailPacket::mail>> receivedMails;
			receivedMails.reserve(pGetMailsByPageDBResult->m_mailInformations.size());

			for (const auto& iterator : pGetMailsByPageDBResult->m_mailInformations) {
				std::vector<flatbuffers::Offset<item>> attachedItems;
				attachedItems.reserve(iterator.m_attachedRewards.size());

				for (const auto& itemIterator : iterator.m_attachedRewards)
					attachedItems.push_back(Createitem(flatBuffer->m_flatBuffer, itemIterator.m_iItemID, itemIterator.m_iItemCount));

				receivedMails.emplace_back(MailPacket::CreatemailDirect(
					flatBuffer->m_flatBuffer, iterator.m_iMailID, iterator.m_sMailName.c_str(),
					::SQLTimeStampToSQLDateString(iterator.m_receivedDate).c_str(),
					iterator.m_sRemainTime.c_str(), &attachedItems, iterator.m_bIsCheck));
			}

			pNewTransmitQueueData = new FTransmitQueueData(
				pMessageData->m_pRequestedClientConnection,
				CreateMailsInfoPacket(
					flatBuffer->m_flatBuffer, RequestMessageType::RequestMessageType_Succeeded, pGetMailsByPageDBResult->m_iUUID,
					pGetMailsByPageDBResult->m_iRowCountLimit, pGetMailsByPageDBResult->m_iTotalMailCount, pGetMailsByPageDBResult->m_iPage, receivedMails));
		}
		else 
			pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateErrorPacket(flatBuffer->m_flatBuffer, "", false));
	}
	return pNewTransmitQueueData;
}

FTransmitQueueData* CMessageProcessor::DBConfirmMailByMailIDProcessed(const FMessageData* const pMessageData) {
	using namespace FlatPacket;
	using namespace SERVER::FUNCTIONS::UTIL;

	FTransmitQueueData* pNewTransmitQueueData = nullptr;

	if (auto pCheckRequestResult = std::static_pointer_cast<FDBConfirmMailRequest, void>(pMessageData->m_pMessage)) {
		auto flatBuffer = std::make_unique<FFlatBuffer>();

		if (pCheckRequestResult->m_requestResult == RequestMessageType::RequestMessageType_Succeeded)
			pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateConfirmMailPacket(flatBuffer->m_flatBuffer, RequestMessageType::RequestMessageType_Succeeded, pCheckRequestResult->m_iUUID, pCheckRequestResult->m_iMailID));
		else
			pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateErrorPacket(flatBuffer->m_flatBuffer, "Mail Verification Failed!", false));
	}
	return pNewTransmitQueueData;
}

FTransmitQueueData* CMessageProcessor::DBMailReceiveProcessed(const FMessageData* const pMessageData) {
	using namespace FlatPacket;
	using namespace SERVER::FUNCTIONS::UTIL;

	FTransmitQueueData* pNewTransmitQueueData = nullptr;

	if (auto pReceiveMailResult = std::static_pointer_cast<FDBReceiveMailsRequest, void>(pMessageData->m_pMessage)) {
		auto flatBuffer = std::make_unique<FFlatBuffer>();

		if (pReceiveMailResult->m_requestResult == RequestMessageType::RequestMessageType_Succeeded) {
			m_userSystem.AddItemToInventory(pReceiveMailResult->m_iUUID, pReceiveMailResult->m_receivedItems);

			std::vector<flatbuffers::Offset<item>> receivedRewards;
			receivedRewards.reserve(pReceiveMailResult->m_receivedItems.size());
			for (const auto& iterator : pReceiveMailResult->m_receivedItems)
				receivedRewards.push_back(Createitem(flatBuffer->m_flatBuffer, iterator.m_iItemID, iterator.m_iItemCount));

			pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateMailReceivePacket(flatBuffer->m_flatBuffer, RequestMessageType::RequestMessageType_Succeeded, pReceiveMailResult->m_iUUID, 0, false, receivedRewards));
		}
		else
			pNewTransmitQueueData = new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateErrorPacket(flatBuffer->m_flatBuffer, "", false));
	}
	return pNewTransmitQueueData;
}



FTransmitQueueData* CMessageProcessor::JsonDownloadStartProcessing(const FMessageData* const pMessageData) {
	using namespace FlatPacket;
	using namespace SERVER::FUNCTIONS::LOG;

	auto flatBuffer = std::make_unique<FFlatBuffer>();
	if (auto pPacketStruct = std::static_pointer_cast<PACKET_STRUCT, void>(pMessageData->m_pMessage)) {
		if (auto pJsonDownloadNotificationPacket = FlatPacket::JsonPacket::GetJsonDownloadNotifyPacket(pPacketStruct->m_sPacketData)) {
			Log::WriteLog(L"Json File Download Start! Compressed Size : [%d], Original Size : [%d]", pJsonDownloadNotificationPacket->compressed_json_length(), pJsonDownloadNotificationPacket->original_json_length());

			m_jsonDownloadSystem.DownloadPrepare(pJsonDownloadNotificationPacket->compressed_json_length(), pJsonDownloadNotificationPacket->original_json_length());
		}
	}
	return new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateJsonDownloadNotificationPacket(flatBuffer->m_flatBuffer, PacketType::PacketType_JsonDownloadStartPacket));
}

FTransmitQueueData* CMessageProcessor::JsonDataDownloadProcessing(const FMessageData* const pMessageData) {
	using namespace FlatPacket;
	using namespace SERVER::FUNCTIONS::LOG;

	auto flatBuffer = std::make_unique<FFlatBuffer>();
	if (auto pPacketStruct = std::static_pointer_cast<PACKET_STRUCT, void>(pMessageData->m_pMessage)) {
		if (auto pJsonDownloadPacket = FlatPacket::JsonPacket::GetJsonDownloadPacket(pPacketStruct->m_sPacketData)) {
			Log::WriteLog(L"Json File Downloading Start!");

			m_jsonDownloadSystem.Download(pJsonDownloadPacket->compressed_json_data());
		}
	}
	return new FTransmitQueueData(pMessageData->m_pRequestedClientConnection, CreateJsonDownloadNotificationPacket(flatBuffer->m_flatBuffer, PacketType::PacketType_JsonDownloadingPacket));
}