#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#include "../DBUtil/DBUtil.h"
#include <Functions/SQL/MSSQL/MSSQL.hpp>
#include <Functions/CriticalSection/CriticalSection.hpp>
#include <Functions/CircularQueue/CircularQueue.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

namespace MAINSERVER {
	class CDBBaseSystem {
		typedef SERVER::FUNCTIONS::CIRCULARQUEUE::CircularQueue<FDBBaseQueueData*, 200> DB_QUEUE_TYPE;
		typedef std::unordered_map<FlatPacket::PacketType, std::function<std::shared_ptr<void>(void*)>> DB_REQUEST_PROCESSOR;

		const std::chrono::milliseconds m_durationForDBThread;
		const std::function<void(struct FMessageData*)> m_dbRequestProcessedCallback;
	private:
		SERVER::FUNCTIONS::CRITICALSECTION::CriticalSection m_csForDBRequestQueue;
		std::unique_ptr<DB_QUEUE_TYPE> m_pDBRequestProcessingQueue;
		std::unique_ptr<DB_QUEUE_TYPE> m_pDBRequestStockQueue;

		std::condition_variable m_cvForDBWorkerThread;
		std::mutex m_csForDBWorkerThread;

		std::atomic_bool m_bDBThreadRunState;
		std::thread m_dbRequestProcessingThread;

	private:
		void DBWorkerThread();

	protected:
		DB_REQUEST_PROCESSOR m_requestProcessor;

		SERVER::FUNCTIONS::SQL::MSSQL::CMSSQLPool m_sqlPool;

	public:
		CDBBaseSystem(const std::string& sHostName, const std::string& sDBName, const std::string& sUserName, const std::string& sPassword, const uint16_t iMaxPoolConnection, const std::chrono::milliseconds& durationForDBThread, const std::function<void(struct FMessageData*)>& dbRequestProcessedCallback);
		~CDBBaseSystem();

	public:
		void AddNewDBRequest(FDBBaseQueueData* pNewQueueData);

	};

}