#include <gtest/gtest.h>
#include <textencode/internal/common.hpp>
#include <textencode/internal/nix.hpp>

namespace textencode::internal {

TEST(InternalNixTest, Base32Symbols) {
    const auto symbols = Properties<EncodingType::Nix32>::symbols;
    EXPECT_EQ(32, symbols.size());
    EXPECT_EQ('6', symbols[6]);
    EXPECT_EQ('f', symbols[14]);
    EXPECT_EQ('x', symbols[29]);
}

TEST(InternalNixTest, Base32Common) {
    using Common = Common<EncodingType::Nix32>;
    EXPECT_EQ(5, Common::shift);
    EXPECT_EQ(40, Common::quantum_bits);
    EXPECT_EQ(8, Common::quantum_symbols);

    EXPECT_EQ(static_cast<int>(CharCodes::Invalid), Common::inverse['a' - 1]);
    EXPECT_EQ(static_cast<int>(CharCodes::Invalid), Common::inverse['E']);
    EXPECT_EQ(14, Common::inverse['f']);
    EXPECT_EQ(14, Common::inverse['F']);
    EXPECT_EQ(3, Common::inverse['3']);
    EXPECT_EQ(static_cast<int>(CharCodes::Ignore), Common::inverse['\r']);
    EXPECT_EQ(static_cast<int>(CharCodes::Padding), Common::inverse['=']);
}

}  // namespace textencode::internal
