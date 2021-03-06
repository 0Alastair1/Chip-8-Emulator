cmake_minimum_required(VERSION 3.8)

set(CMAKE_C_EXTENSIONS OFF)
set(C_EXTENSIONS OFF)

project(Chip-8-Emulator)

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(Vendor/SDL2)
add_subdirectory(Vendor/SDL_ttf)
add_subdirectory(Vendor/portaudio)
add_subdirectory(Vendor/plibsys)
add_subdirectory(Vendor/libsamplerate)

add_executable(Chip-8-Emulator Chip-8-Emulator/emulator.c Chip-8-Emulator/main.c Chip-8-Emulator/filehandler.c Vendor/tinyfd/tinyfiledialogs.c Chip-8-Emulator/audio.c)

include_directories(Chip-8-Emulator PRIVATE Chip-8-Emulator/include/ Vendor/SDL_ttf/ Vendor/SDL2/include/ Vendor/SDL_ttf/ Vendor/tinyfd/ Vendor/libsamplerate/include/ Vendor/portaudio/include/)

target_link_libraries(Chip-8-Emulator SDL2-static)
target_link_libraries(Chip-8-Emulator SDL2_ttf)
target_link_libraries(Chip-8-Emulator PortAudio)
target_link_libraries(Chip-8-Emulator plibsysstatic)
target_link_libraries(Chip-8-Emulator samplerate)

#use c11 for atomics and inline
set_target_properties(Chip-8-Emulator PROPERTIES C_STANDARD 11 C_EXTENSIONS OFF)
set_property(TARGET Chip-8-Emulator PROPERTY C_STANDARD 11)
target_compile_features(Chip-8-Emulator PUBLIC c_std_11)

