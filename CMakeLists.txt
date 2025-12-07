cmake_minimum_required(VERSION 3.10)
project(simanneal C)
set(CMAKE_C_STANDARD 99)

option(DEBUG OFF)

if (DEBUG MATCHES ON)
    message("Compiling in DEBUG mode...")
    set(CMAKE_C_FLAGS "-O0 -g -Wall -Wextra")
else()
    message("Compiling in RELEASE mode...")
    set(CMAKE_C_FLAGS "-O2 -Wall -Wextra")
endif()

add_subdirectory(src)

option(EXAMPLES OFF)
if (EXAMPLES MATCHES ON)
    message("Enabled compiling examples...")
    add_subdirectory(src/examples)
endif()
