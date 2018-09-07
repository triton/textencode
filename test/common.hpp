#pragma once

#include <string>
#include <string_view>
#include <textencode/common.hpp>

namespace textencode
{

template <typename Encoder>
std::string encode_trivial(std::string_view data)
{
	Encoder e;
	std::string ret = e.process(data);
	ret += e.complete();
	return ret;
}

} // namespace textencode
