cmake_minimum_required(VERSION 3.8)

project(Chip-8-Emulator)

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(Vendor/SDL2)
add_subdirectory(Vendor/SDL_ttf)

add_executable(Chip-8-Emulator emulator.c Vendor/tinyfd/tinyfiledialogs.c)

include_directories(Chip-8-Emulator PRIVATE Vendor/SDL_ttf/ Vendor/SDL2/include/ Vendor/SDL_ttf/ Vendor/tinyfd/ )

target_link_libraries(Chip-8-Emulator SDL2-static)
target_link_libraries(Chip-8-Emulator SDL2_ttf)

#set_property(TARGET Chip-8-Emulator PROPERTY C_STANDARD 90)
#set_property(TARGET Chip-8-Emulator PROPERTY C_STANDARD_REQUIRED ON)