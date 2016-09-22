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
			uint32_t ContextId;
			Buffer AddionalInformation;
			uint32_t RoutineId;
			uint64_t RoutineParameters;
			uint32_t DataLength;

			PacketHeader(IByteBuffer& buffer) {
				Parse(buffer);
			}
			PacketHeader()
			{}

			void Parse(IByteBuffer& buffer) {
				ContextId = buffer.GetUInt();
				AddionalInformation.Length = (size_t)buffer.GetUShort();
				AddionalInformation.DataPtr = buffer.GetBytes(AddionalInformation.Length);
				RoutineId = buffer.GetUInt();
				RoutineParameters = buffer.GetULong();
				DataLength = buffer.GetUInt();
			}
		};

		class Packet {
		private:
			bool m_missingData{ true };
		public:
			PacketHeader Header;
			std::vector<Buffer> Data;

			void Parse(IByteBuffer& buffer) {
				Header.Parse(buffer);
				
				if (Header.DataLength == 0) {
					m_missingData = false;
					return;
				}
				size_t available = buffer.Available();
				if (available > 0)
					AddData(available, buffer.GetBytes());
			}

			bool MissingData() const {
				return m_missingData;
			}

			void AddData(size_t length, const uint8_t* buffer) {
				Data.emplace_back(length, buffer);

				m_missingData = std::accumulate(Data.begin(), Data.end(), Data.front().Length, [](size_t a, Buffer& b) {
					return a + b.Length;
				}) < Header.DataLength;
			}
		};
	}
}