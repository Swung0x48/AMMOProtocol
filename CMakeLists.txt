cmake_minimum_required(VERSION 3.14)
project(AMMOProtocol)

set(CMAKE_CXX_STANDARD 20)

include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
    message(STATUS "Big endian detected")
    add_definitions(-D__BYTE_ORDER=4321)
else()
    message(STATUS "Not big endian, assuming little endian")
    add_definitions(-D__BYTE_ORDER=1234)
endif()

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/703bd9caab50b139428cea1aaff9974ebee5742e.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(submodules/crc32)
include_directories(includes/common)
add_executable(AMMOProtocol main.cpp includes/AMMOProtocol.hpp submodules/crc32/Crc32.cpp submodules/crc32/Crc32.h)

enable_testing()

include_directories(tests)
add_executable(sanity_check tests/sanity_check.cpp)
target_link_libraries(sanity_check gtest_main)

add_executable(packet_rw tests/packet_rw.cpp submodules/crc32/Crc32.cpp)
target_link_libraries(packet_rw gtest_main)

add_executable(entity_serialization tests/entity_serialization.cpp submodules/crc32/Crc32.cpp)
target_link_libraries(entity_serialization gtest_main)

include(GoogleTest)
add_test(NAME sanity_check COMMAND sanity_check)
add_test(NAME packet_rw COMMAND packet_rw)
add_test(NAME entity_serialization COMMAND entity_serialization)