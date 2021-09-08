#ifndef AMMOPROTOCOL_SEND_EVENT_HPP
#define AMMOPROTOCOL_SEND_EVENT_HPP

#include "message.hpp"

namespace ammo::event {
    template <typename T>
    class connection_send_event {
    public:
        explicit connection_send_event(ammo::common::message<T>& message):
            message_(message) {}

        ammo::common::message<T>& get_message() {
            return message_;
        }

        ammo::common::message<T>& message_;
    };
}

#endif //AMMOPROTOCOL_SEND_EVENT_HPP
