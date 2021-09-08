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
                sender_(io_context_, socket_),
                connections_() {
            if (port == 0)
                socket_.open(asio::ip::udp::v4());
            else
                socket_.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), port));

            event_handler_
                .template on<event::on_message_event<T>>(
                        [](event::on_message_event<T>& e) {
//                            on_message(e.get_connection(), e.get_message());
                        })
                .template on<event::role_send_event<T>>(
                        [](event::role_send_event<T>& e) {
//                            commit_send({ e.get_connection().get_remote(), e.get_message() });
                        });
        }

        virtual ~role() {
            stop();
        }

        virtual bool start() {
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

        virtual void send(network::connection<T>& destination, common::message<T>& msg) {
            destination.on_send(msg);
        }

        virtual void accept_connection(asio::ip::udp::endpoint& endpoint) {
            connections_.emplace(endpoint, std::make_shared<ammo::network::connection<T>>(endpoint, event_handler_));
        }
    protected:
        virtual void on_receive(ammo::common::owned_message<T>& msg) {
            if (!connections_.contains(msg.remote)) [[unlikely]] {
                on_authenticate_message(msg);
            }

            connections_[msg.remote]->on_receive(msg.message);
        }

        virtual void on_authenticate_message(ammo::common::owned_message<T>& msg) = 0;
        virtual void on_message(network::connection<T>& destination, common::message<T>& msg) = 0;
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
        std::unordered_map<asio::ip::udp::endpoint, std::shared_ptr<ammo::network::connection<T>>> connections_;
    };
}

#endif //AMMOPROTOCOL_ROLE_HPP
