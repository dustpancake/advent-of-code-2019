#include <gtest/gtest.h>

extern "C" {
#include "paintbot.c"
}

TEST(Movement, Directions) {
    ASSERT_EQ(
        pb_decode_direction(PB_UP, 1),
        PB_RIGHT
    );
    ASSERT_EQ(
        pb_decode_direction(PB_UP, 0),
        PB_LEFT
    );
    ASSERT_EQ(
        pb_decode_direction(PB_LEFT, 1),
        PB_UP
    );
    ASSERT_EQ(
        pb_decode_direction(PB_LEFT, 0),
        PB_DOWN
    );
    ASSERT_EQ(
        pb_decode_direction(PB_DOWN, 0),
        PB_RIGHT
    );
}

TEST(Movement, FullRight) {
    ASSERT_EQ(
        pb_decode_direction(PB_UP, 1),
        PB_RIGHT
    );
    ASSERT_EQ(
        pb_decode_direction(PB_RIGHT, 1),
        PB_DOWN
    );
    ASSERT_EQ(
        pb_decode_direction(PB_DOWN, 1),
        PB_LEFT
    );
    ASSERT_EQ(
        pb_decode_direction(PB_LEFT, 1),
        PB_UP
    );
}

TEST(Movement, FullLeft) {
    ASSERT_EQ(
        pb_decode_direction(PB_UP, 0),
        PB_LEFT
    );
    ASSERT_EQ(
        pb_decode_direction(PB_LEFT, 0),
        PB_DOWN
    );
    ASSERT_EQ(
        pb_decode_direction(PB_DOWN, 0),
        PB_RIGHT
    );
    ASSERT_EQ(
        pb_decode_direction(PB_RIGHT, 0),
        PB_UP
    );
}