#ifndef AMMOPROTOCOL_MESSAGE
#define AMMOPROTOCOL_MESSAGE
#include "common.hpp"

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
    struct packet {
    public:
        message_header<T> header {};
        std::vector<uint8_t> body;
        size_t write_position = 0;
        size_t read_position = 0;
        bool packed = false;
        bool validated = false;

    public:
        bool write(const void* data, size_t size) {
            if (packed)
                return false;
            body.resize(write_position + size);
            std::memcpy(body.data() + write_position, data, size);
            write_position += size;
            return true;
        }

        bool read(void* data, size_t size) {
            if (!validated)
                return false;

            std::memcpy(data, body.data() + read_position, size);
            read_position += size;
            return true;
        }

        template<typename Data>
        friend packet<T>& operator<<(packet<T>& pkt, const Data& data) {
            static_assert(std::is_standard_layout<Data>::value,
                          "Type of data is not in standard layout thus not able to be serialized.");

            if (!pkt.write(&data, sizeof(data)))
                throw std::runtime_error("Packet write failed. Maybe packet is packed?");

            // Return the resulting msg so that it could be chain-called.
            return pkt;
        }

        template<typename Data>
        friend packet<T>& operator>>(packet<T>& pkt, Data& data) {
            static_assert(std::is_standard_layout<Data>::value,
                          "Type of data is not in standard layout thus not able to be deserialized.");

            if (!pkt.read(&data, sizeof(data)))
                throw std::runtime_error("Packet read failed. Maybe not unpacked?");

            // Return the resulting msg so that it could be chain-called.
            return pkt;
        }

        void pack() {
            header.message_size = body.size();
            header.crc32 = crc32_fast(body.data(), header.message_size);
            packed = true;
        }

        bool unpack() {
            if (!packed || crc32_fast(body.data(), header.message_size) != header.crc32)
                return false;

            packed = false;
            validated = true;
            return true;
        }
    };
}


#endif //AMMOPROTOCOL_MESSAGE
