#pragma once
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/bind/bind.hpp>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>

namespace MAINSERVER {
	class CTimerSystem {
	private:
		boost::asio::io_context m_ioContext;
		boost::asio::steady_timer m_onTimeTimer;

		std::atomic_bool m_threadRunState;
		std::thread m_timerThread;

		std::mutex m_csForOnTimeTimer;
		std::vector<std::function<void()>> m_onTimeTimerCallbacks;

	private:
		void BindTimer();

		void OnTimeTimerCallback(const boost::system::error_code& errorCode);

	public:
		CTimerSystem();
		~CTimerSystem();

	public:
		void AddNewCallbackToOnTimeTimer(const std::function<void()>& newCallback);

	};
}