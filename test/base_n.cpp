#include <gtest/gtest.h>
#include <stdexcept>
#include <string_view>
#include <textencode/base_n.hpp>

#include "common.hpp"

namespace textencode
{

TEST(Base16Test, NoInputTo)
{
	EXPECT_EQ("", ToBase16().complete());
	EXPECT_EQ("", encode_trivial<ToBase16>(""));
}

TEST(Base16Test, NoInputFrom)
{
	EXPECT_EQ("", FromBase16().complete());
	EXPECT_EQ("", encode_trivial<FromBase16>(""));
}

TEST(Base16Test, MinimalBufferTo)
{
	std::string expected;
	std::string output;
	ToBase16 b;

	expected += "10F513";
	output += b.process("\x10\xF5\x13");
	EXPECT_LE(4, output.size());

	expected += "20C6";
	output += b.process("\x20\xC6");
	EXPECT_LE(8, output.size());

	output += b.complete();
	EXPECT_EQ(expected, output);
}

TEST(Base16Test, MinimalBufferFrom)
{
	std::string expected;
	std::string output;
	FromBase16 b;

	expected += "\x10\xFC";
	output += b.process("10f");
	EXPECT_EQ(1, output.size());
	expected += "\x86\xE4";
	output += b.process("c86e4");
	EXPECT_LE(3, output.size());

	output += b.complete();
	EXPECT_EQ(expected, output);
}

TEST(Base16Test, ToRFC4648)
{
	EXPECT_EQ("66", encode_trivial<ToBase16>("f"));
	EXPECT_EQ("666F", encode_trivial<ToBase16>("fo"));
	EXPECT_EQ("666F6F", encode_trivial<ToBase16>("foo"));
	EXPECT_EQ("666F6F62", encode_trivial<ToBase16>("foob"));
	EXPECT_EQ("666F6F6261", encode_trivial<ToBase16>("fooba"));
	EXPECT_EQ("666F6F626172", encode_trivial<ToBase16>("foobar"));
}

TEST(Base16Test, FromRFC4648)
{
	EXPECT_EQ("f", encode_trivial<FromBase16>("66"));
	EXPECT_EQ("fo", encode_trivial<FromBase16>("666F"));
	EXPECT_EQ("foo", encode_trivial<FromBase16>("666f6f"));
	EXPECT_EQ("foob", encode_trivial<FromBase16>("666F6F62"));
	EXPECT_EQ("fooba", encode_trivial<FromBase16>("666F6F6261"));
	EXPECT_EQ("foobar", encode_trivial<FromBase16>("666F6f626172"));
}

TEST(Base16Test, IgnoreCharacters)
{
	EXPECT_EQ("\xA7\xDE\xF6", encode_trivial<FromBase16>("a7\nd\r ef6\n"));
}

TEST(Base16Test, BadShortInput)
{
	EXPECT_THROW(encode_trivial<FromBase16>("a"), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase16>("AbCde"), std::runtime_error);
}

TEST(Base16Test, BadCharacters)
{
	EXPECT_THROW(FromBase16().process("Z"), std::runtime_error);
	EXPECT_THROW(FromBase16().process("g"), std::runtime_error);
	EXPECT_THROW(FromBase16().process("+"), std::runtime_error);
}

TEST(Base16Test, BadPadding)
{
	EXPECT_THROW(FromBase16().process("1=0"), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase16>("="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase16>("=="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase16>("abc="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase16>("abcd="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase16>("abcd=="), std::runtime_error);
}

TEST(Base32Test, NoInputTo)
{
	EXPECT_EQ("", ToBase32().complete());
	EXPECT_EQ("", encode_trivial<ToBase32>(""));
}

TEST(Base32Test, NoInputFrom)
{
	EXPECT_EQ("", FromBase32().complete());
	EXPECT_EQ("", encode_trivial<FromBase32>(""));
}

TEST(Base32Test, MinimalBufferTo)
{
	std::string expected;
	std::string output;
	ToBase32 b;

	expected += "CD2SBRUP";
	output += b.process("\x10\xF5");
	output += b.process("\x20\xC6\x8F");

	expected += "KR3O6MQJCE======";
	output += b.process("\x54");
	EXPECT_LE(8, output.size());
	output += b.process("\x76\xEF\x32\x09\x11");
	EXPECT_LE(16, output.size());

	output += b.complete();
	EXPECT_EQ(expected, output);
}

TEST(Base32Test, MinimalBufferFrom)
{
	std::string expected;
	std::string output;
	FromBase32 b;

	expected += "\x10\xF5\x20\xC6\x8F";
	output += b.process("CD2sB");
	output += b.process("RUP");

	expected += "\x54\x76\xEF\x32\x09\x11";
	output += b.process("K");
	EXPECT_LE(5, output.size());
	output += b.process("R3O6mQJCE======");
	EXPECT_LE(10, output.size());

	output += b.complete();
	EXPECT_EQ(expected, output);
}

TEST(Base32Test, ToRFC4648)
{
	EXPECT_EQ("MY======", encode_trivial<ToBase32>("f"));
	EXPECT_EQ("MZXQ====", encode_trivial<ToBase32>("fo"));
	EXPECT_EQ("MZXW6===", encode_trivial<ToBase32>("foo"));
	EXPECT_EQ("MZXW6YQ=", encode_trivial<ToBase32>("foob"));
	EXPECT_EQ("MZXW6YTB", encode_trivial<ToBase32>("fooba"));
	EXPECT_EQ("MZXW6YTBOI======", encode_trivial<ToBase32>("foobar"));
}

TEST(Base32Test, FromRFC4648)
{
	EXPECT_EQ("f", encode_trivial<FromBase32>("MY======"));
	EXPECT_EQ("fo", encode_trivial<FromBase32>("MZXq===="));
	EXPECT_EQ("foo", encode_trivial<FromBase32>("MZXW6==="));
	EXPECT_EQ("foob", encode_trivial<FromBase32>("MZxW6YQ="));
	EXPECT_EQ("fooba", encode_trivial<FromBase32>("mzXW6YTB"));
	EXPECT_EQ("foobar", encode_trivial<FromBase32>("MZXW6YTBOI======"));
}

TEST(Base32Test, IgnoreCharacters)
{
	EXPECT_EQ("foo", encode_trivial<FromBase32>("Mz\nX W6 \r= ==\n"));
}

TEST(Base32Test, BadShortInput)
{
	EXPECT_THROW(encode_trivial<FromBase32>("MzXw6"), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase32>("MZXW6=="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase32>("mzXW6YTBO"), std::runtime_error);
}

TEST(Base32Test, BadCharacters)
{
	EXPECT_THROW(FromBase32().process("1"), std::runtime_error);
	EXPECT_THROW(FromBase32().process("9"), std::runtime_error);
	EXPECT_THROW(FromBase32().process("+"), std::runtime_error);
}

TEST(Base32Test, BadPaddingInline)
{
	EXPECT_THROW(FromBase32().process("MZXW=6=="), std::runtime_error);
}

TEST(Base32Test, BadPaddingSuperfluous)
{
	EXPECT_THROW(encode_trivial<FromBase32>("="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase32>("========"), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase32>("mzXW6YTB="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase32>("mzXW6YTB======="), std::runtime_error);
}

TEST(Base32Test, IllegalEncoding)
{
	EXPECT_THROW(encode_trivial<FromBase32>("M======="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase32>("MZA====="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase32>("MZxW6A=="), std::runtime_error);
}

TEST(Base32Test, BadZeroEncoding)
{
	// Mutation of valid MZXW6===
	EXPECT_THROW(encode_trivial<FromBase32>("MZXW7==="), std::runtime_error);
	// Mutations of valid MZxW6YQ=
	EXPECT_THROW(encode_trivial<FromBase32>("MZxW6YR="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase32>("MZxW6YU="), std::runtime_error);
}

TEST(Base64Test, NoInputTo)
{
	EXPECT_EQ("", ToBase64().complete());
	EXPECT_EQ("", encode_trivial<ToBase64>(""));
}

TEST(Base64Test, NoInputFrom)
{
	EXPECT_EQ("", FromBase64().complete());
	EXPECT_EQ("", encode_trivial<FromBase64>(""));
}

TEST(Base64Test, MinimalBufferTo)
{
	std::string expected;
	std::string output;
	ToBase64 b;

	expected += "EPUg";
	output += b.process("\x10\xF5");
	output += b.process("\x20");

	expected += "VHbvAg==";
	output += b.process("\x54");
	EXPECT_LE(4, output.size());
	output += b.process("\x76\xEF\x02");
	EXPECT_LE(8, output.size());

	output += b.complete();
	EXPECT_EQ(expected, output);
}

TEST(Base64Test, MinimalBufferFrom)
{
	std::string expected;
	std::string output;
	FromBase64 b;

	expected += "\x10\xF5\x20";
	output += b.process("EPU");
	output += b.process("g");

	expected += "\x54\x76\xEF\x02";
	output += b.process("V");
	EXPECT_LE(3, output.size());
	output += b.process("HbvAg==");
	EXPECT_LE(6, output.size());

	output += b.complete();
	EXPECT_EQ(expected, output);
}

TEST(Base64Test, ToRFC4648)
{
	EXPECT_EQ("Zg==", encode_trivial<ToBase64>("f"));
	EXPECT_EQ("Zm8=", encode_trivial<ToBase64>("fo"));
	EXPECT_EQ("Zm9v", encode_trivial<ToBase64>("foo"));
	EXPECT_EQ("Zm9vYg==", encode_trivial<ToBase64>("foob"));
	EXPECT_EQ("Zm9vYmE=", encode_trivial<ToBase64>("fooba"));
	EXPECT_EQ("Zm9vYmFy", encode_trivial<ToBase64>("foobar"));
}

TEST(Base64Test, FromRFC4648)
{
	EXPECT_EQ("f", encode_trivial<FromBase64>("Zg=="));
	EXPECT_EQ("fo", encode_trivial<FromBase64>("Zm8="));
	EXPECT_EQ("foo", encode_trivial<FromBase64>("Zm9v"));
	EXPECT_EQ("foob", encode_trivial<FromBase64>("Zm9vYg=="));
	EXPECT_EQ("fooba", encode_trivial<FromBase64>("Zm9vYmE="));
	EXPECT_EQ("foobar", encode_trivial<FromBase64>("Zm9vYmFy"));
}

TEST(Base64Test, IgnoreCharacters)
{
	EXPECT_EQ("foob", encode_trivial<FromBase64>("Zm\n9vY g\r= =\n"));
}

TEST(Base64Test, BadShortInput)
{
	EXPECT_THROW(encode_trivial<FromBase64>("Zm8"), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase64>("Zg="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase64>("Zm9vY"), std::runtime_error);
}

TEST(Base64Test, BadCharacters)
{
	EXPECT_THROW(FromBase64().process("-"), std::runtime_error);
	EXPECT_THROW(FromBase64().process("_"), std::runtime_error);
}

TEST(Base64Test, BadPaddingInline)
{
	EXPECT_THROW(FromBase64().process("Zm=8"), std::runtime_error);
}

TEST(Base64Test, BadPaddingSuperfluous)
{
	EXPECT_THROW(encode_trivial<FromBase64>("="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase64>("===="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase64>("Zm9v="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase64>("Zm9v===="), std::runtime_error);
}

TEST(Base64Test, IllegalEncoding)
{
	EXPECT_THROW(encode_trivial<FromBase64>("A==="), std::runtime_error);
}

TEST(Base64Test, BadZeroEncoding)
{
	// Mutation of valid Zg==
	EXPECT_THROW(encode_trivial<FromBase64>("Zo=="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase64>("Zh=="), std::runtime_error);
	// Mutations of valid Zm8=
	EXPECT_THROW(encode_trivial<FromBase64>("Zm9="), std::runtime_error);
	EXPECT_THROW(encode_trivial<FromBase64>("Zm+="), std::runtime_error);
}


} // namespace textencode
