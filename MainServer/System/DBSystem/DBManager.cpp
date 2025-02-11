#include "DBManager.h"
#include "../../MessageProcessor/MessageProcessor.h"

#define DB_HOST_NAME "34.22.104.37"
#define DB_USERNAME "sqlserver"
#define DB_PWD "a2233212!"
#define MAX_SQL_POOL_CONNECTION 4

using namespace MAINSERVER;

CDBManager::CDBManager(const std::chrono::milliseconds& durationForDBThreads, const std::function<void(struct FMessageData*)>&& dbRequestProcessedCallback)
	: m_userDBSystem(DB_HOST_NAME, "UserDB", DB_USERNAME, DB_PWD, MAX_SQL_POOL_CONNECTION, durationForDBThreads, dbRequestProcessedCallback)
	, m_gameDBSystem(DB_HOST_NAME, "GameDB", DB_USERNAME, DB_PWD, MAX_SQL_POOL_CONNECTION, durationForDBThreads, dbRequestProcessedCallback) {
}

void CDBManager::AddNewDBRequestData(FDBBaseQueueData* pNewQueueData) {
	if (pNewQueueData) {
		switch (pNewQueueData->m_dbType) {
		case EDBType::EDT_UserDB:
			m_userDBSystem.AddNewDBRequest(pNewQueueData);
			break;
		case EDBType::EDT_GameDB:
			m_gameDBSystem.AddNewDBRequest(pNewQueueData);
			break;
		case EDBType::EDT_LogDB:

			break;
		}
	}
}