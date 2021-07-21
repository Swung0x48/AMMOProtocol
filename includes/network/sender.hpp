#ifndef AMMOPROTOCOL_SENDER_HPP
#define AMMOPROTOCOL_SENDER_HPP

#include "network.hpp"
namespace ammo::network {
    template <typename T>
    class sender {
    private:
        ammo::structure::ts_queue<ammo::common::owned_message<T>>& outgoing_messages_;
        asio::ip::udp::socket& socket_;
        asio::io_context& io_context_;
    public:
        sender(asio::io_context& io_context, asio::ip::udp::socket& socket, ammo::structure::ts_queue<ammo::common::owned_message<T>>& outgoing_messages):
                io_context_(io_context), socket_(socket), outgoing_messages_(outgoing_messages) {

        }

        virtual ~sender() = default;

        void send(const ammo::common::owned_message<T>& msg) {
            asio::post(io_context_,
                       [this, msg]() {
                           bool writing_message = !outgoing_messages_.empty();
                           outgoing_messages_.push_back(msg);
                           if (!writing_message)
                               write_message();
                       });
        }

    private:
        // async
        void write_message() {
            std::array<asio::const_buffer, 2> message_buffer = {
                    asio::buffer(&outgoing_messages_.front().message.header,
                                 sizeof(outgoing_messages_.front().message.header)),
                    asio::buffer(outgoing_messages_.front().message.body.data(),
                                 outgoing_messages_.front().message.body.size())
            };

            socket_.async_send_to(message_buffer, outgoing_messages_.front().remote,
                                  [this](const asio::error_code &error, size_t bytes_transferred) {
                if (!error) {
                    outgoing_messages_.pop_front();
                    if (!outgoing_messages_.empty())
                        write_message();
                } else {
                    std::cerr << "[ERR] Failed to send message.\n";
                    std::cerr << "[ERR] Remote: " << outgoing_messages_.front().remote << std::endl;
                }
            });
        }
    };
}

#endif //AMMOPROTOCOL_SENDER_HPP
