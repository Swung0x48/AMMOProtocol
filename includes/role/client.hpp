#ifndef AMMOPROTOCOL_CLIENT_HPP
#define AMMOPROTOCOL_CLIENT_HPP
namespace ammo::role {
    template<typename T>
    class client {
    protected:
        asio::io_context io_context_;
        std::thread ctx_thread_;
        asio::ip::udp::socket socket_;
        ammo::network::receiver<T> receiver_;
        ammo::network::sender<T> sender_;
    private:
        ammo::structure::ts_queue<ammo::common::owned_message<T>> incoming_messages_;
        ammo::structure::ts_queue<ammo::common::owned_message<T>> outgoing_messages_;

        // async
        void connect_to_server(const asio::ip::udp::resolver::results_type& endpoints) {
            std::atomic_int retry_count = 0;
            for (const auto& i: endpoints) {

            }
        }
    public:
        client():
            socket_(io_context_),
            receiver_(socket_, incoming_messages_),
            sender_(io_context_, socket_, outgoing_messages_) {

        }

        bool connect(const std::string& host, const uint16_t port) {
            try {
                asio::ip::udp::resolver resolver(io_context_);
                auto endpoints = resolver.resolve(host, std::to_string(port));
                connect_to_server(endpoints);
                ctx_thread_ = std::thread([this]() { io_context_.run(); });
            } catch (std::exception& e) {
                std::cerr << "Client exception: " << e.what() << std::endl;
                return false;
            }
        }
    };
}
#endif //AMMOPROTOCOL_CLIENT_HPP
