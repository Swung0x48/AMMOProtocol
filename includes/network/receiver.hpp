#ifndef AMMOPROTOCOL_RECEIVER_HPP
#define AMMOPROTOCOL_RECEIVER_HPP

#include "network.hpp"
namespace ammo::common {
    template<typename Data>
    class message;

    template<typename Data>
    class owned_message;
}

namespace ammo::network {
    template<typename T>
    class receiver {
    private:
        ammo::structure::ts_queue<ammo::common::owned_message<T>>& incoming_messages_;
        std::array<uint8_t, 2 * MAX_PACKET_SIZE> buffer_;
        asio::ip::udp::socket& socket_;
        asio::ip::udp::endpoint current_remote_endpoint_;
        ammo::common::message<T> current_incoming_message_;
    public:
        receiver(asio::ip::udp::socket& socket, ammo::structure::ts_queue<ammo::common::owned_message<T>>& incoming_messages):
                socket_(socket), incoming_messages_(incoming_messages) {

        }

        virtual ~receiver() = default;

        virtual void start_receiving() {
            read_message();
        }
    private:
        std::string endpoint_to_string(const asio::ip::udp::endpoint& endpoint) {
            return endpoint.address().to_string() + ':' + std::to_string(endpoint.port());
        }

        void add_to_incoming_message_queue() {
            incoming_messages_.push_back({ current_remote_endpoint_, current_incoming_message_ });
        }

        void parse_message_from_buffer() {
//            std::memcpy(current_incoming_message_.header, buffer_, sizeof(current_incoming_message_.header));
            std::memcpy(buffer_.data(), &current_incoming_message_.header, sizeof(current_incoming_message_.header));
            current_incoming_message_.body.resize(current_incoming_message_.header.message_size);
            std::memcpy(buffer_.data() + sizeof(current_incoming_message_.header), current_incoming_message_.body.data(), current_incoming_message_.body.size());
        }

        // async
        void read_message() {
            socket_.async_receive_from(
                    asio::buffer(buffer_), current_remote_endpoint_,
                    [this](const asio::error_code &error, size_t bytes_transferred) {
                        if (!error) {
//                            current_incoming_message_(buffer_, bytes_transferred);
                            parse_message_from_buffer();
                            if (!current_incoming_message_.unpack_and_verify()) {
                                std::cout << "[WARN] From " << endpoint_to_string(current_remote_endpoint_) << "\n";
                                std::cout << "[WARN] Packet failed on unpack_and_verify.\n";
                                std::cout << "[WARN] Packet probably corrupted." << '\n';
                                std::cout << "[WARN] Bytes transferred: " << bytes_transferred << std::endl;
                            } else {
                                add_to_incoming_message_queue();
                            }
                        }
                        read_message();
                    });
        }
    };
}

#endif //AMMOPROTOCOL_RECEIVER_HPP
