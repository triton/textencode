#pragma once

#include <array>
#include <textencode/common.hpp>
#include <textencode/internal/common.hpp>

namespace textencode::internal {

template <>
class Properties<EncodingType::Nix32> {
  public:
    // 0..9 + a..z - {e,o,t,u}
    static constexpr std::array<char, 32> symbols = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a',
        'b', 'c', 'd', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'p', 'q', 'r', 's', 'v', 'w', 'x', 'y', 'z',
    };
};

}  // namespace textencode::internal
