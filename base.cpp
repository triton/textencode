#include <array>
#include <cerrno>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include <unistd.h>

#include "CLI11.hpp"

enum class EncodingType {
	Binary,
	Base16,
	Base32,
	Nix32,
	Base64,
};

namespace converter {

class Converter {
	public:
		virtual std::string process(std::string_view data) = 0;
		virtual std::string complete() = 0;
};

class Binary : public Converter {
	public:
		std::string process(std::string_view data) override {
			return std::string(data);
		}

		std::string complete() override {
			return {};
		}
};

constexpr size_t lcm(size_t a, size_t b)
{
	for (size_t i = a; i < a * b; i += a)
		if (i % b == 0)
			return i;
	return a * b;
}

template<EncodingType type>
constexpr size_t shiftWidth()
{
	throw std::logic_error("Invalid call");
}

template<>
constexpr size_t shiftWidth<EncodingType::Base16>()
{
	return 4;
}

template<>
constexpr size_t shiftWidth<EncodingType::Base32>()
{
	return 5;
}

template<>
constexpr size_t shiftWidth<EncodingType::Base64>()
{
	return 6;
}

template <EncodingType type>
char toSymbol(char byte)
{
	(void)byte;
	throw std::logic_error("Invalid call");
}

template <>
char toSymbol<EncodingType::Base16>(char byte) {
	byte &= 0xf;
	if (byte < 10)
		return byte + '0';
	return byte + 'A' - 10;
}

template <>
char toSymbol<EncodingType::Base32>(char byte) {
	byte &= 0x1f;
	if (byte < 26)
		return byte + 'A';
	return byte + '2' - 26;
}

template <>
char toSymbol<EncodingType::Base64>(char byte) {
	byte &= 0x3f;
	if (byte < 26)
		return byte + 'A';
	if (byte < 52)
		return byte + 'a' - 26;
	if (byte < 62)
		return byte + '0' - 52;
	if (byte == 62)
		return '+';
	return '/';
}

template <EncodingType type>
char fromSymbol(char symbol)
{
	(void)symbol;
	throw std::logic_error("Invalid call");
}

template <>
char fromSymbol<EncodingType::Base16>(char symbol) {
	if (symbol >= '0' && symbol <= '9')
		return symbol - '0';
	if (symbol >= 'A' && symbol <= 'F')
		return 10 + symbol - 'A';
	if (symbol >= 'a' && symbol <= 'f')
		return 10 + symbol - 'a';
	throw std::runtime_error("Not a base16 character");
}

template <>
char fromSymbol<EncodingType::Base32>(char symbol) {
	if (symbol >= 'A' && symbol <= 'Z')
		return symbol - 'A';
	if (symbol >= 'a' && symbol <= 'z')
		return symbol - 'a';
	if (symbol >= '2' && symbol <= '7')
		return 26 + symbol - '2';
	throw std::runtime_error("Not a base32 character");
}

template <>
char fromSymbol<EncodingType::Base64>(char symbol) {
	if (symbol >= 'A' && symbol <= 'Z')
		return symbol - 'A';
	if (symbol >= 'a' && symbol <= 'z')
		return 26 + symbol - 'a';
	if (symbol >= '0' && symbol <= '9')
		return 52 + symbol - '0';
	if (symbol == '+')
		return 62;
	if (symbol == '/')
		return 63;
	throw std::runtime_error("Not a base64 character");
}

template <EncodingType type>
class ToBaseN : public Converter {
	public:
		std::string process(std::string_view data) override {
			std::string ret;
			ret.reserve(data.size() << 1);
			for (auto byte : data) {
				if (num_bits == quantum_bits)
					flushBuffer(ret);
				buffer = (buffer << 8) | (byte & 0xff);
				num_bits += 8;
			}
			return ret;
		}

		std::string complete() override {
			if (num_bits == 0)
				return {};

			std::string ret;
			ret.reserve(quantum_chars);
			flushBuffer(ret);
			if (num_bits > 0)
				ret += toSymbol<type>(buffer << (shift - num_bits));
			ret.resize(quantum_chars, '=');
			return ret;
		}

	private:
		static constexpr size_t shift = shiftWidth<type>();
		static constexpr size_t quantum_bits = lcm(8, shift);
		static constexpr size_t quantum_chars = quantum_bits / shift;

		static_assert(quantum_bits < sizeof(uint64_t) << 3);
		uint64_t buffer = 0;
		uint8_t num_bits = 0;
		
		void flushBuffer(std::string &out) {
			for (; num_bits >= shift; num_bits -= shift)
				out += toSymbol<type>(buffer >> (num_bits - shift));
		}
};

template <EncodingType type>
class FromBaseN : public Converter {
	public:
		std::string process(std::string_view data) override {
			std::string ret;
			ret.reserve(data.size());
			for (char byte : data) {
				if (byte == ' ' || byte == '\n' || byte == '\r')
					continue;
				if (byte == '=') {
					padding_bits += shift;
					continue;
				}
				if (padding_bits > 0)
					throw std::runtime_error("Invalid padding");

				if (num_bits == quantum_bits)
					flushBuffer(ret);
				buffer = (buffer << shift) | fromSymbol<type>(byte);
				num_bits += shift;
			}
			return ret;
		}

		std::string complete() override {
			if (padding_bits >= quantum_bits)
				throw std::runtime_error("Too much padding");
			if ((num_bits + padding_bits) % quantum_bits != 0)
				throw std::runtime_error("Bad input width");
			int zero_mask = (1 << (num_bits % 8)) - 1;
			if (buffer & zero_mask)
				throw std::runtime_error("Bad encoding");

			std::string ret;
			ret.reserve(num_bits >> 3);
			flushBuffer(ret);
			if (num_bits >= shift)
				throw std::runtime_error("Invalid padding");
			return ret;
		}

	private:
		static constexpr size_t shift = shiftWidth<type>();
		static constexpr size_t quantum_bits = lcm(8, shift);
		static constexpr size_t quantum_chars = quantum_bits / shift;

		static_assert(quantum_bits < sizeof(uint64_t) << 3);
		uint64_t buffer = 0;
		uint8_t num_bits = 0;
		uint8_t padding_bits = 0;

		void flushBuffer(std::string &out) {
			for (; num_bits >= 8; num_bits -= 8)
				out += buffer >> (num_bits - 8);
		}
};

// Must be lower case
constexpr std::string_view nix32_symbols = "0123456789abcdfghijklmnpqrsvwxyz";
static_assert(nix32_symbols.size() == 32);

class ToNix32 : public Converter {
	public:
		std::string process(std::string_view data) override {
			input += data;
			return {};
		}

		std::string complete() override {
			std::string ret;
			ret.reserve((input.size() * 8 + 9) / 5 - 1);
			return ret;
		}

	private:
		std::string input;
};

class FromNix32 : public Converter {
	public:
		FromNix32() {
			for (char i = 0; i < 32; ++i)
				inverse[nix32_symbols[i]] = i;
		}

		std::string process(std::string_view data) override {
			input += data;
			return {};
		}

		std::string complete() override {
			if ((input.size() + 7) * 5 / 8 == (input.size() + 8) * 5 / 8)
				throw std::runtime_error("Invalid nix32 length");
			size_t num_zeroes = input.size() * 5 % 8;
			int zero_mask = ((1 << num_zeroes) - 1) << (4 - num_zeroes);
			if (inverse.at(input[0]) & zero_mask)
				throw std::runtime_error("Invalid nix32 hash");

			std::string ret;
			ret.reserve(input.size());
			return ret;
		}

	private:
		std::string input;
		std::unordered_map<char, char> inverse;
};

using Map = std::unordered_map<EncodingType, std::function<std::unique_ptr<converter::Converter>()>>;

const Map to_binary = {
	{EncodingType::Binary, [](){ return std::make_unique<converter::Binary>(); }},
	{EncodingType::Base16, [](){ return std::make_unique<converter::ToBaseN<EncodingType::Base16>>(); }},
	{EncodingType::Base32, [](){ return std::make_unique<converter::ToBaseN<EncodingType::Base32>>(); }},
	{EncodingType::Nix32, [](){ return std::make_unique<converter::ToNix32>(); }},
	{EncodingType::Base64, [](){ return std::make_unique<converter::ToBaseN<EncodingType::Base64>>(); }},
};

const Map from_binary = {
	{EncodingType::Binary, [](){ return std::make_unique<converter::Binary>(); }},
	{EncodingType::Base16, [](){ return std::make_unique<converter::FromBaseN<EncodingType::Base16>>(); }},
	{EncodingType::Base32, [](){ return std::make_unique<converter::FromBaseN<EncodingType::Base32>>(); }},
	{EncodingType::Nix32, [](){ return std::make_unique<converter::FromNix32>(); }},
	{EncodingType::Base64, [](){ return std::make_unique<converter::FromBaseN<EncodingType::Base64>>(); }},
};

}  // namespace converter

const std::unordered_map<std::string, EncodingType> type_map = {
	{"bin", EncodingType::Binary},
	{"binary", EncodingType::Binary},
	{"base16", EncodingType::Base16},
	{"hex", EncodingType::Base16},
	{"base32", EncodingType::Base32},
	{"nix32", EncodingType::Nix32},
	{"base64", EncodingType::Base64},
};

std::string validateEncoding(const std::string &opt)
{
	if (type_map.find(opt) != type_map.end())
		return "";
	return opt + " is not a valid encoding type";
}

template <std::size_t N>
std::string_view read(int fd, std::array<char, N> &buffer)
{
	ssize_t ret = read(fd, buffer.data(), N);
	if (ret < 0)
		throw std::system_error(errno, std::generic_category(), "Failed to read data");
	return {buffer.data(), static_cast<size_t>(ret)};
}

void write(int fd, std::string_view data)
{
	ssize_t ret = write(fd, data.data(), data.size());
	if (ret < 0)
		throw std::system_error(errno, std::generic_category(), "Failed to write data");
	if (static_cast<size_t>(ret) != data.size())
		throw std::runtime_error("Failed to write data");
}

void transcode(int fd_in, EncodingType from, int fd_out, EncodingType to)
{
	std::unique_ptr<converter::Converter> from_func = converter::from_binary.at(from)();
	std::unique_ptr<converter::Converter> to_func = converter::to_binary.at(to)();

	std::array<char, 4096> buffer;
	std::string_view data;
	while (data = read(fd_in, buffer), data.size() > 0)
		write(fd_out, to_func->process(from_func->process(data)));

	write(fd_out, to_func->process(from_func->complete()));
	write(fd_out, to_func->complete());
}

int main(int argc, char *argv[])
{
	CLI::App app{"Binary Encoding Converter"};
	std::string to_str, from_str;
	app.add_option("-t,--to", to_str, "The type to convert to")
		->required()
		->check(validateEncoding);
	app.add_option("-f,--from", from_str, "The type to convert from")
		->required()
		->check(validateEncoding);
	CLI11_PARSE(app, argc, argv);

	try {
		transcode(STDIN_FILENO, type_map.at(from_str),
				STDOUT_FILENO, type_map.at(to_str));
	} catch(const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}
