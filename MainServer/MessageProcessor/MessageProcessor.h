#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#include <NetworkModel/IOCP/IOCP.hpp>
#include <Functions/CircularQueue/CircularQueue.hpp>
#include "../System/DBSystem/DBManager.h"
#include "../System/UserSystem/UserSystem.h"
#include "../System/AttendanceSystem/AttendanceSystem.h"
#include "../System/TimerSystem/TimerSystem.h"
#include "../System/System.h"
#include "../System/JsonDownloadSystem/JsonDownloadSystem.h"
#include "../System/SessionSystem/SessionSystem.h"
#include "../../Shared/Util/Util.h"
#include <atomic>

namespace MAINSERVER {
	using namespace SERVER::NETWORK::PACKET;

	struct FMessageData : public SERVER::FUNCTIONS::CIRCULARQUEUE::QUEUEDATA::BaseData<FMessageData> {
	public:
		SERVER::NETWORKMODEL::IOCP::CONNECTION* const m_pRequestedClientConnection;
		uint8_t m_messageType;
		std::shared_ptr<void> m_pMessage;

	public:
		FMessageData() : m_pRequestedClientConnection(nullptr), m_messageType(), m_pMessage() {};
		FMessageData(SERVER::NETWORKMODEL::IOCP::CONNECTION* const pRequestedClientConnection, uint8_t messageType, std::shared_ptr<void> pMessage) : m_pRequestedClientConnection(pRequestedClientConnection), m_messageType(messageType), m_pMessage(pMessage) {};

	};

	struct FFlatBuffer : SERVER::FUNCTIONS::MEMORYMANAGER::CMemoryManager<FFlatBuffer> {
	public:
		flatbuffers::FlatBufferBuilder m_flatBuffer;

	public:
		FFlatBuffer() : m_flatBuffer(SERVER::NETWORK::PACKET::PACKET_STRUCT::BUFFER_LENGTH) {};

	};

	struct FTransmitQueueData;

	class CMessageProcessor {
		typedef SERVER::FUNCTIONS::CIRCULARQUEUE::CircularQueue<FMessageData*> MESSAGEQUEUE;
		typedef std::unordered_map<uint8_t, std::function<FTransmitQueueData*(const FMessageData* const)>> PROCESSOR;

		const std::chrono::milliseconds m_durationForProcessingThread;
		const std::function<void(FTransmitQueueData*)> m_messageProcessedCallback;
	private:
		PROCESSOR m_messageProcessor;

		CDBManager m_databaseManager;

		CAttendanceSystem m_attendanceSystem;

		CUserSystem m_userSystem;

		CSessionSystem m_sessionSystem;

		CTimerSystem m_timerSystem;

		CSystem m_systemInstance;

		CJsonDownloadSystem m_jsonDownloadSystem;

	private:
		SERVER::FUNCTIONS::CRITICALSECTION::CriticalSection m_csForMessageQueue;
		std::unique_ptr<MESSAGEQUEUE> m_pMessageProcessingQueue;
		std::unique_ptr<MESSAGEQUEUE> m_pMessageStockQueue;

		std::mutex m_csForProcessingThread;
		std::condition_variable m_cvForProcessingThread;
		std::atomic_bool m_bMessageProcessingThreadRunState;
		std::thread m_messageProcessingThread;

	private:
		void MessageProcessingWorkerThread();
		FTransmitQueueData* MessageProcessing(const FMessageData* const pMessageData);

	private:
		FTransmitQueueData* SignInProcessing(const FMessageData* const  pMessageData);
		FTransmitQueueData* SignUpProcessing(const FMessageData* const  pMessageData);

		FTransmitQueueData* UserInformationProcessing(const FMessageData* const pMessageData);

		FTransmitQueueData* AttendanceSheetsProcessing(const FMessageData* const  pMessageData);
		FTransmitQueueData* AttendanceCheckProcessing(const FMessageData* const  pMessageData);

		FTransmitQueueData* GetMailsByPageProcessing(const FMessageData* const pMessageData);
		FTransmitQueueData* ConfirmMailByMailIDProcessing(const FMessageData* const pMessageData);
		FTransmitQueueData* MailReceiveProcessing(const FMessageData* const pMessageData);

		FTransmitQueueData* CreateSessionProcessing(const FMessageData* const pMessageData);
		FTransmitQueueData* FindSessionProcessing(const FMessageData* const pMessageData);
		FTransmitQueueData* JoinSessionProcessing(const FMessageData* const pMessageData);
		FTransmitQueueData* UserSessionRequestProcessing(const FMessageData* const pMessageData);

		FTransmitQueueData* ClientDisconnectProcessing(const FMessageData* const pMessageData);

		FTransmitQueueData* DBSignInProcessed(const FMessageData* const  pMessageData);
		FTransmitQueueData* DBSignUpProcessed(const FMessageData* const  pMessageData);

		FTransmitQueueData* DBUserInformationProcessed(const FMessageData* const pMessageData);

		FTransmitQueueData* DBAttendanceSheetsProcessed(const FMessageData* const  pMessageData);
		FTransmitQueueData* DBAttendanceCheckProcessed(const FMessageData* const  pMessageData);

		FTransmitQueueData* DBGetMailsByPageProcessed(const FMessageData* const pMessageData);
		FTransmitQueueData* DBConfirmMailByMailIDProcessed(const FMessageData* const pMessageData);
		FTransmitQueueData* DBMailReceiveProcessed(const FMessageData* const pMessageData);


		FTransmitQueueData* JsonDownloadStartProcessing(const FMessageData* const pMessageData);
		FTransmitQueueData* JsonDataDownloadProcessing(const FMessageData* const pMessageData);

	public:
		CMessageProcessor(const std::chrono::milliseconds& durationForProcessingThread, const std::function<void(FTransmitQueueData*)>& messageProcessedCallback);
		~CMessageProcessor();

	public:
		bool Initialize(const std::string& sAttendanceJsonFilePath, const std::string& sGameServerExeFilePath);

	public:
		void AddNewMessage(FMessageData* pNewMessageData);

	};
}