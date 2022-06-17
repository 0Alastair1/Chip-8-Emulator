
#ifdef _WIN32
    #include <windows.h>
    #include <malloc.h>
    #define swap_16(x) _byteswap_ushort(x)
    #define swap_32(x) _byteswap_ulong(x)
    #define swap_64(x) _byteswap_uint64(x)
#endif

#ifdef __linux__
    #include <byteswap.h>
    #include <alloca.h>
    #define swap_16(x) bswap_16(x)
    #define swap_32(x) bswap_32(x)
    #define swap_64(x) bswap_64(x)

#endif

#if defined(__GNUC__)
    #define PACKED( __Declaration__) __Declaration__ __attribute__((__packed__))

#elif defined(_MSC_VER)
    #define PACKED( __Declaration__) __pragma( pack(push, 1)) __Declaration__ __pragma(pack(pop))
#endif


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

uint8_t Width = 128;
uint8_t Height = 64;


struct file{
    uint8_t* data;
    uint32_t size;
};

//https://en.wikipedia.org/wiki/CHIP-8
//http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
//https://tobiasvl.github.io/blog/write-a-chip-8-emulator

void cpuLoop(uint8_t*, uint32_t);
char* openFile();
struct file readFile(char*);

int main(int argc, char const *argv[])
{
    char* filepath = openFile();
    struct file file = readFile(filepath);
    cpuLoop(file.data, file.size);
    return 0;
}

void cpuLoop(uint8_t* data, uint32_t size)
{
    uint8_t PC;
    uint8_t SP;
    uint8_t i;
    uint8_t* memory = alloca(4096);
    uint8_t v[16];
    uint16_t stack[16];
    uint8_t delayTimer;
    uint8_t soundTimer;

    //copy file data to memory
    memcpy(memory, data, size);

    //starting at 0x00
    PC = 0x00;
    SP = 0x00;

    for(i = 0; i < 16; i++)
    {
        v[i] = 0;
        stack[i] = 0;
    }
    delayTimer = 0;
    soundTimer = 0;

    #pragma push();
    #pragma pack(1)
    union{
        uint16_t opcode;
        struct PACKED{
            uint8_t two:2;
            uint8_t four:2;
            uint8_t six:2;
            uint8_t eight:2;
            uint8_t ten:2;
            uint8_t twelve:2;
            uint8_t forteen:2;
            uint8_t sixteen:2;
        };
    }s;
    #pragma pop();

    //cpu loop
    while (true)
    {
        s.opcode = memory[PC] << 8 | memory[PC + 1];
        printf("%04x %04x\n", s.opcode, PC);
        

        //if()


    }

    return;
}

char* openFile()
{
    char* filepath = malloc(64);
    #ifdef _WIN32
        OPENFILENAME  ofn;        
        memset(&ofn,0,sizeof(ofn));
        ofn.lStructSize     = sizeof(ofn);
        ofn.hwndOwner       = NULL; 
        ofn.lpstrFile       = filepath;
        ofn.nMaxFile        = MAX_PATH;
        ofn.lpstrTitle      = L"Open File";
        ofn.Flags           = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    #else
        //no gui D:
        printf("Please enter the filepath: ");
        #ifdef __STDC_LIB_EXT1__
            scanf_s("%s", filepath, 64);
        #else
        scanf("%s", filepath);
        #endif
    #endif

    if(!strlen(filepath))
    {
        printf("No filepath.\n");
        exit(1);
    }
    
    return filepath;
}

struct file readFile(char* filepath)
{
    struct file file;
    FILE* fp;

    #ifdef __STDC_LIB_EXT1__
        fopen_s(&fp, filepath, "rb");
    #else
        fp = fopen(filepath, "rb");
    #endif

    if (fp == NULL)
    {
        printf("Can't open file.\n");
        exit(1);
    }
    free(filepath);

    fseek(fp, 0, SEEK_END);
    file.size = ftell(fp);

    uint8_t* fileContent = malloc(file.size) + 1;

    fseek(fp, 0, SEEK_SET);
    fread(fileContent, 1, file.size, fp);

    fclose(fp);

    file.data = fileContent;

    if(!file.size)
    {
        printf("Empty file.\n");
        exit(1);
    }

    return file;
}