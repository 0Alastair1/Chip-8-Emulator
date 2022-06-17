
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

    //cpu loop
    while (true)
    {
        uint16_t opcode = memory[PC] << 8 | memory[PC + 1];
        printf("%04x %04x\n", opcode, PC);
        switch(opcode)
        {
            case 0x0000:
                PC += 2;
                break;

            


           case 0x00E0:
                //todo later

                PC += 2;
                break;

            case 0x00EE:
                PC = stack[SP];
                SP--;
                break;




            default:
                PC += 2;
                break;

        }


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