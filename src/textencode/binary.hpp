#pragma once

#include <textencode/common.hpp>

namespace textencode
{

class Binary : public Converter {
	public:
		std::string process(std::string_view data) override;
		std::string complete() override;
};

} // namespace textencode
