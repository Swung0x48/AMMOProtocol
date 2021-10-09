#ifndef AMMOPROTOCOL_MESSAGE
#define AMMOPROTOCOL_MESSAGE
#include "common.hpp"
#include "../network/network_all.hpp"

namespace ammo::common {
    template <typename T>
    struct message_header {
        static_assert(sizeof(T) == 4 && CHAR_BIT == 8, "T should be an uint32_t based enum");
        uint32_t begin_of_packet = 0u;
        uint32_t crc32 = 0u;
        uint32_t sequence = 0u;
        T opcode {};
        uint16_t message_size = 0u;
        uint8_t message_state = 0u;

        uint32_t last_acked = 0u;
        uint32_t ack_bitmap = 0u;
        uint64_t send_time = 0u;
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
//        static constexpr uint32_t end_of_packet_ = 1;
        static constexpr uint8_t PACKED_MASK = 0b00000001;
        static constexpr uint8_t VALIDATED_MASK = 0b00000010;
        static constexpr uint8_t RELIABLE_MASK = 0b00000100;
        static constexpr uint8_t ORDERED_MASK = 0b00001000;

    public:
        message() = default;

        static inline bool sequence_greater_than(T s1, T s2, uint64_t wrap) {
            return ((s1 > s2) && (s1 - s2 <= wrap / 2)) ||
                   ((s1 < s2) && (s2 - s1 > wrap / 2));
        }

        static inline bool sequence_greater_than(T s1, T s2) {
            static_assert(std::is_unsigned<T>(), "T should be an unsigned integral type.");
            return sequence_greater_than(s1, s2, std::numeric_limits<T>::max());
        }

        [[nodiscard]] static inline T sequence_max(T s1, T s2, uint64_t wrap) {
            return sequence_greater_than(s1, s2, wrap) ? s1 : s2;
        }

        [[nodiscard]] static inline T sequence_max(T s1, T s2) {
            return sequence_greater_than(s1, s2) ? s1 : s2;
        }

        [[nodiscard]] static inline T sequence_min(T s1, T s2, uint64_t wrap) {
            return sequence_greater_than(s1, s2, wrap) ? s2 : s1;
        }

        [[nodiscard]] static inline T sequence_min(T s1, T s2) {
            return sequence_greater_than(s1, s2) ? s2 : s1;
        }

        message(const uint8_t* buffer, size_t size) {
            if (size < sizeof(message_header<T>) || buffer == nullptr)
                return;

            std::memcpy(&header, buffer, sizeof(header));
            body.resize(size - sizeof(header));
            std::memcpy(body.data(),
                        &buffer[sizeof(header)],
                        size - sizeof(header));
//            packed_ = true;
            header.message_state |= PACKED_MASK;
        }

        bool clear() {
            body.clear();
            header.message_size = 0;
            header.message_state = 0u;
            read_position = 0;
            write_position = 0;
            return true;
        }

        bool write(const void* data, size_t size) {
            if (is_packed())
                return false;

            header.message_state = 0u;
            if (body.size() < write_position + size)
                body.resize(write_position + size);
            std::memcpy(body.data() + write_position, data, size);
            write_position += size;
            return true;
        }

        bool read(void* data, size_t size) {
            if (!is_validated())
                return false;

            if (read_position + size > body.size())
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
                throw std::runtime_error("Packet write failed. Maybe packet is packed?");

            // Return the resulting msg so that it could be chain-called.
            return pkt;
        }

        template<typename Data>
        friend message<T>& operator>>(message<T>& pkt, Data& data) {
            static_assert(std::is_standard_layout<Data>::value,
                          "Type of data is not in standard layout thus not able to be deserialized.");

            if (!pkt.read(&data, sizeof(data)))
                throw std::runtime_error("Packet read failed. Maybe not unpacked or read past the end of packet. " + std::to_string(pkt.body.size()) + "/" + std::to_string(pkt.read_position + sizeof(Data)));

            // Return the resulting msg so that it could be chain-called.
            return pkt;
        }

        bool pack() {
//            write_position = body.size();
//            write(&end_of_packet_, sizeof(end_of_packet_));
            header.begin_of_packet = 1u;
            header.message_size = body.size();
            if (header.message_size > MAX_PACKET_SIZE) {
                std::cerr << "[ERR] Message size exceeds MAX_PACKET_SIZE. " << '(' << header.message_size << '>' << MAX_PACKET_SIZE << ')' << std::endl;
                return false;
            }
            header.message_state |= PACKED_MASK;
            header.message_state &= ~VALIDATED_MASK; // not validated
            header.crc32 = crc32_fast(&magical_prefix_, sizeof(magical_prefix_));
            header.crc32 = crc32_fast(body.data(), body.size(), header.crc32);
            header.crc32 = crc32_fast(&header.sequence, sizeof(header) - sizeof(header.crc32) - sizeof(header.begin_of_packet), header.crc32);
            return true;
        }

        bool is_packed() {
            return bool(header.message_state & PACKED_MASK);
        }

        bool is_validated() {
            return bool((header.message_state & VALIDATED_MASK) >> 1);
        }

        bool is_reliable() {
            return (header.message_state & RELIABLE_MASK) != 0;
        }

        void set_reliable(bool value) {
            if (value)
                header.message_state |= RELIABLE_MASK;
            else
                header.message_state &= (~RELIABLE_MASK);
        }

        void set_ordered(bool value) {
            if (value)
                header.message_state |= ORDERED_MASK;
            else
                header.message_state &= (~ORDERED_MASK);
        }

        bool is_ordered() {
            return (header.message_state & ORDERED_MASK) != 0;
        }

        bool unpack_and_verify() {
            if (header.begin_of_packet != 1u)
                return false;
            auto actual_crc32 = crc32_fast(&magical_prefix_, sizeof(magical_prefix_));
            actual_crc32 = crc32_fast(body.data(), header.message_size, actual_crc32);
            actual_crc32 = crc32_fast(&header.sequence, sizeof(header) - sizeof(header.crc32) - sizeof(header.begin_of_packet), actual_crc32);
            if (!is_packed() || actual_crc32 != header.crc32)
                return false;
            // Check if has a valid end_of_packet_
//            if (*((typeof(end_of_packet_)*)(body.data() + body.size() - sizeof(end_of_packet_))) != end_of_packet_)
//                return false;

//            body.resize(body.size() - sizeof(end_of_packet_));
//            header.message_size -= sizeof(end_of_packet_);
            header.message_state &= ~PACKED_MASK; // not packed
            header.message_state |= VALIDATED_MASK; // validated
//            packed_ = false;
//            validated_ = true;
            return true;
        }
    };

#include "../network/network.hpp"
    template<typename T>
    struct owned_message
    {
        asio::ip::udp::endpoint remote;
        struct message<T> message;
    };
}


#endif //AMMOPROTOCOL_MESSAGE
