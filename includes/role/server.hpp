#ifndef AMMOPROTOCOL_SERVER_HPP
#define AMMOPROTOCOL_SERVER_HPP
namespace ammo::role {
    template <typename T>
    class server {
    protected:
        asio::io_context io_context_;
        std::thread ctx_thread_;
        asio::ip::udp::socket socket_;
        ammo::network::receiver<T> receiver_;
        ammo::network::sender<T> sender_;
    private:
        ammo::structure::ts_queue<ammo::common::owned_message<T>> incoming_messages_;
        ammo::structure::ts_queue<ammo::common::owned_message<T>> outgoing_messages_;

    public:
        explicit server(uint16_t port):
            socket_(io_context_, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
            receiver_(socket_, incoming_messages_),
            sender_(io_context_, socket_, outgoing_messages_) {
        }

        bool start() {
            try {
                receiver_.start_receiving();
                ctx_thread_ = std::thread([this]() { io_context_.run(); });
            } catch (std::exception& e) {
                std::cerr << "[ERR] Exception occurred on server start.\n";
                std::cerr << "[ERR] Exception: " << e.what() << std::endl;
                return false;
            }

            std::cout << "[INFO] Server started!\n";
            return true;
        }

        void update(size_t max_message_count = -1, bool wait = true) {
            if (wait)
                incoming_messages_.wait();

            size_t message_count = 0;
            while (message_count < max_message_count && !incoming_messages_.empty()) {
                auto msg = incoming_messages_.pop_front();
                on_message(msg);
                ++message_count;
            }
        }

        void stop() {
            io_context_.stop();
            if (ctx_thread_.joinable())
                ctx_thread_.join();

            std::cout << "[INFO] Server Stopped!" << std::endl;
        }

        bool accept_client() {

        }

    protected:
        virtual void on_message(ammo::common::owned_message<T>& msg) = 0;
    };
}
#endif //AMMOPROTOCOL_SERVER_HPP
