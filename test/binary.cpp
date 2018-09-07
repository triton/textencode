#include <gtest/gtest.h>
#include <string_view>
#include <textencode/binary.hpp>

namespace textencode {

TEST(BinaryTest, NoInput) {
    EXPECT_EQ("", Binary().complete());

    Binary b;
    EXPECT_EQ("", b.process(""));
    EXPECT_EQ("", b.complete());
}

TEST(BinaryTest, NoBuffer) {
    constexpr std::string_view str1 = "abc";
    constexpr std::string_view str2 = "defghijk";

    Binary b;
    EXPECT_EQ(str1, b.process(str1));
    EXPECT_EQ(str2, b.process(str2));
    EXPECT_EQ("", b.complete());
}

}  // namespace textencode
