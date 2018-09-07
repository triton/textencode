#include <unistd.h>
#include <stdexcept>
#include <string>
#include <system_error>
#include <textencode/fd.hpp>
#include <textencode/map.hpp>

namespace textencode {

namespace {

std::string read(int fd, size_t size) {
    std::string buffer(size, '\0');
    ssize_t ret = ::read(fd, buffer.data(), buffer.size());
    if (ret < 0)
        throw std::system_error(errno, std::generic_category(),
                                "Failed to read data");
    buffer.resize(ret);
    return buffer;
}

void write(int fd, std::string_view data) {
    ssize_t ret = ::write(fd, data.data(), data.size());
    if (ret < 0)
        throw std::system_error(errno, std::generic_category(),
                                "Failed to write data");
    if (static_cast<size_t>(ret) != data.size())
        throw std::runtime_error("Failed to write data");
}

}  // namespace

void transcode(int fd_in, EncodingType from, int fd_out, EncodingType to) {
    auto from_func = from_binary.at(from)();
    auto to_func = to_binary.at(to)();

    std::string data;
    while (data = read(fd_in, 4096), data.size() > 0)
        write(fd_out, to_func->process(from_func->process(data)));

    write(fd_out, to_func->process(from_func->complete()));
    write(fd_out, to_func->complete());
}

}  // namespace textencode
