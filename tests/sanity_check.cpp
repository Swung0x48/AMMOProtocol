#include <gtest/gtest.h>

TEST(SanityCheck, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("Hello", "World");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}