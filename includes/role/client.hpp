#ifndef AMMOPROTOCOL_CLIENT_HPP
#define AMMOPROTOCOL_CLIENT_HPP

#include "role.hpp"

namespace ammo::role {
    template<typename T>
    class client: public role<T> {
    public:
        explicit client(): role<T>() {}

        bool connect(const std::string& host, const uint16_t port) {
            try {
                asio::ip::udp::resolver resolver(role<T>::io_context_);
                server_endpoint_ = *resolver.resolve(host, std::to_string(port)).begin();
//                role<T>::connections_.emplace(server_endpoint_, std::make_shared<network::connection<T>>(server_endpoint_, role<T>::event_handler_));
                if (!role<T>::start())
                    return false;
                connect_to_server();
            } catch (std::exception& e) {
                std::cerr << "Client exception: " << e.what() << std::endl;
                return false;
            }

            return true;
        }

        std::shared_ptr<network::connection<T>> get_server_connection() {
            return role<T>::connections_[server_endpoint_];
        }

        virtual void connect_to_server() {
            client_state_ = client_state::Pending;
            send_request();
        }

        virtual void send_request() = 0;

        void confirm_validation() {
            client_state_ = client_state::Connected;
            role<T>::accept_connection(server_endpoint_);
        }

        virtual void disconnect() {
            client_state_ = ammo::role::client_state::Disconnected;
        }

        client_state get_state() {
            return client_state_;
        }

        bool connected() {
            return client_state_ == client_state::Connected;
        }
    protected:
        asio::ip::udp::endpoint server_endpoint_;
        client_state client_state_ = client_state::Disconnected;
        uint32_t sequence_ = 0u;
    };
}
#endif //AMMOPROTOCOL_CLIENT_HPP
