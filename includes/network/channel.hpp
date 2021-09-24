#ifndef AMMOPROTOCOL_CHANNEL_HPP
#define AMMOPROTOCOL_CHANNEL_HPP

#include <event/connection_on_message_event.hpp>
#include <event/connection_send_event.hpp>
#include "network.hpp"
namespace ammo::network {
    template<typename T>
    class channel {
    public:
        channel() = delete;
        explicit channel(event::event_handler& handler):
                event_handler_(handler) {}

        void submit_for_send(ammo::common::message<T>& msg) {
            on_send(msg);

            event::connection_send_event e(msg);
            event_handler_.emit(e);
        }

        virtual void on_receive(ammo::common::message<T>& msg) {
            this->on_message(msg);
        }

        virtual void on_update() {}

    protected:
        virtual void on_send(ammo::common::message<T>& msg) {}

        virtual void on_message(ammo::common::message<T>& msg) {
            event::connection_on_message_event e(msg);
            event_handler_.emit(e);
        }
        event::event_handler& event_handler_;
    };
}

#endif //AMMOPROTOCOL_CHANNEL_HPP
