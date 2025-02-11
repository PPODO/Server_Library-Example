#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#include "../DBBaseSystem/DBBaseSystem.h"

namespace MAINSERVER {
	class CUserDBSystem : public CDBBaseSystem {
	public:
		CUserDBSystem(const std::string& sHostName, const std::string& sDBName, const std::string& sUserName, const std::string& sPassword, const uint16_t iMaxPoolConnection, const std::chrono::milliseconds& durationForDBThread, const std::function<void(struct FMessageData*)>& dbRequestProcessedCallback);

	private:
		std::shared_ptr<void> SignInRequest(void* const pRequestData);
		std::shared_ptr<void> SignUpRequest(void* const pRequestData);

	private:
		bool SignInErrorHandling(const SQLINTEGER iQueryReturnValue, FDBBaseQueueData* const pBaseQueueData);
		bool SignUpErrorHandling(const SQLINTEGER iQueryReturnValue, FDBBaseQueueData* const pBaseQueueData);

	};
}