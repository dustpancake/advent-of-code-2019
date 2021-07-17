#include <gtest/gtest.h>

extern "C" {
#include "day09_impl.c"
}

TEST(OutTest, Part1) {
    ASSERT_EQ(3518157894, part1());
}


TEST(OutTest, Part2) {
    ASSERT_EQ(80379, part2());
}