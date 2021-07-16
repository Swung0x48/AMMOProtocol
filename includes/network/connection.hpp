//#ifndef AMMOPROTOCOL_CONNECTION_HPP
//#define AMMOPROTOCOL_CONNECTION_HPP
//
//#include "common_all.hpp"
//#include "network.hpp"
//#include "../structure/structure_all.hpp"
//
//namespace ammo::common {
//    template<typename Data>
//    class message;
//
//    template<typename Data>
//    class owned_message;
//}
//
//namespace ammo::network {
//    template<typename T>
//    class connection: public std::enable_shared_from_this<connection<T>> {
//    public:
//        enum class owner {
//            server,
//            client
//        };
//    protected:
//        ammo::common::message<T> current_incoming_packet_;
//        asio::ip::udp::socket socket_;
//        asio::ip::udp::endpoint current_remote_endpoint_;
//        uint64_t id_ = 0;
//        owner owner_ = owner::server;
//        bool validated_ = false;
//
//        uint64_t checksum_out_ = 0;
//        uint64_t checksum_in_ = 0;
//        uint64_t expected_checksum_ = 0;
//    public:
//        connection(owner owner, asio::io_context &io_context, uint16_t port)
//        : owner_(owner), socket_(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)) {
//            checksum_in_ = 0;
//            checksum_out_ = 0;
//
//            if (owner_ == owner::server) {
//                checksum_out_ = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());
//                expected_checksum_ = encode(checksum_out_);
//            }
//        }
//
//        virtual ~connection() = default;
//
//        uint64_t get_id() const {
//            return id_;
//        }
//
//        bool is_validated() const {
//            return validated_;
//        }
//
//    private:
//        void add_to_incoming_message_queue() {
//            incoming_packets_.push_back({
//                (owner_ == owner::server) ? this->shared_from_this() : nullptr,
//                current_incoming_packet_
//            });
//        }
//
//        uint64_t encode(uint64_t bin) {
//            auto* slice = reinterpret_cast<uint8_t *>(&bin);
//            for (size_t i = 0; i < sizeof(bin) / sizeof(uint8_t); i++) {
//                slice[i] = (slice[i] << 3 | slice[i] >> 5);
//                slice[i] = -(slice[i] ^ uint8_t(14));
//            }
//            return bin;
//        }
//
//        // async
//        void read_message() {
//            socket_.async_receive_from(
//                    asio::buffer(buffer_), current_remote_endpoint_,
//                    [this](const asio::error_code &error, size_t bytes_transferred) {
//                        if (!error) {
//                            current_incoming_packet_(buffer_, bytes_transferred);
//                            if (!current_incoming_packet_.unpack_and_verify()) {
//                                std::cout << "[WARN] " << id_ << ": Packet failed on unpack_and_verify." << std::endl;
//                                std::cout << "[WARN] " << id_ << ": Packet probably corrupted." << std::endl;
//                                return;
//                            }
//
//                            add_to_incoming_message_queue();
////                            incoming_packets_.push_back({current_remote_endpoint_, current_incoming_packet_});
//                        }
//                        read_message();
//                    });
//        }
//
//        // async
//        void write_message() {
//            std::array<asio::const_buffer, 2> message_buffer = {
//                    asio::buffer(&outgoing_packets_.front().message.header, sizeof(outgoing_packets_.front().message.header)),
//                    asio::buffer(outgoing_packets_.front().message.body.data(), outgoing_packets_.front().message.body.size())
//            };
//            socket_.async_send_to(message_buffer, outgoing_packets_.front().owner,
//                                  [this](const asio::error_code &error, size_t bytes_transferred) {
//                if (!error) {
//                    outgoing_packets_.pop_front();
//                    if (!outgoing_packets_.empty())
//                        write_message();
//                } else {
//                    std::cout << "[WARN] " << id_ << ": Send message failed.\n";
//                    std::cout << "[WARN] " << id_ << ": " << error.message() << std::endl;
//                    socket_.close();
//                }
//            });
//        }
//
//        // async
//        void read_validation() {
//
//        }
//
//        // async
//        void write_validation() {
//
//        }
//    };
//}
//
//#endif //AMMOPROTOCOL_CONNECTION_HPP
