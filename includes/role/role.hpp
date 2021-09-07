#ifndef AMMOPROTOCOL_ROLE_HPP
#define AMMOPROTOCOL_ROLE_HPP

#include <event/role_send_event.hpp>
#include "network/network_all.hpp"
namespace ammo::role {
    template <typename T>
    class role {
    public:
        explicit role(uint16_t port = 0):
                socket_(io_context_),
                receiver_(socket_),
                sender_(io_context_, socket_) {
            if (port == 0)
                socket_.open(asio::ip::udp::v4());
            else
                socket_.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), port));

            event_handler_
                .on<event::on_message_event>(
                        [this](event::on_message_event<T>& e) {
                            on_message(*(e.get_connection()), e.get_message()); })
                .template on<event::role_send_event>(
                        [this](event::role_send_event<T>& e) {
                            commit_send(e.get_message());
                        });
        }

        virtual ~role() {
            stop();
        }

        bool start() {
            receiver_.start_receiving();
            ctx_thread_ = std::thread([this]() { io_context_.run(); });
        }

        void stop() {
            if (!io_context_.stopped())
                io_context_.stop();
            if (ctx_thread_.joinable())
                ctx_thread_.join();
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
                on_receive(msg);
                ++message_count;
            }
            on_update();

            return status;
        }

        void send(ammo::network::connection<T>& connection, ammo::common::message<T>& msg) = 0;
    protected:
        virtual void on_receive(ammo::common::owned_message<T>& msg) = 0;
        virtual void on_message(ammo::network::connection<T>& connection, ammo::common::message<T>& msg) = 0;
        virtual void on_update() = 0;

        void commit_send(ammo::common::owned_message<T>& msg) {
            if (msg.message.is_packed())
                msg.message.unpack_and_verify();
            msg.message.pack();
            sender_.send(msg);
        }
    protected:
        asio::io_context io_context_;
        std::thread ctx_thread_;
        asio::ip::udp::socket socket_;
        ammo::network::receiver<T> receiver_;
        ammo::network::sender<T> sender_;
        ammo::event::event_handler event_handler_;
    };
}

#endif //AMMOPROTOCOL_ROLE_HPP
