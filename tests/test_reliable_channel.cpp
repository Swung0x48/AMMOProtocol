#ifndef AMMOPROTOCOL_RELIABLE_CHANNEL_TEST_HPP
#define AMMOPROTOCOL_RELIABLE_CHANNEL_TEST_HPP

#include <gtest/gtest.h>
#include "AMMOProtocol.hpp"
using namespace ammo;

TEST(ReliableChannel, BasicAssertions) {
    event::event_handler handler_a;
    event::event_handler handler_b;
    network::reliable_channel<uint32_t> channel_a(handler_a);
    network::reliable_channel<uint32_t> channel_b(handler_b);

    handler_a
        .on<event::connection_send_event<uint32_t>>(
            [&](event::connection_send_event<uint32_t>& e) {
                std::cout << "a send: " << e.get_message().header.sequence << std::endl;
                channel_b.on_receive(e.get_message());
            })
        .on<event::connection_on_message_event<uint32_t>>(
            [&](event::connection_on_message_event<uint32_t>& e) {
                std::cout << "a rcvd: " << e.get_message().header.sequence << std::endl;
        });

    handler_b
            .on<event::connection_send_event<uint32_t>>(
                    [&](event::connection_send_event<uint32_t>& e) {
                        std::cout << "b send: " << e.get_message().header.sequence << std::endl;
                        channel_a.on_receive(e.get_message());
                    })
            .on<event::connection_on_message_event<uint32_t>>(
                    [&](event::connection_on_message_event<uint32_t>& e) {
                        std::cout << "b rcvd: " << e.get_message().header.sequence << std::endl;
                        channel_b.submit_for_send(e.get_message());
                    });


    common::message<uint32_t> msg;
    std::cout << "a: submit send" << std::endl;

    for (int i = 0; i < 10; ++i) {
        channel_a.submit_for_send(msg);
    }

    for (int i = 0; i < 10; ++i) {
        channel_a.on_update();
    }

    std::cout << "b update" << std::endl;
    channel_b.on_update();

}


#endif //AMMOPROTOCOL_RELIABLE_CHANNEL_TEST_HPP
