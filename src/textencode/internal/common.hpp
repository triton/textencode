#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <textencode/common.hpp>
#include <textencode/internal/utils.hpp>

namespace textencode::internal {

enum class CharCodes : char {
    Invalid = -1,
    Ignore = -2,
    Padding = -3,
};

template <EncodingType type>
class Properties {};

template <EncodingType type>
class Common : public Properties<type> {
  public:
    static constexpr size_t shift = sizeToShift(Common::symbols.size());
    static constexpr size_t quantum_bits = lcm(8, shift);
    static constexpr size_t quantum_symbols = quantum_bits / shift;

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

    static constexpr bool validByte(char byte) {
        return (byte & ~((1 << shift) - 1)) == 0;
    };
};

}  // namespace textencode::internal
