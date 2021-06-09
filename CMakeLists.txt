cmake_minimum_required(VERSION 3.14)
project(BMMOProtocol)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/703bd9caab50b139428cea1aaff9974ebee5742e.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(includes/common)
add_executable(BMMOProtocol main.cpp includes/BMMOProtocol.hpp)

enable_testing()

include_directories(tests)
add_executable(sanity_check tests/sanity_check.cpp)
target_link_libraries(sanity_check gtest_main)

include(GoogleTest)
gtest_discover_tests(sanity_check)