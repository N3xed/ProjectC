#pragma once

#include <stdint.h>
#include "IByteBuffer.h"
#include <vector>
#include "Container.h"
#include <numeric>

namespace ProjectC {
	namespace Networking {
		
		class PacketHeader {
		public:
			uint16_t ContextId; // 2 bytes
			Buffer AddionalInformation; // 2 bytes + data
			uint16_t RoutineId; // 2 bytes
			uint32_t DataLength; // 4 bytes

			PacketHeader(IByteBuffer& buffer) {
				Parse(buffer);
			}
			PacketHeader()
			{}

			uint32_t GetSize() const {
				return AddionalInformation.Length + 10;
			}

			void Serialize(IByteBuffer& buffer) const {
				buffer.WriteUShort(ContextId);
				buffer.WriteUShort(static_cast<uint16_t>(AddionalInformation.Length));
				buffer.WriteBytes(AddionalInformation.DataPtr, AddionalInformation.Length);
				buffer.WriteUShort(RoutineId);
				buffer.WriteUInt(DataLength);
			}

			void Parse(IByteBuffer& buffer) {
				ContextId = buffer.GetUInt();
				AddionalInformation.Length = (size_t)buffer.GetUShort();
				AddionalInformation.DataPtr = buffer.GetBytes(AddionalInformation.Length);
				RoutineId = buffer.GetUInt();
				DataLength = buffer.GetUInt();
			}
		};

		class Packet {
		private:
			uint32_t m_dataLength{ 0 };
		public:
			PacketHeader Header;
			std::vector<Buffer> Data;

			void Parse(IByteBuffer& buffer) {
				Header.Parse(buffer);
				
				if (Header.DataLength == 0) {
					return;
				}
				size_t available = buffer.Available();
				if (available > 0)
					AddData(available, buffer.GetBytes());
			}

			uint32_t GetSize() const {
				return Header.GetSize() + Header.DataLength;
			}

			void Serialize(IByteBuffer& buffer) const {
				assert(m_dataLength == Header.DataLength);
				Header.Serialize(buffer);
				for (auto& e : Data) {
					buffer.WriteBytes(e.DataPtr, e.Length);
				}
			}

			bool IsMissingData() const {
				return m_dataLength < Header.DataLength;
			}

			void AddData(size_t length, const uint8_t* buffer) {
				Data.emplace_back(length, buffer);
				m_dataLength += length;
			}
		};
	}
}