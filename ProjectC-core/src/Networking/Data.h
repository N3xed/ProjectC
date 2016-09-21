#pragma once

#include <stdint.h>
#include "IByteBuffer.h"
#include <vector>
#include "Container.hpp"
#include <numeric>

namespace ProjectC {
	namespace Networking {
		class Data {
		private:
			IByteBuffer& m_buffer;
		public:
			uint32_t ContextId;
			Buffer AddionalInformation;
			uint32_t RoutineId;
			uint64_t RoutineParameters;
			uint32_t InnerDataLength;
			std::vector<Buffer> InnerData;

			Data(IByteBuffer& buffer) : m_buffer(buffer) {
				ContextId = m_buffer.GetUInt();
				AddionalInformation.Length = (size_t)m_buffer.GetUShort();
				AddionalInformation.DataPtr = m_buffer.GetBytes(AddionalInformation.Length);
				RoutineId = m_buffer.GetUInt();
				RoutineParameters = m_buffer.GetULong();
				InnerDataLength = m_buffer.GetUInt();
				AddInnerData(m_buffer);
			}

			void AddInnerData(Buffer buffer) {
				InnerData.emplace_back(buffer);
			}
			void AddInnerData(const uint8_t* buffer, size_t length) {
				InnerData.emplace_back(length, buffer);
			}
			void AddInnerData(IByteBuffer& buffer) {
				InnerData.emplace_back(buffer.Available(), buffer.GetBytes());
			}

			bool MissingInnerData() const {
				size_t length = std::accumulate(InnerData.begin(), InnerData.end(), InnerData.front().Length, [](size_t a, const Buffer& b) {
					return a + b.Length;
				});
				return length < InnerDataLength;
			}
		};
	}
}