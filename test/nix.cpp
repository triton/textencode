#include <gtest/gtest.h>
#include <textencode/nix.hpp>

#include "common.hpp"

namespace textencode {

TEST(Nix32Test, NoInputTo) {
    EXPECT_EQ("", ToNix32().complete());
    EXPECT_EQ("", encode_trivial<ToNix32>(""));
}

TEST(Nix32Test, NoInputFrom) {
    EXPECT_EQ("", FromNix32().complete());
    EXPECT_EQ("", encode_trivial<FromNix32>(""));
}

TEST(Nix32Test, Encode) {
    EXPECT_EQ("36", encode_trivial<ToNix32>("f"));
    EXPECT_EQ("0vv6", encode_trivial<ToNix32>("fo"));
    EXPECT_EQ("6yvv6", encode_trivial<ToNix32>("foo"));
    EXPECT_EQ("1i6yvv6", encode_trivial<ToNix32>("foob"));
    EXPECT_EQ("c5i6yvv6", encode_trivial<ToNix32>("fooba"));
    EXPECT_EQ("3jc5i6yvv6", encode_trivial<ToNix32>("foobar"));

    // No leading zero
    EXPECT_EQ("i5i6yvv6", encode_trivial<ToNix32>("foob\x89"));
}

TEST(Nix32Test, Decode) {
    EXPECT_EQ("f", encode_trivial<FromNix32>("36"));
    EXPECT_EQ("fo", encode_trivial<FromNix32>("0Vv6"));
    EXPECT_EQ("foo", encode_trivial<FromNix32>("6yvv6"));
    EXPECT_EQ("foob", encode_trivial<FromNix32>("1i6yvv6"));
    EXPECT_EQ("fooba", encode_trivial<FromNix32>("C5i6Yvv6"));
    EXPECT_EQ("foobar", encode_trivial<FromNix32>("3Jc5i6yVv6"));

    // No leading zero
    EXPECT_EQ("foob\x89", encode_trivial<FromNix32>("i5i6yvv6"));
}

TEST(Nix32Test, IgnoreCharacters) {
    EXPECT_EQ("foo", encode_trivial<FromNix32>("6 y\n\rvv6"));
}

TEST(Nix32Test, BadCharacters) {
    EXPECT_THROW(FromNix32().process("e"), std::runtime_error);
    EXPECT_THROW(FromNix32().process("E"), std::runtime_error);
    EXPECT_THROW(FromNix32().process("+"), std::runtime_error);
}

TEST(Nix32Test, BadPaddingSuperfluous) {
    EXPECT_THROW(encode_trivial<FromNix32>("="), std::runtime_error);
    EXPECT_THROW(encode_trivial<FromNix32>("=="), std::runtime_error);
    EXPECT_THROW(encode_trivial<FromNix32>("0vv="), std::runtime_error);
    EXPECT_THROW(encode_trivial<FromNix32>("0vv=="), std::runtime_error);
    EXPECT_THROW(encode_trivial<FromNix32>("0v=v="), std::runtime_error);
}

TEST(Nix32Test, IllegalEncoding) {
    EXPECT_THROW(encode_trivial<FromNix32>("0"), std::runtime_error);
    EXPECT_THROW(encode_trivial<FromNix32>("000"), std::runtime_error);
    EXPECT_THROW(encode_trivial<FromNix32>("000000"), std::runtime_error);
}

TEST(Nix32Test, BadZeroEncoding) {
    // Mutation of valid 36
    EXPECT_THROW(encode_trivial<FromNix32>("b6"), std::runtime_error);
    EXPECT_THROW(encode_trivial<FromNix32>("k6"), std::runtime_error);
    // Mutations of valid 0vv6
    EXPECT_THROW(encode_trivial<FromNix32>("hvv6"), std::runtime_error);
    EXPECT_THROW(encode_trivial<FromNix32>("2vvh"), std::runtime_error);
    // Mutations of valid 6yvv6
    EXPECT_THROW(encode_trivial<FromNix32>("nyvv6"), std::runtime_error);
}

}  // namespace textencode
