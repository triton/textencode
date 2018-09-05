#include <memory>
#include <textencode/base_n.hpp>
#include <textencode/binary.hpp>
#include <textencode/common.hpp>
#include <textencode/map.hpp>
#include <textencode/nix.hpp>

namespace textencode
{

const ConverterMap to_binary = {
	{EncodingType::Binary, [](){ return std::make_unique<Binary>(); }},
	{EncodingType::Base16, [](){ return std::make_unique<ToBase16>(); }},
	{EncodingType::Base32, [](){ return std::make_unique<ToBase32>(); }},
	{EncodingType::Nix32, [](){ return std::make_unique<ToNix32>(); }},
	{EncodingType::Base64, [](){ return std::make_unique<ToBase64>(); }},
};

const ConverterMap from_binary = {
	{EncodingType::Binary, [](){ return std::make_unique<Binary>(); }},
	{EncodingType::Base16, [](){ return std::make_unique<FromBase16>(); }},
	{EncodingType::Base32, [](){ return std::make_unique<FromBase32>(); }},
	{EncodingType::Nix32, [](){ return std::make_unique<FromNix32>(); }},
	{EncodingType::Base64, [](){ return std::make_unique<FromBase64>(); }},
};

} // namespace textencode
