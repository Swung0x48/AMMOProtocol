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
        std::atomic_bool ctx_started_ = false;
        std::thread update_thread_;
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
            send_request();
        }

        virtual void on_message(ammo::common::owned_message<T>& message) = 0;
    public:
        client():
        socket_(io_context_),
        receiver_(socket_, incoming_messages_),
        sender_(io_context_, socket_, outgoing_messages_) {
        }

        bool connect(const std::string& host, const uint16_t port) {
            try {
                if (!ctx_started_) {
                    socket_.open(asio::ip::udp::v4());
                }
                asio::ip::udp::resolver resolver(io_context_);
                server_endpoint_ = *resolver.resolve(host, std::to_string(port)).begin();
                connect_to_server();
                if (!ctx_started_) {
                    receiver_.start_receiving();
                    ctx_thread_ = std::thread([this]() { io_context_.run(); });
                    ctx_started_ = true;
                    update_thread_ = std::thread([this]() {
                        while (ctx_started_)
                            update(64, true, std::chrono::minutes(5));
                    });
                }
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

        virtual void send_request() = 0;

        void confirm_validation() {
            client_state_ = client_state::Connected;
        }

        virtual void disconnect() {
            client_state_ = ammo::role::client_state::Disconnected;
        }

        virtual void shutdown() {
            if (ctx_started_) {
                ctx_started_ = false;
                io_context_.stop();
                if (ctx_thread_.joinable())
                    ctx_thread_.join();
                incoming_messages_.tick();
                if (update_thread_.joinable())
                    update_thread_.join();
                socket_.close();
            }
        }

        template<class Rep, class Period>
        std::cv_status update(size_t max_message_count = -1,
                              bool wait = true,
                              const std::chrono::duration<Rep, Period>& rel_time = std::chrono::steady_clock::duration::zero()) {
            auto status = std::cv_status::no_timeout;
            if (wait) {
                if (rel_time == std::chrono::steady_clock::duration::zero())
                    incoming_messages_.wait();
                else
                    status = incoming_messages_.wait_for(rel_time);
            }

            size_t message_count = 0;
            while (message_count < max_message_count && !incoming_messages_.empty()) {
                auto msg = incoming_messages_.pop_front();
                on_message(msg);
                ++message_count;
            }
            return status;
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
