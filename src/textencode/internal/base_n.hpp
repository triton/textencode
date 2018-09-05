#pragma once

#include <array>
#include <cstddef>
#include <textencode/common.hpp>
#include <textencode/internal/common.hpp>

namespace textencode::internal
{

template <>
class Properties<EncodingType::Base16>
{
	public:
		static constexpr auto symbols = []() {
			std::array<char, 16> ret{};
			for (size_t i = 0; i < 10; ++i)
				ret[i] = '0' + i;
			for (size_t i = 0; i < 6; ++i)
				ret[i + 10] = 'A' + i;
			return ret;
		}();
};

template <>
class Properties<EncodingType::Base32>
{
	public:
		static constexpr auto symbols = []() {
			std::array<char, 32> ret{};
			for (size_t i = 0; i < 26; ++i)
				ret[i] = 'A' + i;
			for (size_t i = 0; i < 6; ++i)
				ret[i + 26] = '2' + i;
			return ret;
		}();
};

template <>
class Properties<EncodingType::Base64>
{
	public:
		static constexpr auto symbols = []() {
			std::array<char, 64> ret{};
			for (size_t i = 0; i < 26; ++i)
				ret[i] = 'A' + i;
			for (size_t i = 0; i < 26; ++i)
				ret[i + 26] = 'a' + i;
			for (size_t i = 0; i < 10; ++i)
				ret[i + 52] = '0' + i;
			ret[62] = '+';
			ret[63] = '/';
			return ret;
		}();
};

} // namespace textencode::internal
