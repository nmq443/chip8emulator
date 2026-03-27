cmake_minimum_required(VERSION 3.25)

project(ch8emu)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

################################################################################
#                           Fetch GoogleTest and SDL3                          #
################################################################################

include(FetchContent)

# Google test
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)

# SDL3
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        release-3.4.2
)
FetchContent_MakeAvailable(SDL3)

################################################################################
#                                  Build rules                                 #
################################################################################

# Define core logic as a library
add_library(ch8_core STATIC 
    src/cpu.cxx 
    src/utils.cxx
    src/renderer.cxx
)

target_link_libraries(ch8_core PUBLIC SDL3::SDL3)

# Define the main executable
add_executable(ch8emu src/main.cxx)
target_link_libraries(ch8emu PRIVATE ch8_core)

################################################################################
#                                    Testing                                   #
################################################################################

# Setup Testing
enable_testing() # Call this before discovering tests
include(GoogleTest)

add_executable(cpu_tests tests/cpu_tests.cxx)
# Link to the core library and GTest
target_link_libraries(cpu_tests PRIVATE ch8_core GTest::gtest_main)
# Ensure headers in src/ are visible
target_include_directories(cpu_tests PRIVATE src)

gtest_discover_tests(cpu_tests)
