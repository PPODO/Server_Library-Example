#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#include <NetworkModel/IOCP/IOCP.hpp>
#include <Functions/CircularQueue/CircularQueue.hpp>
#include "../System/InGameSystem/InGameSystem.h"

namespace GAMESERVER {
	using namespace SERVER::NETWORKMODEL::BASEMODEL;
	using namespace SERVER::NETWORKMODEL::IOCP;
	using namespace SERVER::NETWORK::PACKET;

	struct FTransmitQueueData : public SERVER::FUNCTIONS::CIRCULARQUEUE::QUEUEDATA::BaseData<FTransmitQueueData, 400> {
	public:
		SERVER::NETWORK::PROTOCOL::UDP::PeerInfo m_peerInformation;
		PACKET_STRUCT m_packetStructure;
		bool m_bReliable;

	public:
		FTransmitQueueData() : m_peerInformation(), m_packetStructure(), m_bReliable(false) {};
		FTransmitQueueData(const SERVER::NETWORK::PROTOCOL::UDP::PeerInfo& peerInformation, const PACKET_STRUCT& packetStructure, const bool bReliable) : m_peerInformation(peerInformation), m_packetStructure(packetStructure), m_bReliable(bReliable) {};

	};

	class CIOCP : public IOCP {
		typedef SERVER::FUNCTIONS::CIRCULARQUEUE::CircularQueue<FTransmitQueueData*> TRANSMITQUEUE;

		const std::chrono::milliseconds m_durationForTransmissionThread;
	private:
		PACKETPROCESSOR m_packetProcessor;

		std::atomic_bool m_bIOCPRunState;

	private:
		CInGameSystem m_inGameSystem;

	private:
		SERVER::FUNCTIONS::CRITICALSECTION::CriticalSection m_csForTransmissionQueue;

		std::unique_ptr<TRANSMITQUEUE> m_pTransmissionProcessingQueue;
		std::unique_ptr<TRANSMITQUEUE> m_pTransmissionStockQueue;

		std::mutex m_csForTransmissionThread;
		std::condition_variable m_cvForTransmissionThread;

		std::atomic_bool m_bTransmissonThreadRunState;
		std::thread m_transmissionThread;

	private:
		void TransmissionWorkerThread();

	private:
		void PlayerJoinPacketProcessing(SERVER::NETWORK::PACKET::PacketQueueData* const pPacketData);
		void PlayerReadyPakcetProcessing(SERVER::NETWORK::PACKET::PacketQueueData* const pPacketData);
		void PlayerGameDataProcessing(SERVER::NETWORK::PACKET::PacketQueueData* const pPacketData);

	public:
		CIOCP();

	public:
		virtual bool Initialize(const EPROTOCOLTYPE protocolType, SERVER::FUNCTIONS::SOCKETADDRESS::SocketAddress& bindAddress) override final;
		virtual void Run() override final;
		virtual void Destroy() override final;

	public:
		void AddNewTransmitData(FTransmitQueueData* pNewTransmitData);

	public:
		bool GetIOCPRunState() const { return m_bIOCPRunState; }

	};
}