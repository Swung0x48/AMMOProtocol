#include <gtest/gtest.h>
#include "common.hpp"

TEST(SanityCheck, BasicAssertions) {
    // Expect two strings not to be equal.
    ASSERT_STRNE("Hello", "World");
    // Expect equality.
    ASSERT_EQ(7 * 6, 42);
}