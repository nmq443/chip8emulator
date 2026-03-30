cmake_minimum_required(VERSION 3.25)

project(ch8emu)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Apply warnings
if(MSVC)
    # For msvc
    target_compile_options(ch8_core PRIVATE /W4 /WX)
    target_compile_options(ch8emu PRIVATE /W4 /WX)
else()
    # For GCC/Clang
    target_compile_options(ch8_core PRIVATE -Wall -Wextra -Werror)
    target_compile_options(ch8emu PRIVATE -Wall -Wextra -Werror)
endif()

# Copy rom files to build folder after building
# Define the assets folder
set(ROM_FILES 
    "rom/IBM Logo.ch8"
    "rom/1-chip8-logo.ch8"
    "rom/2-ibm-logo.ch8"
    "rom/3-corax+.ch8"
    "rom/4-flags.ch8"
    "rom/5-quirks.ch8"
    "rom/6-keypad.ch8"
    "rom/7-beep.ch8"
    "rom/8-scrolling.ch8"
)

# Copy assets to the output directory where the .exe lives
foreach(FILE ${ROM_FILES})
    add_custom_command(
        TARGET ch8emu POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}"
                "$<TARGET_FILE_DIR:ch8emu>/${FILE}"
    )
endforeach()

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
