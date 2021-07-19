#ifndef AMMOPROTOCOL_CLIENT_HPP
#define AMMOPROTOCOL_CLIENT_HPP
namespace ammo::role {
    enum class client_state {
        Disconnected,
        Pending,
        Connected
    };
    template<typename T>
    class client {
    protected:
        asio::io_context io_context_;
        std::thread ctx_thread_;
        asio::ip::udp::socket socket_;
        ammo::network::receiver<T> receiver_;
        ammo::network::sender<T> sender_;
        asio::ip::udp::endpoint server_endpoint_;
        client_state client_state_ = client_state::Disconnected;
        uint16_t sequence_ = 0u;
    private:
        ammo::structure::ts_queue<ammo::common::owned_message<T>> incoming_messages_;
        ammo::structure::ts_queue<ammo::common::owned_message<T>> outgoing_messages_;

    protected:
        // async
        virtual void connect_to_server() {
            client_state_ = client_state::Pending;
            send_validation();
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
                server_endpoint_ = *resolver.resolve(host, std::to_string(port)).begin();
                connect_to_server();
//                sender_.send_validation(server_endpoint_);
                receiver_.start_receiving();
                ctx_thread_ = std::thread([this]() { io_context_.run(); });
            } catch (std::exception& e) {
                std::cerr << "Client exception: " << e.what() << std::endl;
                return false;
            }

            return true;
        }

        void send(ammo::common::message<T>& msg) {
            if (msg.is_packed())
                msg.unpack_and_verify();
            msg.header.sequence = sequence_++;
            msg.pack();
            ammo::common::owned_message<T> owned_message = { server_endpoint_, std::move(msg) };
            sender_.send(owned_message);
        }

        virtual void send_validation() = 0;

        void confirm_validation() {
            client_state_ = client_state::Connected;
        }

        void disconnect() {
            io_context_.stop();
            if (ctx_thread_.joinable())
                ctx_thread_.join();
        }

        ammo::structure::ts_queue<ammo::common::owned_message<T>>& get_incoming_messages() {
            return incoming_messages_;
        }

        client_state get_state() {
            return client_state_;
        }

        bool connected() {
            return client_state_ == client_state::Connected;
        }
    };
}
#endif //AMMOPROTOCOL_CLIENT_HPP
