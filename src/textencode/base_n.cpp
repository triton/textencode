#include <stdexcept>
#include <string>
#include <textencode/base_n.hpp>
#include <textencode/common.hpp>
#include <textencode/internal/base_n.hpp>
#include <textencode/internal/common.hpp>

namespace textencode {

using internal::CharCodes;
using internal::Common;

template <EncodingType type>
std::string ToBaseN<type>::process(std::string_view data) {
    constexpr auto quantum_bits = Common<type>::quantum_bits;
    static_assert(quantum_bits < sizeof(decltype(buffer)) * 8);

    std::string ret;
    ret.reserve(data.size() << 1);

    for (const auto byte : data) {
        if (num_bits == quantum_bits)
            flushBuffer(ret);
        buffer = (buffer << 8) | (byte & 0xff);
        num_bits += 8;
    }

    return ret;
}

template <EncodingType type>
std::string ToBaseN<type>::complete() {
    if (num_bits == 0)
        return {};

    std::string ret;
    ret.reserve(Common<type>::quantum_symbols);

    flushBuffer(ret);
    if (num_bits > 0)
        ret += toSymbol(buffer << (Common<type>::shift - num_bits));
    ret.resize(Common<type>::quantum_symbols, '=');

    return ret;
}

template <EncodingType type>
void ToBaseN<type>::flushBuffer(std::string& out) {
    constexpr auto shift = Common<type>::shift;
    for (; num_bits >= shift; num_bits -= shift)
        out += toSymbol(buffer >> (num_bits - shift));
}

template <EncodingType type>
char ToBaseN<type>::toSymbol(char byte) {
    constexpr auto& symbols = Common<type>::symbols;
    return symbols[byte & (symbols.size() - 1)];
}

template class ToBaseN<EncodingType::Base16>;
template class ToBaseN<EncodingType::Base32>;
template class ToBaseN<EncodingType::Base64>;

template <EncodingType type>
std::string FromBaseN<type>::process(std::string_view data) {
    constexpr auto shift = Common<type>::shift;
    constexpr auto quantum_bits = Common<type>::quantum_bits;
    static_assert(quantum_bits < sizeof(decltype(buffer)) * 8);

    std::string ret;
    ret.reserve(data.size());

    for (const char symbol : data) {
        const char byte = Common<type>::inverse[symbol];
        if (byte == static_cast<char>(CharCodes::Ignore))
            continue;
        if (byte == static_cast<char>(CharCodes::Padding)) {
            padding_bits += shift;
            continue;
        }
        if (padding_bits > 0)
            throw std::runtime_error("Invalid padding");
        if (!Common<type>::validByte(byte))
            throw std::runtime_error("Invalid symbol");

        if (num_bits == quantum_bits)
            flushBuffer(ret);
        buffer = (buffer << shift) | byte;
        num_bits += shift;
    }

    return ret;
}

template <EncodingType type>
std::string FromBaseN<type>::complete() {
    constexpr auto quantum_bits = Common<type>::quantum_bits;
    if (padding_bits >= quantum_bits)
        throw std::runtime_error("Too much padding");
    if ((num_bits + padding_bits) % quantum_bits != 0)
        throw std::runtime_error("Bad input width");
    const int zero_mask = (1 << (num_bits % 8)) - 1;
    if (buffer & zero_mask)
        throw std::runtime_error("Bad encoding");

    std::string ret;
    ret.reserve(num_bits >> 3);

    flushBuffer(ret);
    if (num_bits >= Common<type>::shift)
        throw std::runtime_error("Invalid padding");

    return ret;
}

template <EncodingType type>
void FromBaseN<type>::flushBuffer(std::string& out) {
    for (; num_bits >= 8; num_bits -= 8)
        out += buffer >> (num_bits - 8);
}

template class FromBaseN<EncodingType::Base16>;
template class FromBaseN<EncodingType::Base32>;
template class FromBaseN<EncodingType::Base64>;

}  // namespace textencode
