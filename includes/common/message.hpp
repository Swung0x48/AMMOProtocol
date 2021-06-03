#ifndef BMMOPROTOCOL_MESSAGE
#define BMMOPROTOCOL_MESSAGE
#include "common.hpp"

namespace bmmo::message {
    template <typename T>
    struct message_header {
        uint32_t crc32 = 0u;
        uint32_t sequence = 0u;
        T id {};   // should be an uint32_t based enum
        uint16_t message_size = 0u;
        uint8_t message_state = 0u;
    };

    template <typename T>
    struct message {
        message_header<T> header {};
        std::vector<uint8_t> body;
    };
}


#endif //BMMOPROTOCOL_MESSAGE
