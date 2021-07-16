#include <gtest/gtest.h>
#include "common.hpp"
#include <random>

TEST(PacketReadWrite, BasicAssertions) {
    ammo::common::message<PacketType> message;

    auto now = std::chrono::high_resolution_clock::now();
    std::default_random_engine e(now.time_since_epoch().count());
    unsigned int random[255];
    for (unsigned int& i: random) {
        i = e();
    }

    for (auto i: random) {
        message << i;
    }
    ASSERT_TRUE(message.pack()) << "Failed on message pack. " << message.header.message_size << '/' << MAX_PACKET_SIZE;

    ASSERT_TRUE(message.unpack_and_verify()) << "Failed on message unpack_and_verify.";
    for (unsigned int & i: random) {
        int temp;
        message >> temp;
        ASSERT_EQ(temp, i) << "Item #" << i << " is not equal to expected.";
    }

    std::vector<uint8_t> data;
    data.resize(message.header.message_size + sizeof(message.header));
    std::memcpy(data.data(), &message.header, sizeof(message.header));
    std::memcpy(data.data() + sizeof(message.header), message.body.data(), message.body.size());

    ammo::common::message<PacketType> reconstructed_message(data.data(), data.size());
    ASSERT_TRUE(reconstructed_message.unpack_and_verify()) << "Failed on message unpack_and_verify.";
    for (unsigned int & i: random) {
        int temp;
        reconstructed_message >> temp;
        ASSERT_EQ(temp, i) << "Item #" << i << " is not equal to expected.";
    }
}