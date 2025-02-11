#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TimerSystem.h"
#include <Functions/Log/Log.hpp>
#include "../../../Shared/Util/Util.h"

using namespace MAINSERVER;

CTimerSystem::CTimerSystem() : m_onTimeTimer(m_ioContext), m_threadRunState(true) {
	BindTimer();

	m_timerThread = std::thread([&]() { m_ioContext.run(); });
}

CTimerSystem::~CTimerSystem() {
	m_threadRunState = false;
	m_ioContext.stop();

	if (m_timerThread.joinable())
		m_timerThread.join();
}

void CTimerSystem::BindTimer() {
	const auto& cachedDate = GetCurrentDate();
	m_onTimeTimer.expires_from_now(boost::asio::chrono::seconds(((60 - cachedDate.wMinute) * 60) - (60 - cachedDate.wSecond)));
	m_onTimeTimer.async_wait(boost::bind(&CTimerSystem::OnTimeTimerCallback, this, boost::asio::placeholders::error));
}

void CTimerSystem::OnTimeTimerCallback(const boost::system::error_code& errorCode) {
	if (m_threadRunState && !errorCode.failed()) {
		m_csForOnTimeTimer.lock();
		for (const auto& iterator : m_onTimeTimerCallbacks)
			iterator();
		m_csForOnTimeTimer.unlock();

		BindTimer();
	}
}

void CTimerSystem::AddNewCallbackToOnTimeTimer(const std::function<void()>& newCallback) {
	std::unique_lock<std::mutex> lck(m_csForOnTimeTimer);
	m_onTimeTimerCallbacks.push_back(newCallback);
}