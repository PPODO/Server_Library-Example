#include "SessionSystem.h"
#include "../../IOCP/IOCP.h"

using namespace MAINSERVER;

uint16_t CSessionSystem::GenerateSessionID() {
	UUID sessionID;

	if (UuidCreate(&sessionID) != RPC_S_OK)
		return 0;

	RPC_STATUS result;
	auto ID = UuidHash(&sessionID, &result);

	if (result != RPC_S_OK)
		return 0;
	return ID;
}

bool CSessionSystem::Initialize(const std::string& sGameServerExeFilePath) {
	m_sGameServerExeFilePath = sGameServerExeFilePath;

	return true;
}

bool CSessionSystem::CreateNewSession(const std::string& sSessionName, const FSessionUserInformation& sessionUserInfo, const int32_t iMaxUserCount, FSessionStructure& outputSessionInformation) {
	const auto iSessionID = GenerateSessionID();
 	const auto& sessionIterator = std::find_if(m_sessionInformation.begin(), m_sessionInformation.end(), [&](const FSessionStructure& val) {
		return val.m_iSessionID == iSessionID;
		});

	if (sessionIterator == m_sessionInformation.cend()) {
		FSessionStructure newSessionInformation(iSessionID, sSessionName, iMaxUserCount);
		newSessionInformation.m_joinedUserInformation.push_back(sessionUserInfo);

		m_sessionInformation.push_back(newSessionInformation);
		outputSessionInformation = newSessionInformation;
		return true;
	}
	return false;
}

bool CSessionSystem::JoinSession(int32_t iSessionID, const FSessionUserInformation& sessionUserInfo, FSessionStructure& outputSessionInformation) {
	const auto& sessionFindResult = std::find_if(m_sessionInformation.begin(), m_sessionInformation.end(), [&](const FSessionStructure& val) {
		return val.m_iSessionID == iSessionID;
	});

	if (sessionFindResult == m_sessionInformation.cend())
		return false;

	sessionFindResult->m_joinedUserInformation.push_back(sessionUserInfo);
	outputSessionInformation = *sessionFindResult;

	return true;
}

void CSessionSystem::FindSession(const int32_t iRequestedPage, int32_t& iMaxPages, std::vector<FSessionStructure>& outputSessionLists) {
	const int32_t iStartIndex = (iRequestedPage - 1) * NUM_OF_ONE_PAGE_SESSION_LISTINGS;
	const int32_t iWantedEndIndex = (iRequestedPage * NUM_OF_ONE_PAGE_SESSION_LISTINGS);
	const size_t iSessionsCount = m_sessionInformation.size();
	const int32_t iEndIndex = iSessionsCount < iWantedEndIndex ? iSessionsCount : iWantedEndIndex;

	outputSessionLists.reserve(iWantedEndIndex - iStartIndex);
	for (int32_t i = iStartIndex; i < iEndIndex; i++)
		outputSessionLists.push_back(m_sessionInformation.at(i));

	iMaxPages = (iSessionsCount / NUM_OF_ONE_PAGE_SESSION_LISTINGS) + (iSessionsCount % NUM_OF_ONE_PAGE_SESSION_LISTINGS);
}

bool CSessionSystem::QuitSession(const int32_t iSessionID, const int32_t iUUID) {
	if (iSessionID != 0) {
		const auto& sessionFindResult = std::find_if(m_sessionInformation.begin(), m_sessionInformation.end(), [&](const FSessionStructure& val) {
			return val.m_iSessionID == iSessionID;
		});

		if (sessionFindResult == m_sessionInformation.cend())
			return false;

		const auto& userFindResult = std::find_if(sessionFindResult->m_joinedUserInformation.begin(), sessionFindResult->m_joinedUserInformation.end(), [&](const FSessionUserInformation& sessionUserInfo) {
			return sessionUserInfo.m_iUUID == iUUID;
		});

		if (userFindResult == sessionFindResult->m_joinedUserInformation.cend())
			return false;

		sessionFindResult->m_joinedUserInformation.erase(userFindResult);

		if (sessionFindResult->m_joinedUserInformation.size() <= 0)
			m_sessionInformation.erase(sessionFindResult);
	}
	else {
		for (auto iterator = m_sessionInformation.begin(); iterator != m_sessionInformation.cend(); ++iterator) {
			const auto& userFindResult = std::find_if(iterator->m_joinedUserInformation.begin(), iterator->m_joinedUserInformation.end(), [&](const FSessionUserInformation& val) {
				return iUUID == val.m_iUUID;
			});

			if (userFindResult != iterator->m_joinedUserInformation.cend()) {
				iterator->m_joinedUserInformation.erase(userFindResult);
				
				if (iterator->m_joinedUserInformation.size() <= 0)
					m_sessionInformation.erase(iterator);
				
				break;
			}
		}
	}
	return true;
}

bool CSessionSystem::ChangeReadyState(const int32_t iSessionID, const int32_t iUUID, const bool bNewState) {
	const auto& sessionFindResult = std::find_if(m_sessionInformation.begin(), m_sessionInformation.end(), [&](const FSessionStructure& val) {
		return val.m_iSessionID == iSessionID;
	});
	if (sessionFindResult == m_sessionInformation.cend())
		return false;

	const auto& userFindResult = std::find_if(sessionFindResult->m_joinedUserInformation.begin(), sessionFindResult->m_joinedUserInformation.end(), [&](const FSessionUserInformation& val) {
		return val.m_iUUID == iUUID;
	});
	if (userFindResult == sessionFindResult->m_joinedUserInformation.cend())
		return false;

	userFindResult->m_bReadyState = bNewState;
	return true;
}

bool CSessionSystem::CanStartGame(const int32_t iSessionID, const int32_t iUUID) {
	const auto& sessionFindResult = std::find_if(m_sessionInformation.begin(), m_sessionInformation.end(), [&](const FSessionStructure& val) {
		return val.m_iSessionID == iSessionID;
	});
	if (sessionFindResult == m_sessionInformation.cend())
		return false;

	const auto& userFindResult = std::find_if(sessionFindResult->m_joinedUserInformation.begin(), sessionFindResult->m_joinedUserInformation.end(), [&](const FSessionUserInformation& val) {
		return val.m_iUUID == iUUID;
	});
	if (userFindResult == sessionFindResult->m_joinedUserInformation.cend() ||
		!userFindResult->m_bIsHost)
		return false;

	for (const auto& userIterator : sessionFindResult->m_joinedUserInformation)
		if (!userIterator.m_bReadyState) return false;

	return true;
}

void CSessionSystem::BroadcastToSession(const int32_t iSessionID, const SERVER::NETWORK::PACKET::PACKET_STRUCT& sendPacket, const std::function<void(FTransmitQueueData*)>& callbackFunc) {
	const auto& findResult = std::find_if(m_sessionInformation.begin(), m_sessionInformation.end(), [&](const FSessionStructure& val) {
		return val.m_iSessionID == iSessionID;
		});
	if (findResult == m_sessionInformation.cend())
		return;

	for (const auto& iterator : findResult->m_joinedUserInformation)
		callbackFunc(new FTransmitQueueData(static_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(iterator.m_pIOCPConnection), sendPacket));
}

bool CSessionSystem::CreateGameServer(int16_t& iOutputServerPortNumber) {
	STARTUPINFOA startInfo = { sizeof(STARTUPINFOA) };
	PROCESS_INFORMATION processInformation;

	if (CreateProcessA(m_sGameServerExeFilePath.c_str(), const_cast<char* const>(("127.0.0.1" + std::to_string(3560)).c_str()), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startInfo, &processInformation)) {
		iOutputServerPortNumber = 3560; // todo : change
		return true;
	}
	return false;
}

void CSessionSystem::GetUserConnectionsBySessionID(const int32_t iSessionID, std::vector<void*>& joinedUserIOCPConnections) {
	const auto& sessionFindResult = std::find_if(m_sessionInformation.begin(), m_sessionInformation.end(), [&](const FSessionStructure& val) {
		return val.m_iSessionID == iSessionID;
	});
	if (sessionFindResult == m_sessionInformation.cend())
		return;

	joinedUserIOCPConnections.reserve(sessionFindResult->m_joinedUserInformation.size());
	for (const auto& iterator : sessionFindResult->m_joinedUserInformation)
		joinedUserIOCPConnections.push_back(iterator.m_pIOCPConnection);
}

