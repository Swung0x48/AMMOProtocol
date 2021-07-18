#ifndef AMMOPROTOCOL_CLIENT_HPP
#define AMMOPROTOCOL_CLIENT_HPP
namespace ammo::role {
    template<typename T>
    class client {
    protected:
        enum class client_state {
            Disconnected,
            Pending,
            Connected
        };
        asio::io_context io_context_;
        std::thread ctx_thread_;
        asio::ip::udp::socket socket_;
        ammo::network::receiver<T> receiver_;
        ammo::network::sender<T> sender_;
        asio::ip::udp::endpoint server_endpoint_;
        client_state client_state_ = client_state::Disconnected;
    private:
        ammo::structure::ts_queue<ammo::common::owned_message<T>> incoming_messages_;
        ammo::structure::ts_queue<ammo::common::owned_message<T>> outgoing_messages_;

        // async
        void connect_to_server(const asio::ip::udp::endpoint& endpoint) {
            server_endpoint_ = endpoint;
            client_state_ = client_state::Connected;
        }
    public:
        client():
        socket_(io_context_),
        receiver_(socket_, incoming_messages_),
        sender_(io_context_, socket_, outgoing_messages_) {
            socket_.open(asio::ip::udp::v4());
        }

        bool connect(const std::string& host, const uint16_t port) {
            try {
                asio::ip::udp::resolver resolver(io_context_);
                auto endpoint = *resolver.resolve(host, std::to_string(port)).begin();
                connect_to_server(endpoint);
                receiver_.start_receiving();
                sender_.send_validation(endpoint);

                ctx_thread_ = std::thread([this]() { io_context_.run(); });
            } catch (std::exception& e) {
                std::cerr << "Client exception: " << e.what() << std::endl;
                return false;
            }

            return true;
        }

        void send(const ammo::common::message<T>& msg) {
            ammo::common::owned_message<T> owned_message = { server_endpoint_, std::move(msg) };
            sender_.send(owned_message);
        }

        void disconnect() {
            io_context_.stop();
            if (ctx_thread_.joinable())
                ctx_thread_.join();
        }

        ammo::structure::ts_queue<ammo::common::owned_message<T>>& get_incoming_messages() {
            return incoming_messages_;
        }

        bool connected() {
            return client_state_ == client_state::Connected;
        }
    };
}
#endif //AMMOPROTOCOL_CLIENT_HPP
