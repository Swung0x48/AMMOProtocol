#ifndef AMMOPROTOCOL_ON_MESSAGE_EVENT_HPP
#define AMMOPROTOCOL_ON_MESSAGE_EVENT_HPP

namespace ammo::network {
    template <typename T>
    class connection;
}

namespace ammo::event {
    template <typename T>
    class on_message_event {
    public:
        explicit on_message_event(
                std::shared_ptr<network::connection<T>> connection,
                ammo::common::message<T>& msg):
            connection_(connection),
            message_(msg) {}

        ammo::common::message<T>& get_message() {
            return message_;
        }

        std::shared_ptr<network::connection<T>> get_connection() {
            return connection_;
        }

        std::shared_ptr<ammo::network::connection<T>> connection_;
        ammo::common::message<T>& message_;
    };
}

#endif //AMMOPROTOCOL_ON_MESSAGE_EVENT_HPP
