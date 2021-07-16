#ifndef AMMOPROTOCOL_MESSAGE
#define AMMOPROTOCOL_MESSAGE
#include "common.hpp"
#include "../network/network_all.hpp"

namespace ammo::network {
    template <typename Data>
    class connection;
}

namespace ammo::common {
    template <typename T>
    struct message_header {
        static_assert(sizeof(T) == 4 && CHAR_BIT == 8, "T should be an uint32_t based enum");
        uint32_t crc32 = 0u;
        uint32_t sequence = 0u;
        T id {};
        uint16_t message_size = 0u;
        uint8_t message_state = 0u;
    };

    template <typename T>
    struct message {
    public:
        message_header<T> header {};
        std::vector<uint8_t> body;

        // Below members are not sent
        size_t write_position = 0;
        size_t read_position = 0;
    private:
        static constexpr uint64_t magical_prefix_ = 1626227971404604; // Not sent
        static constexpr uint32_t end_of_packet_ = 1;
        bool packed_ = false;
        bool validated_ = false;

    public:
        message() = default;

        message(const uint8_t* buffer, size_t size) {
            if (size < sizeof(message_header<T>) || buffer == nullptr)
                return;

            std::memcpy(&header, buffer, sizeof(header));
            body.resize(size - sizeof(header));
            std::memcpy(body.data(),
                        &buffer[sizeof(header)],
                        size - sizeof(header));
            packed_ = true;
        }

        bool write(const void* data, size_t size) {
            if (packed_)
                return false;
            body.resize(write_position + size);
            std::memcpy(body.data() + write_position, data, size);
            write_position += size;
            return true;
        }

        bool read(void* data, size_t size) {
            if (!validated_)
                return false;

            std::memcpy(data, body.data() + read_position, size);
            read_position += size;
            return true;
        }

        template<typename Data>
        friend message<T>& operator<<(message<T>& pkt, const Data& data) {
            static_assert(std::is_standard_layout<Data>::value,
                          "Type of data is not in standard layout thus not able to be serialized.");

            if (!pkt.write(&data, sizeof(data)))
                throw std::runtime_error("Packet write failed. Maybe packet is packed_?");

            // Return the resulting msg so that it could be chain-called.
            return pkt;
        }

        template<typename Data>
        friend message<T>& operator>>(message<T>& pkt, Data& data) {
            static_assert(std::is_standard_layout<Data>::value,
                          "Type of data is not in standard layout thus not able to be deserialized.");

            if (!pkt.read(&data, sizeof(data)))
                throw std::runtime_error("Packet read failed. Maybe not unpacked?");

            // Return the resulting msg so that it could be chain-called.
            return pkt;
        }

        bool pack() {
            write(&end_of_packet_, sizeof(end_of_packet_));
            header.message_size = body.size();
            if (header.message_size > MAX_PACKET_SIZE) {
                return false;
            }
            header.crc32 = crc32_fast(&magical_prefix_, sizeof(magical_prefix_));
            header.crc32 = crc32_fast(body.data(), body.size(), header.crc32);
            packed_ = true;
            return true;
        }

        bool unpack_and_verify() {
            auto actual_crc32 = crc32_fast(&magical_prefix_, sizeof(magical_prefix_));
            actual_crc32 = crc32_fast(body.data(), header.message_size, actual_crc32);
            if (!packed_ || actual_crc32 != header.crc32)
                return false;
            // Check if has a valid end_of_packet_
            if (*((typeof(end_of_packet_)*)(body.data() + body.size() - sizeof(end_of_packet_))) != end_of_packet_)
                return false;

            packed_ = false;
            validated_ = true;
            return true;
        }
    };

#include "../network/network.hpp"
    template<typename T>
    struct owned_message
    {
        asio::ip::udp::endpoint remote;
        message<T> message;
    };
}


#endif //AMMOPROTOCOL_MESSAGE
