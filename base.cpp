#include <array>
#include <cassert>
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
		virtual ~Converter() { };

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

constexpr size_t sizeToShift(size_t size) {
	assert(size != 0);
	size_t shift = 0;
	for (; (size & 0x1) == 0; size >>= 1)
		shift += 1;
	assert(size == 1);
	return shift;
}

constexpr size_t lcm(size_t a, size_t b)
{
	assert(a != 0);
	assert(b != 0);
	for (size_t i = a; i < a * b; i += a)
		if (i % b == 0)
			return i;
	return a * b;
}

enum class CharCodes : char {
	Invalid = -1,
	Ignore = -2,
	Padding = -3,
};

template <EncodingType type>
class Properties
{
};

template <>
class Properties<EncodingType::Base16>
{
	protected:
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
	protected:
		static constexpr auto symbols = []() {
			std::array<char, 32> ret{};
			for (size_t i = 0; i < 26; ++i)
				ret[i] = 'A' + i;
			for (size_t i = 0; i < 6; ++i)
				ret[i + 26] = '0' + i;
			return ret;
		}();
};

template <>
class Properties<EncodingType::Nix32>
{
	protected:
		// 0..9 + a..z - {e,o,t,u}
		static constexpr std::array<char, 32> symbols = {
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			'a', 'b', 'c', 'd', 'f', 'g', 'h', 'i', 'j', 'k',
			'l', 'm', 'n', 'p', 'q', 'r', 's', 'v', 'w', 'x',
			'y', 'z',
		};
};

template <>
class Properties<EncodingType::Base64>
{
	protected:
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

template <EncodingType type>
class Common : public Properties<type>
{
	public:
		static constexpr size_t shift = sizeToShift(Common::symbols.size());
		static constexpr size_t quantum_bits = lcm(8, shift);
		static constexpr size_t quantum_chars = quantum_bits / shift;

		static constexpr auto inverse = []() {
			// Allow use of negatives as sentinel values
			static_assert(shift < 8);

			std::array<char, 256> ret{};
			for (size_t i = 0; i < ret.size(); ++i)
				ret[i] = static_cast<char>(CharCodes::Invalid);

			for (size_t i = 0; i < Common::symbols.size(); ++i)
				ret[Common::symbols[i]] = i;

			for (char i = 'A'; i <= 'Z'; ++i)
				if (ret[i] == static_cast<char>(CharCodes::Invalid))
					ret[i] = ret[i - 'A' + 'a'];
			for (char i = 'a'; i <= 'z'; ++i)
				if (ret[i] == static_cast<char>(CharCodes::Invalid))
					ret[i] = ret[i - 'a' + 'A'];

			assert(ret[' '] == static_cast<char>(CharCodes::Invalid));
			ret[' '] = static_cast<char>(CharCodes::Ignore);
			assert(ret['\r'] == static_cast<char>(CharCodes::Invalid));
			ret['\r'] = static_cast<char>(CharCodes::Ignore);
			assert(ret['\n'] == static_cast<char>(CharCodes::Invalid));
			ret['\n'] = static_cast<char>(CharCodes::Ignore);
			assert(ret['='] == static_cast<char>(CharCodes::Invalid));
			ret['='] = static_cast<char>(CharCodes::Padding);

			return ret;
		}();

};

template <EncodingType type>
class ToBaseN : public Converter, protected Common<type> {
	public:
		std::string process(std::string_view data) override {
			std::string ret;
			ret.reserve(data.size() << 1);
			for (auto byte : data) {
				if (num_bits == ToBaseN::quantum_bits)
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
			ret.reserve(ToBaseN::quantum_chars);
			flushBuffer(ret);
			if (num_bits > 0)
				ret += toSymbol(buffer << (ToBaseN::shift - num_bits));
			ret.resize(ToBaseN::quantum_chars, '=');
			return ret;
		}

	private:
		uint64_t buffer = 0;
		static_assert(ToBaseN::quantum_bits < sizeof(decltype(buffer)) * 8);
		uint8_t num_bits = 0;
		
		void flushBuffer(std::string &out) {
			for (; num_bits >= ToBaseN::shift; num_bits -= ToBaseN::shift)
				out += toSymbol(buffer >> (num_bits - ToBaseN::shift));
		}

		static char toSymbol(char byte) {
			return ToBaseN::symbols[byte & (ToBaseN::symbols.size() - 1)];
		}
};

template <EncodingType type>
class FromBaseN : public Converter, protected Common<type> {
	public:
		std::string process(std::string_view data) override {
			std::string ret;
			ret.reserve(data.size());
			for (char symbol : data) {
				char byte = FromBaseN::inverse[symbol];
				if (byte == static_cast<char>(CharCodes::Invalid))
					throw std::runtime_error("Invalid symbol");
				if (byte == static_cast<char>(CharCodes::Ignore))
					continue;
				if (byte == static_cast<char>(CharCodes::Padding)) {
					padding_bits += FromBaseN::shift;
					continue;
				}
				if (padding_bits > 0)
					throw std::runtime_error("Invalid padding");

				if (num_bits == FromBaseN::quantum_bits)
					flushBuffer(ret);
				buffer = (buffer << FromBaseN::shift) | byte;
				num_bits += FromBaseN::shift;
			}
			return ret;
		}

		std::string complete() override {
			if (padding_bits >= FromBaseN::quantum_bits)
				throw std::runtime_error("Too much padding");
			if ((num_bits + padding_bits) % FromBaseN::quantum_bits != 0)
				throw std::runtime_error("Bad input width");
			int zero_mask = (1 << (num_bits % 8)) - 1;
			if (buffer & zero_mask)
				throw std::runtime_error("Bad encoding");

			std::string ret;
			ret.reserve(num_bits >> 3);
			flushBuffer(ret);
			if (num_bits >= FromBaseN::shift)
				throw std::runtime_error("Invalid padding");
			return ret;
		}

	private:
		uint64_t buffer = 0;
		static_assert(FromBaseN::quantum_bits < sizeof(decltype(buffer)) * 8);
		uint8_t num_bits = 0;
		uint8_t padding_bits = 0;

		void flushBuffer(std::string &out) {
			for (; num_bits >= 8; num_bits -= 8)
				out += buffer >> (num_bits - 8);
		}
};

class ToNix32 : public Converter, protected Common<EncodingType::Nix32> {
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

class FromNix32 : public Converter, protected Common<EncodingType::Nix32> {
	public:
		std::string process(std::string_view data) override {
			input += data;
			return {};
		}

		std::string complete() override {
			if ((input.size() + 7) * 5 / 8 == (input.size() + 8) * 5 / 8)
				throw std::runtime_error("Invalid nix32 length");
			size_t num_zeroes = input.size() * 5 % 8;
			int zero_mask = ((1 << num_zeroes) - 1) << (5 - num_zeroes);
			if (inverse[input[0]] & zero_mask)
				throw std::runtime_error("Invalid nix32 hash");

			std::string ret;
			ret.reserve(input.size());
			return ret;
		}

	private:
		std::string input;
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
