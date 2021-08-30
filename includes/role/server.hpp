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

    public:
        explicit server(uint16_t port):
            socket_(io_context_, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
            receiver_(socket_),
            sender_(io_context_, socket_) {
        }

        virtual ~server() {
            stop();
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
            return true;
        }

        template<class Rep, class Period>
        std::cv_status update(size_t max_message_count = -1,
                              bool wait = true,
                              const std::chrono::duration<Rep, Period>& rel_time = std::chrono::steady_clock::duration::zero()) {
            auto status = std::cv_status::no_timeout;
            if (wait) {
                if (rel_time == std::chrono::steady_clock::duration::zero())
                    receiver_.get_incoming_messages().wait();
                else
                    status = receiver_.get_incoming_messages().wait_for(rel_time);
            }

            size_t message_count = 0;
            while (message_count < max_message_count && !receiver_.get_incoming_messages().empty()) {
                auto msg = receiver_.get_incoming_messages().pop_front();
                on_message(msg);
                ++message_count;
            }
            return status;
        }

        void stop() {
            io_context_.stop();
            if (ctx_thread_.joinable())
                ctx_thread_.join();
        }

        void tick() {
            receiver_.get_incoming_messages().tick();
        }

        void send(ammo::common::owned_message<T>& msg) {
            if (msg.message.is_packed())
                msg.message.unpack_and_verify();
            msg.message.pack();
            sender_.send(msg);
        }

        bool accept_client() {
            return true;
        }

    protected:
        virtual void on_message(ammo::common::owned_message<T>& msg) = 0;
    };
}
#endif //AMMOPROTOCOL_SERVER_HPP
