#include <gtest/gtest.h>
#include "../AMMOProtocol.hpp"

TEST(ModuloQueue, BasicAssertions) {
    constexpr size_t size = 20;
    ammo::structure::modulo_queue<int, size> queue;
    for (auto i: queue) {
        EXPECT_FALSE(i);
    }

    queue[size + 1] = 1;
    EXPECT_TRUE(queue[size + 1]);
    EXPECT_EQ(queue[size + 1], 1);
    EXPECT_TRUE(queue[1]);
    EXPECT_EQ(queue[1], 1);

    for (int i = 0; i < size / 2; ++i) {
        queue[i] = i;
    }

    for (int j = 0; j < 10000; ++j) {
        for (size_t i = 0 + j * queue.size(); i < queue.size() / 2 + j * queue.size(); ++i) {
            EXPECT_TRUE(queue[i]);
            EXPECT_EQ(queue[i], i % queue.size());
        }
        for (size_t i = queue.size() / 2 + j * queue.size(); i < queue.size() + j * queue.size(); ++i) {
            EXPECT_FALSE(queue[i]);
        }
    }
}