cmake_minimum_required(VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project(Chip-8-Emulator)

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(Vendor/SDL2)
add_subdirectory(Vendor/SDL_image)
add_subdirectory(Vendor/SDL_ttf)
add_subdirectory(Vendor/plibsys)

add_executable(Chip-8-Emulator emulator.c Vendor/tinyfd/tinyfiledialogs.c)

include_directories(Chip-8-Emulator PRIVATE Vendor/SDL_ttf/ Vendor/SDL2/include/ Vendor/SDL_image/ Vendor/SDL_ttf/ Vendor/tinyfd/ )

target_link_libraries(Chip-8-Emulator SDL2-static)
target_link_libraries(Chip-8-Emulator SDL2_image)
target_link_libraries(Chip-8-Emulator SDL2_ttf)
target_link_libraries(Chip-8-Emulator plibsys)