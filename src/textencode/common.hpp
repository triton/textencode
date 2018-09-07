#pragma once

#include <string>
#include <string_view>

namespace textencode {

enum class EncodingType {
    Binary,
    Base16,
    Base32,
    Nix32,
    Base64,
};

class Converter {
  public:
    virtual ~Converter(){};

    virtual std::string process(std::string_view data) = 0;
    virtual std::string complete() = 0;
};

}  // namespace textencode
