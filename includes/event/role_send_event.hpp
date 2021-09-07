#ifndef AMMOPROTOCOL_SERVER_SEND_EVENT_HPP
#define AMMOPROTOCOL_SERVER_SEND_EVENT_HPP

namespace ammo::event {
    template <typename T>
    class role_send_event {
    public:
        explicit role_send_event(
                std::shared_ptr<network::connection<T>> connection,
                ammo::common::message<T>& msg):
                connection_(connection),
                message_(msg) {}

        const ammo::common::owned_message<T>& get_message() {
            return message_;
        }

        std::shared_ptr<network::connection<T>> get_connection() {
            return connection_;
        }

        ammo::common::owned_message<T>& message_;
        std::shared_ptr<ammo::network::connection<T>> connection_;
    };
}

#endif //AMMOPROTOCOL_SERVER_SEND_EVENT_HPP
