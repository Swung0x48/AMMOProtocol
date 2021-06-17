#include <iostream>
#include "includes/AMMOProtocol.hpp"
#include <Crc32.h>
#define CRC32_TEST_BITWISE
#define CRC32_TEST_HALFBYTE
#define CRC32_TEST_TABLELESS

int main() {
    uint64_t a = 0x21;
    auto b = crc32_fast(reinterpret_cast<void*>(&a), sizeof(a));
    std::cout << b;
//    std::cout << "Hello, World!" << std::endl;
    return 0;
}
