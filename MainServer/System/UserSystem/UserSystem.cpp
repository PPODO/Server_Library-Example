#include "UserSystem.h"

using namespace MAINSERVER;

CUserSystem::CUserSystem() {

}

void CUserSystem::AddItemToInventory(FUserInformation& userInfo, const std::vector<FItemInformation>& itemInformation) {
	for (const auto& itemIterator : itemInformation) {
		const auto findResult = std::find_if(userInfo.m_inventoryInformation.begin(), userInfo.m_inventoryInformation.end(), [&](const FItemInformation& val) {
			return val.m_iItemID == itemIterator.m_iItemID;
		});

		if (findResult != userInfo.m_inventoryInformation.cend())
			findResult->m_iItemCount += itemIterator.m_iItemCount;
		else
			userInfo.m_inventoryInformation.push_back(itemIterator);
	}
}

bool CUserSystem::AddNewUser(SERVER::NETWORKMODEL::IOCP::CONNECTION* const pConnection, const int32_t iUUID, const FUserInformation& userInfo) {
	const auto& findResult = m_loggedInUserInformation.find(iUUID);
	if (findResult == m_loggedInUserInformation.cend()) {
		m_connectionInformation.emplace(pConnection, iUUID);
		m_loggedInUserInformation.emplace(iUUID, userInfo);
		return true;
	}
	return false;
}

bool CUserSystem::DeleteUser(const int32_t iUUID) {
	const auto& findResult = m_loggedInUserInformation.find(iUUID);
	if (findResult != m_loggedInUserInformation.cend()) {
		const auto& connectionFindResult = m_connectionInformation.find(findResult->second.m_pConnection);
		if (connectionFindResult != m_connectionInformation.cend())
			m_connectionInformation.erase(connectionFindResult);

		m_loggedInUserInformation.erase(findResult);
		return true;
	}
	return false;
}

void CUserSystem::UpdateUserAttendanceInformation(const int32_t iUUID, const std::vector<FUserAttendanceInformation>& attendanceInfo) {
	const auto& findResult = m_loggedInUserInformation.find(iUUID);
	if (findResult != m_loggedInUserInformation.cend())
		findResult->second.UpdateAttendanceInformation(attendanceInfo);
}

void CUserSystem::UpdateUserInventoryInformation(const int32_t iUUID, const std::vector<FItemInformation>& itemInformation) {
	const auto& findResult = m_loggedInUserInformation.find(iUUID);
	if (findResult != m_loggedInUserInformation.cend())
		findResult->second.m_inventoryInformation = itemInformation;
}

bool CUserSystem::ApplyAttendanceCheckResult(const int32_t iUUID, const uint32_t iEventKey, const SQL_DATE_STRUCT& updatedAttendanceDate, const int16_t iUpdatedAttendanceCount, const std::vector<FItemInformation>& itemInformation) {
	const auto& findResult = m_loggedInUserInformation.find(iUUID);
	if (findResult != m_loggedInUserInformation.cend()) {
		AddItemToInventory(findResult->second, itemInformation);

		const auto& attendanceFindResult = std::find_if(findResult->second.m_attendanceInformation.begin(), findResult->second.m_attendanceInformation.end(), [&](const FUserAttendanceInformation& value) {
			return value.m_iEventKey == iEventKey;
		});
		if (attendanceFindResult != findResult->second.m_attendanceInformation.cend()) {
			attendanceFindResult->m_iAttendanceCount = iUpdatedAttendanceCount;
			attendanceFindResult->m_lastAttendanceDate = updatedAttendanceDate;
		}
		else
			findResult->second.m_attendanceInformation.emplace_back(iEventKey, updatedAttendanceDate, iUpdatedAttendanceCount);

		return true;
	}
	return false;
}

bool CUserSystem::AddItemToInventory(const int32_t iUUID, const std::vector<FItemInformation>& itemInformation) {
	const auto& findResult = m_loggedInUserInformation.find(iUUID);
	if (findResult != m_loggedInUserInformation.cend()) {
		AddItemToInventory(findResult->second, itemInformation);
		return true;
	}
	return false;
}

bool CUserSystem::CalculateCostRequest(const int32_t iUUID, const int32_t iItemID, const int32_t iItemCount) {
	const auto& findResult = m_loggedInUserInformation.find(iUUID);
	if (findResult != m_loggedInUserInformation.cend()) {
		if (iItemID > 0 && iItemCount > 0) {
			const auto& itemInformation = std::find_if(findResult->second.m_inventoryInformation.begin(), findResult->second.m_inventoryInformation.end(), [&](const FItemInformation& itemInfo) {
				return itemInfo.m_iItemID == iItemID;
			});
			if (itemInformation != findResult->second.m_inventoryInformation.cend() &&
				itemInformation->m_iItemCount >= iItemCount) {
				itemInformation->m_iItemCount -= iItemCount;
				return true;
			}
			else
				return false;
		}
		return true;
	}
	return false;
}

bool CUserSystem::SetParticipatingSessionID(const int32_t iUUID, const int32_t iSessionID) {
	const auto& findResult = m_loggedInUserInformation.find(iUUID);
	if (findResult != m_loggedInUserInformation.cend()) {
		findResult->second.iSessionIDParticipated = iSessionID;
		return true;
	}
	return false;
}

bool CUserSystem::GetUserAttendanceInformationByUUIDAndEventKey(const int32_t iUUID, const uint32_t iEventKey, FUserAttendanceInformation& outputResult) {
	const auto& findResult = m_loggedInUserInformation.find(iUUID);
	if (findResult != m_loggedInUserInformation.cend()) {
		const auto& attendanceFindResult = std::find_if(findResult->second.m_attendanceInformation.begin(), findResult->second.m_attendanceInformation.end(), [&](const FUserAttendanceInformation& value) {
			return value.m_iEventKey == iEventKey;
		});
		if (attendanceFindResult != findResult->second.m_attendanceInformation.cend()) {
			outputResult = *attendanceFindResult;
			return true;
		}
	}
	return false;
}

bool CUserSystem::GetUserInformationByUUID(const int32_t iUUID, FUserInformation& outputUserInfo) const {
	const auto& findResult = m_loggedInUserInformation.find(iUUID);
	if (findResult != m_loggedInUserInformation.cend()) {
		outputUserInfo = findResult->second;
		return true;
	}
	return false;
}

int32_t CUserSystem::GetUUIDByConnection(SERVER::NETWORKMODEL::IOCP::CONNECTION* const pConnection) {
	const auto& findResult =  m_connectionInformation.find(pConnection);
	if (findResult != m_connectionInformation.cend())
		return findResult->second;

	return -1;
}

int32_t CUserSystem::GetParticipatingSessionIDByUUID(const int32_t iUUID) const {
	const auto& findResult = m_loggedInUserInformation.find(iUUID);
	if (findResult != m_loggedInUserInformation.cend()) 
		return findResult->second.iSessionIDParticipated;

	return -1;
}
