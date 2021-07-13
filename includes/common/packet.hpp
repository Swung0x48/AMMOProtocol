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

    public:
        void write(const void* data, size_t size) {
            body.resize(write_position + size);
            std::memcpy(body.data() + write_position, data, size);
            write_position += size;
        }

        void read(void* data, size_t size) {
            std::memcpy(data, body.data() + read_position, size);
            read_position += size;
        }

        template<typename Data>
        friend packet<T>& operator<<(packet<T>& msg, const Data& data) {
            static_assert(std::is_standard_layout<Data>::value,
                          "Type of data is not in standard layout thus not able to be serialized.");
            msg.write(&data, sizeof(data));
        }

        template<typename Data>
        friend packet<T>& operator>>(packet<T>& msg, Data& data) {
            static_assert(std::is_standard_layout<Data>::value,
                          "Type of data is not in standard layout thus not able to be deserialized.");

            msg.read(&data, sizeof(data));
        }

        void pack() {
            header.message_size = body.size();
            header.crc32 = crc32_fast(body.data(), header.message_size);
        }

        bool unpack() {
            if (crc32_fast(body.data(), header.message_size) != header.crc32)
                return false;

            return true;
        }
    };
}


#endif //AMMOPROTOCOL_MESSAGE
