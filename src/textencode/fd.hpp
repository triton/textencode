#pragma once

#include <textencode/common.hpp>

namespace textencode {

void transcode(int fd_in, EncodingType from, int fd_out, EncodingType to);

}  // namespace textencode
