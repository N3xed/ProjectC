#pragma once

#include <array>

namespace ProjectC::Utils {

	template<size_t n>
	bool CompareByteArrays(const std::array<uint8_t, n>& arr1, const std::array<uint8_t, n>& arr2) {
		return memcmp(arr1.data(), arr2.data(), n) == 0;
	}

	template bool CompareByteArrays<128>(const std::array<uint8_t, 128>& arr1, const std::array<uint8_t, 128>& arr2);

	template<typename From, typename To>
	struct PointerConverter {
		To* operator()(From* e) {
			return static_cast<To*>(e);
		}
	};
}