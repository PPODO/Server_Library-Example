#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#include <NetworkModel/IOCP/IOCP.hpp>
#include <Functions/CircularQueue/CircularQueue.hpp>
#include <atomic>
#include "../MessageProcessor/MessageProcessor.h"

namespace MAINSERVER {
	using namespace SERVER::NETWORKMODEL::BASEMODEL;
	using namespace SERVER::NETWORKMODEL::IOCP;
	using namespace SERVER::NETWORK::PACKET;

	struct FTransmitQueueData : public SERVER::FUNCTIONS::CIRCULARQUEUE::QUEUEDATA::BaseData<FTransmitQueueData, 400> {
	public:
		SERVER::NETWORKMODEL::IOCP::CONNECTION* m_pClientConnection;
		PACKET_STRUCT m_packetStructure;

	public:
		FTransmitQueueData() : m_pClientConnection(nullptr), m_packetStructure() {};
		FTransmitQueueData(SERVER::NETWORKMODEL::IOCP::CONNECTION* pConnectedUser, const PACKET_STRUCT& packetStructure) : m_pClientConnection(pConnectedUser), m_packetStructure(packetStructure) {};

	};

	class CIOCP : public IOCP {
		typedef SERVER::FUNCTIONS::CIRCULARQUEUE::CircularQueue<FTransmitQueueData*> TRANSMITQUEUE;

		const std::chrono::milliseconds m_durationForTransmissionThread;
		const std::string m_sConfigJsonPath;
	private:
		PACKETPROCESSOR m_packetProcessor;

		std::atomic_bool m_bIOCPRunState;

		CMessageProcessor m_messageProcessor;

	private: // transmission
		SERVER::FUNCTIONS::CRITICALSECTION::CriticalSection m_csForTransmissionQueue;

		std::unique_ptr<TRANSMITQUEUE> m_pTransmissionProcessingQueue;
		std::unique_ptr<TRANSMITQUEUE> m_pTransmissionStockQueue;

		std::mutex m_csForTransmissionThread;
		std::condition_variable m_cvForTransmissionThread;

		std::atomic_bool m_bTransmissonThreadRunState;
		std::thread m_transmissionThread;

	private:
		void TransmissionWorkerThread();

		void SignInRequest(PacketQueueData* const pPacketData);
		void SignUpRequest(PacketQueueData* const pPacketData);

		void UserInformationRequest(PacketQueueData* const pPacketData);

		void AttendanceSheetsRequest(PacketQueueData* const pPacketData);
		void AttendanceCheckRequest(PacketQueueData* const pPacketData);

		void GetMailsByPageRequest(PacketQueueData* const pPacketData);
		void ConfirmMailByMailIDRequest(PacketQueueData* const pPacketData);
		void MailReceiveRequest(PacketQueueData* const pPacketData);

		void CreateSessionRequest(PacketQueueData* const pPacketData);
		void FindSessionRequest(PacketQueueData* const pPacketData);
		void JoinSessionRequest(PacketQueueData* const pPacketData);
		void UserSessionRequest(PacketQueueData* const pPacketData);


		void JsonDownloadStartRequest(PacketQueueData* const pPacketData);
		void JsonDownloadingRequest(PacketQueueData* const pPacketData);
		void JsonDownloadEndRequest(PacketQueueData* const pPacketData);

	public:
		CIOCP(const std::string& sConfigJsonPath);

	public:
		virtual bool Initialize(const EPROTOCOLTYPE protocolType, SocketAddress& bindAddress) override final;
		virtual void Run() override final;
		virtual void Destroy() override final;

		virtual SERVER::NETWORKMODEL::IOCP::CONNECTION* OnIOTryDisconnect(SERVER::NETWORK::USER_SESSION::USER_SERVER::User_Server* const pClient) override;

	public:
		void AddNewTransmitData(FTransmitQueueData* pNewTransmitData);

	public:
		__forceinline bool GetIOCPRunState() const { return m_bIOCPRunState; }

	};
}