#undef NDEBUG

#include <gtest/gtest.h>
#include <textencode/internal/utils.hpp>

namespace textencode::internal
{

TEST(InternalUtilsTest, SizeToShift0)
{
	EXPECT_DEATH(sizeToShift(0), "");
}

TEST(InternalUtilsTest, SizeToShiftInvalid)
{
	EXPECT_DEATH(sizeToShift(3), "");
	EXPECT_DEATH(sizeToShift(50), "");
}

TEST(InternalUtilsTest, SizeToShiftSuccess)
{
	EXPECT_EQ(0, sizeToShift(1));
	EXPECT_EQ(2, sizeToShift(4));
	EXPECT_EQ(5, sizeToShift(32));
}

TEST(InternalUtilsTest, LcmInvalid)
{
	EXPECT_DEATH(lcm(0, 5), "");
	EXPECT_DEATH(lcm(0, 0), "");
	EXPECT_DEATH(lcm(0, 0), "");
}

TEST(InternalUtilsTest, LcmSuccess)
{
	EXPECT_EQ(8, lcm(8, 8));
	EXPECT_EQ(24, lcm(8, 6));
	EXPECT_EQ(40, lcm(5, 8));
	EXPECT_EQ(12, lcm(3, 4));
}

} // namespace textencode::internal
