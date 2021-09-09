#ifndef AMMOPROTOCOL_SERVER_SEND_EVENT_HPP
#define AMMOPROTOCOL_SERVER_SEND_EVENT_HPP

namespace ammo::network {
    template <typename T>
    class connection;
}

namespace ammo::event {
    template <typename T>
    class role_send_event {
    public:
        explicit role_send_event(
                network::connection<T>& connection,
                ammo::common::message<T>& msg):
                connection_(connection),
                message_(msg) {
            std::cout << "role_send_event ctor" << std::endl;
        }

        const ammo::common::message<T>& get_message() {
            return message_;
        }

        network::connection<T>& get_connection() {
            return connection_;
        }

        common::message<T>& message_;
        network::connection<T>& connection_;
    };
}

#endif //AMMOPROTOCOL_SERVER_SEND_EVENT_HPP
