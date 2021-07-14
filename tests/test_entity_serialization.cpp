#include <gtest/gtest.h>
#include "../AMMOProtocol.hpp"
#include "common.hpp"

TEST(Vec3Serialization, BasicAssertions) {
    ammo::common::packet<PacketType> packet;

    ammo::entity::vec3<PacketType> expected;
    expected.x = 1.0f;
    expected.y = 2.0f;
    expected.z = 3.0f;
    expected.serialize(packet);
    packet.pack();

    ASSERT_TRUE(packet.unpack()) << "Failed on packet unpack.";
    ammo::entity::vec3<PacketType> actual;
    actual.deserialize(packet);
    ASSERT_EQ(expected.x, actual.x);
    ASSERT_EQ(expected.y, actual.y);
    ASSERT_EQ(expected.z, actual.z);
}

TEST(QuaternionSerialization, BasicAssertions) {
    ammo::common::packet<PacketType> packet;

    ammo::entity::quaternion<PacketType> expected;
    expected.x = 1.0f;
    expected.y = 2.0f;
    expected.z = 3.0f;
    expected.w = 4.0f;
    expected.serialize(packet);
    packet.pack();

    ASSERT_TRUE(packet.unpack()) << "Failed on packet unpack.";
    ammo::entity::quaternion<PacketType> actual;
    actual.deserialize(packet);
    ASSERT_EQ(expected.x, actual.x);
    ASSERT_EQ(expected.y, actual.y);
    ASSERT_EQ(expected.z, actual.z);
    ASSERT_EQ(expected.w, actual.w);
}

TEST(MixedSerialization, BasicAssertions) {
    ammo::common::packet<PacketType> packet;

    ammo::entity::vec3<PacketType> expected_vec3;
    expected_vec3.x = 1.0f;
    expected_vec3.y = 2.0f;
    expected_vec3.z = 3.0f;
    expected_vec3.serialize(packet);
    ammo::entity::quaternion<PacketType> expected_quaternion;
    expected_quaternion.x = 4.0f;
    expected_quaternion.y = 5.0f;
    expected_quaternion.z = 6.0f;
    expected_quaternion.w = 7.0f;
    expected_quaternion.serialize(packet);
    packet.pack();

    ASSERT_TRUE(packet.unpack()) << "Failed on packet unpack.";
    ammo::entity::vec3<PacketType> actual_vec3;
    ammo::entity::quaternion<PacketType> actual_quaternion;
    actual_vec3.deserialize(packet);
    actual_quaternion.deserialize(packet);
    ASSERT_EQ(expected_vec3.x, actual_vec3.x);
    ASSERT_EQ(expected_vec3.y, actual_vec3.y);
    ASSERT_EQ(expected_vec3.z, actual_vec3.z);
    ASSERT_EQ(expected_quaternion.x, actual_quaternion.x);
    ASSERT_EQ(expected_quaternion.y, actual_quaternion.y);
    ASSERT_EQ(expected_quaternion.z, actual_quaternion.z);
    ASSERT_EQ(expected_quaternion.w, actual_quaternion.w);
}