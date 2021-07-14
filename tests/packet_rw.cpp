#include <gtest/gtest.h>
#include "../AMMOProtocol.hpp"
#include "common.hpp"
#include <random>

TEST(PacketReadWrite, BasicAssertions) {
    ammo::common::packet<PacketType> packet;

    auto now = std::chrono::high_resolution_clock::now();
    std::default_random_engine e(now.time_since_epoch().count());
    unsigned int random[10000];
    for (unsigned int& i: random) {
        i = e();
    }

    for (auto i: random) {
        packet << i;
    }
    packet.pack();

    ASSERT_TRUE(packet.unpack()) << "Failed on packet unpack.";
    for (unsigned int & i: random) {
        int temp;
        packet >> temp;
        ASSERT_EQ(temp, i) << "Item #" << i << " is not equal to expected.";
    }
}