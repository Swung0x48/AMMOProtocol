#ifndef AMMOPROTOCOL_CONNECTION_HPP
#define AMMOPROTOCOL_CONNECTION_HPP

#include <event/role_send_event.hpp>
#include "event/on_message_event.hpp"
#include "network.hpp"
#include "channel.hpp"
#include "reliable_channel.hpp"
#include <memory>
namespace ammo::network {
    template<typename T>
    class connection {
    public:
        explicit connection(asio::ip::udp::endpoint endpoint, event::event_handler& event_handler):
            endpoint_(std::move(endpoint)),
            main_event_handler_(event_handler),
            channel_(event_handler_),
            reliable_channel_(event_handler_) {
            event_handler_
                .on<event::connection_on_message_event<T>>(
                    [this](event::connection_on_message_event<T>& e) {
                        on_message(e.get_message());
                    })
            .template on<event::connection_send_event<T>>(
                    [this](event::connection_send_event<T>& e) {
                        event::role_send_event<T> ev(*this, e.get_message());
                        main_event_handler_.emit(ev);
                    });
        }

        // To channel
        void on_receive(common::message<T>& msg) {
            // route message to corresponding channel
            if (msg.is_reliable())
                reliable_channel_.on_receive(msg);
            else
                channel_.on_receive(msg);
        }

        // To channel
        void on_update() {
            reliable_channel_.on_update();
        }

        // To channel
        void on_send(common::message<T>& msg) {
            if (msg.is_reliable())
                reliable_channel_.send(msg);
            else
                channel_.send(msg);
        }

        // To role
        void on_message(common::message<T>& msg) {
            event::on_message_event e(*this, msg);
            main_event_handler_.emit(e);
        }

        asio::ip::udp::endpoint& get_remote() {
            return endpoint_;
        }
    private:
        event::event_handler event_handler_;
        asio::ip::udp::endpoint endpoint_;
        event::event_handler& main_event_handler_;
        channel<T> channel_;
        reliable_channel<T> reliable_channel_;
    };
}

#endif //AMMOPROTOCOL_CONNECTION_HPP
