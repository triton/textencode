#pragma once

#include <textencode/common.hpp>

namespace textencode
{

class ToNix32 : public Converter {
	public:
		std::string process(std::string_view data) override;
		std::string complete() override;

	private:
		std::string input;
};

class FromNix32 : public Converter {
	public:
		std::string process(std::string_view data) override;
		std::string complete() override;

	private:
		std::string input;
};

} // namespace textencode
