#pragma once
#pragma comment(lib, "Rpcrt4.lib")
#include <rpc.h>
#include <Functions/CriticalSection/CriticalSection.hpp>

namespace MAINSERVER {
	class CSystem {
	private:
		SERVER::FUNCTIONS::CRITICALSECTION::CriticalSection m_csUUIDGeneration;

	public:
		uint16_t GenerateUUID();

	};
}