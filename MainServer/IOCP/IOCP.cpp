#include "IOCP.h"
#include <json/json.h>
#include <fstream>

#define ATTENDANCE_EVENT_JSON_KEY "Attendance_Event_File_Path"
#define GAME_SERVER_EXE_PATH_JSON_KEY "Game_Server_Exe_File_Path"

using namespace MAINSERVER;

CIOCP::CIOCP(const std::string& sConfigJsonPath)
	: IOCP(m_packetProcessor, std::thread::hardware_concurrency()), m_durationForTransmissionThread(std::chrono::milliseconds(250))
	, m_messageProcessor(m_durationForTransmissionThread, std::bind(&CIOCP::AddNewTransmitData, this, std::placeholders::_1))
	, m_bIOCPRunState(true), m_bTransmissonThreadRunState(true), m_sConfigJsonPath(sConfigJsonPath) {


	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_Sign_In, std::bind(&CIOCP::SignInRequest, this, std::placeholders::_1));
	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_Sign_Up, std::bind(&CIOCP::SignUpRequest, this, std::placeholders::_1));

	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_User_Information, std::bind(&CIOCP::UserInformationRequest, this, std::placeholders::_1));

	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_Attendance_Sheets, std::bind(&CIOCP::AttendanceSheetsRequest, this, std::placeholders::_1));
	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_Attendance_Check, std::bind(&CIOCP::AttendanceCheckRequest, this, std::placeholders::_1));

	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_MailsInfo, std::bind(&CIOCP::GetMailsByPageRequest, this, std::placeholders::_1));
	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_MailConfirm, std::bind(&CIOCP::ConfirmMailByMailIDRequest, this, std::placeholders::_1));
	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_MailReceive, std::bind(&CIOCP::MailReceiveRequest, this, std::placeholders::_1));

	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_CreateSession, std::bind(&CIOCP::CreateSessionRequest, this, std::placeholders::_1));
	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_FindSession, std::bind(&CIOCP::FindSessionRequest, this, std::placeholders::_1));
	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_JoinSession, std::bind(&CIOCP::JoinSessionRequest, this, std::placeholders::_1));
	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_SessionRequest, std::bind(&CIOCP::UserSessionRequest, this, std::placeholders::_1));

	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_JsonDownloadStartPacket, std::bind(&CIOCP::JsonDownloadStartRequest, this, std::placeholders::_1));
	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_JsonDownloadingPacket, std::bind(&CIOCP::JsonDownloadingRequest, this, std::placeholders::_1));
	m_packetProcessor.emplace(FlatPacket::PacketType::PacketType_JsonDownloadEndPacket, std::bind(&CIOCP::JsonDownloadEndRequest, this, std::placeholders::_1));

	m_pTransmissionProcessingQueue = std::make_unique<TRANSMITQUEUE>();
	m_pTransmissionStockQueue = std::make_unique<TRANSMITQUEUE>();

	m_transmissionThread = std::thread(std::bind(&CIOCP::TransmissionWorkerThread, this));
}

bool CIOCP::Initialize(const EPROTOCOLTYPE protocolType, SocketAddress& bindAddress) {
	if (IOCP::Initialize(protocolType, bindAddress)) {
		Json::Value jsonConfig;
		Json::Reader jsonReader;
		std::ifstream configJsonFileStream(m_sConfigJsonPath, std::ios::in);
		if (configJsonFileStream.is_open() && jsonReader.parse(configJsonFileStream, jsonConfig))
			return m_messageProcessor.Initialize(jsonConfig[ATTENDANCE_EVENT_JSON_KEY].asString(), jsonConfig[GAME_SERVER_EXE_PATH_JSON_KEY].asString());
	}
	return false;
}

void CIOCP::Run() {
	IOCP::Run();

}

void CIOCP::Destroy() {
	m_bTransmissonThreadRunState = false;
	if (m_transmissionThread.joinable())
		m_transmissionThread.join();

	IOCP::Destroy();
}

SERVER::NETWORKMODEL::IOCP::CONNECTION* MAINSERVER::CIOCP::OnIOTryDisconnect(SERVER::NETWORK::USER_SESSION::USER_SERVER::User_Server* const pClient) {
	if (auto pConnection = IOCP::OnIOTryDisconnect(pClient))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_Disconnect, nullptr));
	return nullptr;
}

void CIOCP::AddNewTransmitData(FTransmitQueueData* pNewTransmitData) {
	if (pNewTransmitData) {
		SERVER::FUNCTIONS::CRITICALSECTION::CriticalSectionGuard lck(m_csForTransmissionQueue);
		m_pTransmissionStockQueue->Push(pNewTransmitData);
	}
}

void CIOCP::TransmissionWorkerThread() {
	while (m_bTransmissonThreadRunState) {
		std::unique_lock<std::mutex> lck(m_csForTransmissionThread);
		if (m_cvForTransmissionThread.wait_for(lck, m_durationForTransmissionThread, [&]() { return !m_pTransmissionProcessingQueue->IsEmpty(); })) {
			lck.unlock();

			FTransmitQueueData* pQueueData = nullptr;
			if (m_pTransmissionProcessingQueue->Pop(pQueueData)) {
				pQueueData->m_pClientConnection->m_pUser->Send(pQueueData->m_packetStructure);

				delete pQueueData;
			}
		}
		else if (m_pTransmissionProcessingQueue->IsEmpty() && !m_pTransmissionStockQueue->IsEmpty()) {
			SERVER::FUNCTIONS::CRITICALSECTION::CriticalSectionGuard lck(m_csForTransmissionQueue);
			m_pTransmissionProcessingQueue.swap(m_pTransmissionStockQueue);
		}
	}
}


void CIOCP::SignInRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_Sign_In, pPacketData->m_packetData));
}

void CIOCP::SignUpRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_Sign_Up, pPacketData->m_packetData));
}

void CIOCP::UserInformationRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_User_Information, pPacketData->m_packetData));
}


void CIOCP::AttendanceSheetsRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_Attendance_Sheets, pPacketData->m_packetData));
}

void CIOCP::AttendanceCheckRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_Attendance_Check, pPacketData->m_packetData));
}


void CIOCP::GetMailsByPageRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_MailsInfo, pPacketData->m_packetData));
}

void CIOCP::ConfirmMailByMailIDRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_MailConfirm, pPacketData->m_packetData));
}

void CIOCP::MailReceiveRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_MailReceive, pPacketData->m_packetData));
}

void CIOCP::CreateSessionRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_CreateSession, pPacketData->m_packetData));
}

void CIOCP::FindSessionRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_FindSession, pPacketData->m_packetData));
}

void CIOCP::JoinSessionRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_JoinSession, pPacketData->m_packetData));
}

void CIOCP::UserSessionRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_SessionRequest, pPacketData->m_packetData));
}



void CIOCP::JsonDownloadStartRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_JsonDownloadStartPacket, pPacketData->m_packetData));
}

void CIOCP::JsonDownloadingRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner))
		m_messageProcessor.AddNewMessage(new FMessageData(pConnection, FlatPacket::PacketType::PacketType_JsonDownloadingPacket, pPacketData->m_packetData));
}

void CIOCP::JsonDownloadEndRequest(PacketQueueData* const pPacketData) {
	if (auto pConnection = static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pPacketData->m_pOwner)) {
		SERVER::FUNCTIONS::LOG::Log::WriteLog(L"Json Download Is Complete! The Server Is Shutting Down!");
		m_bIOCPRunState = false;
	}
}