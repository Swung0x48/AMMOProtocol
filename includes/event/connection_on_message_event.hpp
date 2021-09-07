#ifndef AMMOPROTOCOL_CONNECTION_ON_MESSAGE_EVENT_HPP
#define AMMOPROTOCOL_CONNECTION_ON_MESSAGE_EVENT_HPP

namespace ammo::event {
    template <typename T>
    class connection_on_message_event {
    public:
        explicit connection_on_message_event(
                ammo::common::message<T>& msg):
                message_(msg) {}

        ammo::common::message<T>& get_message() {
            return message_;
        }

        ammo::common::message<T>& message_;
    };
}

#endif //AMMOPROTOCOL_CONNECTION_ON_MESSAGE_EVENT_HPP
