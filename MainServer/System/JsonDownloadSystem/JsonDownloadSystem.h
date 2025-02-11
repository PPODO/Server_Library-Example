#pragma once
#define NOMINMAX
#define _WINSOCKAPI_
#include <fstream>
#include <zlib.h>
#include <flatbuffers/vector.h>

namespace MAINSERVER {
	class CJsonDownloadSystem {
	private:
		std::fstream m_sJsonFileWriteStream;
		std::string m_sJsonFilePath;

		size_t m_iCompressedJsonLength;
		size_t m_iOriginalJsonLength;

	private:
		bool WriteJsonFromBuffer(const char* const sUncompressedBuffer);

	public:
		CJsonDownloadSystem();

		void Initialize(const std::string& sJsonFilePath);

	public:
		void DownloadPrepare(const size_t iCompressedJsonLength, const size_t iOriginalJsonLength);
		void Download(const flatbuffers::Vector<uint8_t>* compressedJsonString);

	};
}