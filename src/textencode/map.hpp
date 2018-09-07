#pragma once

#include <functional>
#include <memory>
#include <textencode/common.hpp>
#include <unordered_map>

namespace textencode {

using ConverterMap =
    std::unordered_map<EncodingType,
                       std::function<std::unique_ptr<Converter>()>>;

extern const ConverterMap to_binary;
extern const ConverterMap from_binary;

}  // namespace textencode
