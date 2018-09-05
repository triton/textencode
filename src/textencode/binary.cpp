#include <string>
#include <textencode/binary.hpp>

namespace textencode {

std::string Binary::process(std::string_view data)
{
	return std::string(data);
}

std::string Binary::complete()
{
	return {};
}

} // namespace textencode
