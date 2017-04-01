#pragma once

#include <stdint.h>
#include "../Serialization/IByteBuffer.h"
#include <vector>
#include <numeric>

namespace ProjectC {
	namespace Networking {
		using namespace Serialization;
		
		class PacketHeader {
		public:
			uint16_t ContextId; // 2 bytes
			const uint8_t* AdditionalInfoBuf;
			uint16_t AdditionalInfoLength;
			uint16_t RoutineId; // 2 bytes
			uint32_t DataLength; // 4 bytes

			PacketHeader(IByteBuffer& buffer) {
				Parse(buffer);
			}
			PacketHeader()
			{}

			uint32_t GetSize() const {
				return static_cast<uint32_t>(AdditionalInfoLength) + 10;
			}

			void Serialize(IByteBuffer& buffer) const {
				buffer.Write(ContextId);
				buffer.Write(AdditionalInfoLength);
				buffer.WriteAll(AdditionalInfoBuf, AdditionalInfoLength);
				buffer.Write(RoutineId);
				buffer.Write(DataLength);
			}

			void Parse(IByteBuffer& buffer) {
				ContextId = buffer.Read<uint16_t>();
				AdditionalInfoLength = buffer.Read<uint16_t>();
				AdditionalInfoBuf = buffer.GetCurrentPtr();
				buffer.Advance(AdditionalInfoLength);
				RoutineId = buffer.Read<uint16_t>();
				DataLength = buffer.Read<uint32_t>();
			}
		};

		class Packet {
		private:
			size_t m_dataLength{ 0 };

			struct Buffer {
				const uint8_t* buf;
				size_t length;
			};
		public:
			PacketHeader Header;
			std::vector<Buffer> Data;

			void Parse(IByteBuffer& buffer) {
				Header.Parse(buffer);
				
				if (Header.DataLength == 0) {
					return;
				}
				size_t available = buffer.Available();
				if (available > 0) {
					AddData(available, buffer.GetCurrentPtr());
					buffer.SetPosition(buffer.Size());
				}
			}

			uint32_t GetSize() const {
				return Header.GetSize() + Header.DataLength;
			}

			void Serialize(IByteBuffer& buffer) const {
				assert(m_dataLength == Header.DataLength);
				Header.Serialize(buffer);
				for (auto& e : Data) {
					buffer.WriteAll(e.buf, e.length);
				}
			}

			bool IsMissingData() const {
				return m_dataLength < Header.DataLength;
			}

			void AddData(size_t length, const uint8_t* buffer) {
				Data.emplace_back(Buffer{ buffer, length });
				m_dataLength += length;
			}
		};
	}
}