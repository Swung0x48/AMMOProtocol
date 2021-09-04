#ifndef AMMOPROTOCOL_CHANNEL_HPP
#define AMMOPROTOCOL_CHANNEL_HPP

#include "network.hpp"
namespace ammo::network {
    template<typename T>
    class channel {
    public:
        channel(asio::io_context& io_context, asio::ip::udp::socket& socket):
            receiver_(socket),
            sender_(io_context, socket) {}

        void send(ammo::common::owned_message<T>& msg) {
            if (msg.message.is_packed())
                msg.message.unpack_and_verify();

            on_send(msg);
            msg.message.pack();
            sender_.send(msg);
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

    protected:
        virtual void on_send(ammo::common::owned_message<T>& msg) {
            msg.message.header.sequence = send_sequence_++;
        }

        virtual void on_receive(ammo::common::owned_message<T>& msg) {
            on_message(msg);
        }

        virtual void on_update() = 0;

        virtual void on_message(ammo::common::owned_message<T>& msg) = 0;

        ammo::network::receiver<T> receiver_;
        ammo::network::sender<T> sender_;
        uint32_t send_sequence_ = 0u;
    };
}

#endif //AMMOPROTOCOL_CHANNEL_HPP
