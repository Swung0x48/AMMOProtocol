#ifndef AMMOPROTOCOL_CONNECTION_HPP
#define AMMOPROTOCOL_CONNECTION_HPP
#include "network.hpp"
#include "../structure/structure_all.hpp"

template <typename T>
class connection {
private:
    ammo::structure::ts_queue<ammo::common::message<T>> incoming_packets_;
    std::array<uint8_t, MAX_PACKET_SIZE> buffer_;
    ammo::common::message<T> current_packet_;
    asio::ip::udp::socket socket_;
    asio::ip::udp::endpoint remote_endpoint_;
public:
    connection(asio::io_context& io_context, uint16_t port)
        : socket_(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)){

    }

private:
    void read_message() {
        socket_.async_receive_from(
                asio::buffer(buffer_), remote_endpoint_,
                [this](const asio::error_code& error, size_t bytes_transferred) {
                    if (!error) {
                        current_packet_(buffer_, bytes_transferred);
                        if (!current_packet_.unpack_and_verify())
                            return;

                        incoming_packets_.push_back(current_packet_);
                    }
                });
    }
};

#endif //AMMOPROTOCOL_CONNECTION_HPP
