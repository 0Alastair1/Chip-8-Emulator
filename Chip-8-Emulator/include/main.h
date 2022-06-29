#if defined(_WIN32) || defined(_WIN64)
    #include <malloc.h>
    #define swap_16(x) _byteswap_ushort(x)
    #define swap_32(x) _byteswap_ulong(x)
    #define swap_64(x) _byteswap_uint64(x)
#endif

#if defined(__linux__) || defined(__unix__)
    #include <byteswap.h>
    #include <alloca.h>
    #define swap_16(x) bswap_16(x)
    #define swap_32(x) bswap_32(x)
    #define swap_64(x) bswap_64(x)
#endif

#if defined(__APPLE__)
    #include <libkern/OSByteOrder.h>
    #define swap_16(x) OSSwapInt16(x)
    #define swap_32(x) OSSwapInt32(x)
    #define swap_64(x) OSSwapInt64(x)
#endif

#if defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__sun__)
    #include <sys/types.h>
    #include <sys/endian.h>
    #define swap_16(x) swap16(x)
    #define swap_32(x) swap32(x)
    #define swap_64(x) swap64(x)
#endif

#if defined(__GNUC__)
    #define PACKED( __Declaration__) __Declaration__ __attribute__((__packed__))

#elif defined(_MSC_VER)
    #define PACKED( __Declaration__) __pragma( pack(push, 1)) __Declaration__ __pragma(pack(pop))
#else
    #define PACKED  
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <locale.h>

#include "tinyfiledialogs.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>

#define Uint8 uint8_t
#define Uint16 uint16_t
#define Uint32 uint32_t
#define Uint64 uint64_t
#define Uint128 uint128_t

#define bool uint8_t
#define true 1
#define false 0
#define isTrue(x) (!!(x))

#define x (opcode & 0x0F00) >> 8
#define y (opcode & 0x00F0) >> 4
#define n (opcode & 0x000F)
#define kk (opcode & 0x00FF)
#define nn kk
#define nnn (opcode & 0x0FFF)
#define numFirst (opcode & 0xF000) >> 12
#define numSecond x
#define numThird y
#define numLast n
#define byteFirst (opcode & 0xFF00) >> 8
#define byteLast kk


struct file{
    uint8_t* data;
    uint32_t size;
};

char* openFile();
struct file readFile(char* filepath);
void writeFont();