cmake_minimum_required(VERSION 3.25)

project(ch8emu)

# 1. Define core logic as a library
add_library(ch8_core STATIC 
    src/cpu.cxx 
    src/utils.cxx
)

# 2. Define the main executable
add_executable(ch8emu src/main.cxx)
target_link_libraries(ch8emu PRIVATE ch8_core)

# 3. Setup GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)

# 4. Setup Testing
enable_testing() # Call this before discovering tests
include(GoogleTest)

add_executable(cpu_tests tests/cpu_tests.cxx)
# Link to the core library and GTest
target_link_libraries(cpu_tests PRIVATE ch8_core GTest::gtest_main)
# Ensure headers in src/ are visible
target_include_directories(cpu_tests PRIVATE src)

gtest_discover_tests(cpu_tests)
