cmake_minimum_required(VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project(Chip-8-Emulator)

add_subdirectory(Vendor/SDL2)

add_executable(Chip-8-Emulator main.c)

include_directories(Chip-8-Emulator PRIVATE Vendor/SDL2/include Vendor/tinyfd/ )

target_link_libraries(Chip-8-Emulator SDL2-static)
