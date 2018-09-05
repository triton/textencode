#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <textencode/common.hpp>

namespace textencode
{

template <EncodingType type>
class ToBaseN : public Converter {
	public:
		std::string process(std::string_view data) override;
		std::string complete() override;

	private:
		uint64_t buffer = 0;
		uint8_t num_bits = 0;
		
		void flushBuffer(std::string &out);
		static char toSymbol(char byte);
};

using ToBase16 = ToBaseN<EncodingType::Base16>;
using ToBase32 = ToBaseN<EncodingType::Base32>;
using ToBase64 = ToBaseN<EncodingType::Base64>;

template <EncodingType type>
class FromBaseN : public Converter {
	public:
		std::string process(std::string_view data) override;
		std::string complete() override;

	private:
		uint64_t buffer = 0;
		uint8_t num_bits = 0;
		uint8_t padding_bits = 0;

		void flushBuffer(std::string &out);
};

using FromBase16 = FromBaseN<EncodingType::Base16>;
using FromBase32 = FromBaseN<EncodingType::Base32>;
using FromBase64 = FromBaseN<EncodingType::Base64>;

} // namespace textencode
