#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#include "UserDBSystem/UserDBSystem.h"
#include "GameDBSystem/GameDBSystem.h"

namespace MAINSERVER {
	class CDBManager {
	private:
		CUserDBSystem m_userDBSystem;
		CGameDBSystem m_gameDBSystem;

	public:
		CDBManager(const std::chrono::milliseconds& durationForDBThreads, const std::function<void(struct FMessageData*)>&& dbRequestProcessedCallback);

	public:
		void AddNewDBRequestData(FDBBaseQueueData* pNewQueueData);

	};
}