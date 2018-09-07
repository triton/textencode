#include <gtest/gtest.h>
#include <textencode/internal/base_n.hpp>
#include <textencode/internal/common.hpp>

namespace textencode::internal
{

TEST(InternalBaseNTest, Base16Symbols)
{
	const auto symbols = Properties<EncodingType::Base16>::symbols;
	EXPECT_EQ(16, symbols.size());
	EXPECT_EQ('2', symbols[2]);
	EXPECT_EQ('9', symbols[9]);
	EXPECT_EQ('C', symbols[12]);
}

TEST(InternalBaseNTest, Base16Common)
{
	using Common = Common<EncodingType::Base16>;
	EXPECT_EQ(4, Common::shift);
	EXPECT_EQ(8, Common::quantum_bits);
	EXPECT_EQ(2, Common::quantum_symbols);

	EXPECT_TRUE(Common::validByte(0x1));
	EXPECT_TRUE(Common::validByte(0x0f));
	EXPECT_FALSE(Common::validByte(0x10));
	EXPECT_FALSE(Common::validByte(static_cast<char>(CharCodes::Invalid)));

	EXPECT_EQ(static_cast<int>(CharCodes::Invalid), Common::inverse['A' - 1]);
	EXPECT_EQ(12, Common::inverse['c']);
	EXPECT_EQ(12, Common::inverse['C']);
	EXPECT_EQ(8, Common::inverse['8']);
	EXPECT_EQ(static_cast<int>(CharCodes::Ignore), Common::inverse[' ']);
	EXPECT_EQ(static_cast<int>(CharCodes::Padding), Common::inverse['=']);
}

TEST(InternalBaseNTest, Base32Symbols)
{
	const auto symbols = Properties<EncodingType::Base32>::symbols;
	EXPECT_EQ(32, symbols.size());
	EXPECT_EQ('B', symbols[1]);
	EXPECT_EQ('Z', symbols[25]);
	EXPECT_EQ('6', symbols[30]);
}

TEST(InternalBaseNTest, Base32Common)
{
	using Common = Common<EncodingType::Base32>;
	EXPECT_EQ(5, Common::shift);
	EXPECT_EQ(40, Common::quantum_bits);
	EXPECT_EQ(8, Common::quantum_symbols);

	EXPECT_TRUE(Common::validByte(0x1));
	EXPECT_TRUE(Common::validByte(0x1f));
	EXPECT_FALSE(Common::validByte(0x20));
	EXPECT_FALSE(Common::validByte(static_cast<char>(CharCodes::Invalid)));

	EXPECT_EQ(static_cast<int>(CharCodes::Invalid), Common::inverse['A' - 1]);
	EXPECT_EQ(25, Common::inverse['z']);
	EXPECT_EQ(25, Common::inverse['Z']);
	EXPECT_EQ(31, Common::inverse['7']);
	EXPECT_EQ(static_cast<int>(CharCodes::Invalid), Common::inverse['1']);
	EXPECT_EQ(static_cast<int>(CharCodes::Invalid), Common::inverse['+']);
	EXPECT_EQ(static_cast<int>(CharCodes::Ignore), Common::inverse['\n']);
	EXPECT_EQ(static_cast<int>(CharCodes::Padding), Common::inverse['=']);
}

TEST(InternalBaseNTest, Base64Symbols)
{
	const auto symbols = Properties<EncodingType::Base64>::symbols;
	EXPECT_EQ(64, symbols.size());
	EXPECT_EQ('B', symbols[1]);
	EXPECT_EQ('f', symbols[31]);
	EXPECT_EQ('2', symbols[54]);
	EXPECT_EQ('+', symbols[62]);
}

TEST(InternalBaseNTest, Base64Common)
{
	using Common = Common<EncodingType::Base64>;
	EXPECT_EQ(6, Common::shift);
	EXPECT_EQ(24, Common::quantum_bits);
	EXPECT_EQ(4, Common::quantum_symbols);

	EXPECT_TRUE(Common::validByte(0x1));
	EXPECT_TRUE(Common::validByte(0x3f));
	EXPECT_FALSE(Common::validByte(0x40));
	EXPECT_FALSE(Common::validByte(static_cast<char>(CharCodes::Invalid)));

	EXPECT_EQ(static_cast<int>(CharCodes::Invalid), Common::inverse['A' - 1]);
	EXPECT_EQ(2, Common::inverse['C']);
	EXPECT_EQ(28, Common::inverse['c']);
	EXPECT_EQ(60, Common::inverse['8']);
	EXPECT_EQ(62, Common::inverse['+']);
	EXPECT_EQ(static_cast<int>(CharCodes::Ignore), Common::inverse['\r']);
	EXPECT_EQ(static_cast<int>(CharCodes::Padding), Common::inverse['=']);
}

} // namespace textencode::internal
