#pragma once

#include <cassert>
#include <cstddef>

namespace textencode::internal
{

constexpr size_t sizeToShift(size_t size)
{
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

} // namespace textencode::internal
