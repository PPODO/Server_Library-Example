#include "DBBaseSystem.h"
#include "../../../MessageProcessor/MessageProcessor.h"

using namespace MAINSERVER;

CDBBaseSystem::CDBBaseSystem(const std::string& sHostName, const std::string& sDBName, const std::string& sUserName, const std::string& sPassword, const uint16_t iMaxPoolConnection, const std::chrono::milliseconds& durationForDBThread, const std::function<void(FMessageData*)>& dbRequestProcessedCallback)
	: m_dbRequestProcessedCallback(dbRequestProcessedCallback), m_durationForDBThread(durationForDBThread)
	, m_sqlPool(sHostName, sDBName, sUserName, sPassword, iMaxPoolConnection), m_bDBThreadRunState(true) {

	m_pDBRequestProcessingQueue = std::make_unique<DB_QUEUE_TYPE>();
	m_pDBRequestStockQueue = std::make_unique<DB_QUEUE_TYPE>();

	m_dbRequestProcessingThread = std::thread(std::bind(&CDBBaseSystem::DBWorkerThread, this));
}

CDBBaseSystem::~CDBBaseSystem() {
	m_bDBThreadRunState = false;
	if (m_dbRequestProcessingThread.joinable())
		m_dbRequestProcessingThread.join();
}

void MAINSERVER::CDBBaseSystem::AddNewDBRequest(FDBBaseQueueData* pNewQueueData) {
	SERVER::FUNCTIONS::CRITICALSECTION::CriticalSectionGuard lck(m_csForDBRequestQueue);
	m_pDBRequestStockQueue->Push(pNewQueueData);
}

void MAINSERVER::CDBBaseSystem::DBWorkerThread() {
	using namespace SERVER::NETWORKMODEL::IOCP;

	while (m_bDBThreadRunState) {
		std::unique_lock<std::mutex> lck(m_csForDBWorkerThread);
		if (m_cvForDBWorkerThread.wait_for(lck, m_durationForDBThread, [&]() { return !m_pDBRequestProcessingQueue->IsEmpty(); })) {
			lck.unlock();

			FDBBaseQueueData* pQueueData = nullptr;
			if (m_pDBRequestProcessingQueue->Pop(pQueueData)) {
				const auto& findResult = m_requestProcessor.find(pQueueData->m_packetType);
				if (findResult != m_requestProcessor.cend())
					m_dbRequestProcessedCallback(new FMessageData(static_cast<CONNECTION*>(pQueueData->m_pRequestedClientConnection), pQueueData->m_packetType, findResult->second(pQueueData)));
			}
		}
		else if(m_pDBRequestProcessingQueue->IsEmpty() && !m_pDBRequestStockQueue->IsEmpty()) {
			SERVER::FUNCTIONS::CRITICALSECTION::CriticalSectionGuard lck(m_csForDBRequestQueue);
			m_pDBRequestProcessingQueue.swap(m_pDBRequestStockQueue);
		}
	}
}