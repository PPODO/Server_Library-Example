#include "JsonDownloadSystem.h"
#include <Functions/Log/Log.hpp>
#include <json/json.h>
#include <memory>
#include "../../../Shared/AttendanceInformation/AttendanceInformation.h"

using namespace MAINSERVER;

CJsonDownloadSystem::CJsonDownloadSystem() : m_iCompressedJsonLength(), m_iOriginalJsonLength() {

}

void CJsonDownloadSystem::Initialize(const std::string& sJsonFilePath) {
	m_sJsonFilePath = sJsonFilePath;
}

void CJsonDownloadSystem::DownloadPrepare(const size_t iCompressedJsonLength, const size_t iOriginalJsonLength) {
	m_iCompressedJsonLength = iCompressedJsonLength;
	m_iOriginalJsonLength = iOriginalJsonLength;
}

void CJsonDownloadSystem::Download(const flatbuffers::Vector<uint8_t>* compressedJsonString) {
	using namespace SERVER::FUNCTIONS::LOG;

	if (m_iCompressedJsonLength > 0 && m_iOriginalJsonLength > 0) {
		auto sUncompressBuffer = new char[m_iOriginalJsonLength];
		ZeroMemory(sUncompressBuffer, m_iOriginalJsonLength);

		uncompress((Bytef*)sUncompressBuffer, (uLongf*)&m_iOriginalJsonLength, compressedJsonString->Data(), m_iCompressedJsonLength);

		if (WriteJsonFromBuffer(sUncompressBuffer))
			return;
	}
	Log::WriteLog(L"Json Downloading Failure!");
}

bool CJsonDownloadSystem::WriteJsonFromBuffer(const char* const sUncompressedBuffer) {
	Json::Value jsonRoot;
	Json::Reader jsonReader;

	m_sJsonFileWriteStream.open(m_sJsonFilePath, std::ios::out | std::ios::ate);
	if (m_sJsonFileWriteStream.is_open() && jsonReader.parse(sUncompressedBuffer, jsonRoot)) {
		Json::Value writeRoot;
		for (auto& iterator : jsonRoot) {
			auto newData = FAttendanceInfo::ReadFromJson(iterator);

			Json::Value writeItem;
			newData.WriteToJson(writeItem);

			writeRoot.append(writeItem);
		}

		Json::StyledWriter styledWriter;
		m_sJsonFileWriteStream << styledWriter.write(writeRoot);
		m_sJsonFileWriteStream.close();

		delete[] sUncompressedBuffer;
		return true;
	}
	delete[] sUncompressedBuffer;
	return false;
}