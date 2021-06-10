cmake_minimum_required(VERSION 3.14)
project(BMMOProtocol)

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
add_executable(BMMOProtocol main.cpp includes/BMMOProtocol.hpp submodules/crc32/Crc32.cpp submodules/crc32/Crc32.h)

enable_testing()

include_directories(tests)
add_executable(sanity_check tests/sanity_check.cpp)
target_link_libraries(sanity_check gtest_main)

include(GoogleTest)
gtest_discover_tests(sanity_check)