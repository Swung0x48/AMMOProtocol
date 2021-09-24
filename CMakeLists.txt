cmake_minimum_required(VERSION 3.14)
project(AMMOProtocol)

set(CMAKE_CXX_STANDARD 20)
set(THREADS_PREFER_PTHREAD_FLAG ON)

include_directories(submodules/asio/asio/include)

add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>")
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
add_executable(test_sanity_check tests/test_sanity_check.cpp)
target_link_libraries(test_sanity_check gtest_main)

add_executable(test_packet_rw tests/test_packet_rw.cpp submodules/crc32/Crc32.cpp)
target_link_libraries(test_packet_rw gtest_main)

add_executable(test_entity_serialization tests/test_entity_serialization.cpp submodules/crc32/Crc32.cpp)
target_link_libraries(test_entity_serialization gtest_main)

add_executable(test_modulo_queue tests/test_modulo_queue.cpp)
target_link_libraries(test_modulo_queue gtest_main)

add_executable(test_reliable_channel tests/test_reliable_channel.cpp)
target_link_libraries(test_reliable_channel gtest_main)

include(GoogleTest)
add_test(NAME test_sanity_check COMMAND test_sanity_check)
add_test(NAME test_packet_rw COMMAND test_packet_rw)
add_test(NAME test_entity_serialization COMMAND test_entity_serialization)
add_test(NAME test_modulo_queue COMMAND test_modulo_queue)
add_test(NAME test_reliable_channel COMMAND test_reliable_channel)

include_directories(submodules/asio)
include_directories(submodules/crc32)
include_directories(includes)

find_package(Threads REQUIRED)
add_executable(SimpleServer example_impl/SimpleServer.cpp  submodules/crc32/Crc32.cpp submodules/crc32/Crc32.h)
target_link_libraries (SimpleServer PRIVATE Threads::Threads)
add_executable(SimpleClient example_impl/SimpleClient.cpp  submodules/crc32/Crc32.cpp submodules/crc32/Crc32.h)
target_link_libraries (SimpleClient PRIVATE Threads::Threads)
