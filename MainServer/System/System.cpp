#include "System.h"

uint16_t MAINSERVER::CSystem::GenerateUUID() {
	UUID userUUID;

	m_csUUIDGeneration.Lock();
	if (UuidCreate(&userUUID) != RPC_S_OK)
		return 0;

	RPC_STATUS result;
	auto iUUID = UuidHash(&userUUID, &result);
	m_csUUIDGeneration.UnLock();

	if (result != RPC_S_OK)
		return 0;
	return iUUID;
}
