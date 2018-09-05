#include <cstddef>
#include <stdexcept>
#include <string>
#include <textencode/internal/common.hpp>
#include <textencode/internal/nix.hpp>
#include <textencode/nix.hpp>

namespace textencode
{

using internal::CharCodes;
using Common = internal::Common<EncodingType::Nix32>;

std::string ToNix32::process(std::string_view data)
{
	input += data;
	return {};
}

std::string ToNix32::complete()
{
	std::string ret((input.size() * 8 + 9) / 5 - 1, 0);

	for (size_t i = 0; i < ret.size(); ++i) {
		const size_t bit_offset = (ret.size() - i - 1) * 5;
		const size_t byte_offset = bit_offset >> 3;
		const size_t byte_shift = bit_offset & 0x7;
		const int upper = input[byte_offset] & 0xff;
		const int lower = byte_offset + 1 == input.size() ? 0
			: input[byte_offset + 1] & 0xff;
		const char byte = (upper >> byte_shift)
			| (lower << (8 - byte_shift));
		ret[i] = Common::symbols[byte & 0x1f];
	}

	return ret;
}

std::string FromNix32::process(std::string_view data)
{
	for (const char symbol : data) {
		const char byte = Common::inverse[symbol];
		if (byte == static_cast<char>(CharCodes::Ignore))
			continue;
		if (!Common::validByte(byte))
			throw std::runtime_error("Invalid symbol");

		input += byte;
	}

	return {};
}

std::string FromNix32::complete()
{
	if ((input.size() + 7) * 5 / 8 == (input.size() + 8) * 5 / 8)
		throw std::runtime_error("Invalid nix32 length");
	const size_t num_zeroes = input.size() * 5 % 8;
	const int zero_mask = ((1 << num_zeroes) - 1) << (5 - num_zeroes);
	if (input[0] & zero_mask)
		throw std::runtime_error("Invalid nix32 hash");

	std::string ret(input.size() * 5 / 8, 0);

	for (size_t i = 0; i < input.size(); ++i) {
		const char byte = input[input.size() - 1 - i];
		const size_t bit_offset = i * 5;
		const size_t byte_offset = bit_offset >> 3;
		const size_t byte_shift = bit_offset & 0x7;
		ret[byte_offset] |= byte << byte_shift;
		ret[byte_offset + 1] |= byte >> (8 - byte_shift);
	}

	return ret;
}

} // namespace textencode
