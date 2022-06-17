
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

#define Uint8 uint8_t
#define Uint16 uint16_t
#define Uint32 uint32_t
#define Uint64 uint64_t

inline bool isLittleEndian()
{
    Uint16 val = 0xFF00;
    return *(Uint8*)&val == 0xFF;
}

bool chip8SuperMode();

Uint8 Width = 128;
Uint8 Height = 64;


struct file{
    Uint8* data;
    uint32_t size;
};


//https://en.wikipedia.org/wiki/CHIP-8
//http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
//https://tobiasvl.github.io/blog/write-a-chip-8-emulator
//http://johnearnest.github.io/Octo/docs/SuperChip.html

void cpuLoop(Uint8*, uint32_t);
char* openFile();
struct file readFile(char*);

int main(int argc, char const *argv[])
{
    char* filepath = openFile();
    struct file file = readFile(filepath);
    cpuLoop(file.data, file.size);
    return 0;
}

void cpuLoop(Uint8* data, uint32_t size)
{
    Uint16 PC;
    Uint8 SP;
    Uint8 i;
    Uint8* memory = alloca(4096);
    Uint8 V[16];
    Uint16 stack[16];
    Uint8 delayTimer;
    Uint8 soundTimer;

    //copy file data to memory
    memcpy(memory, data, size);

    //starting at 0x00
    PC = 0x00;
    SP = 0x00;

    for(i = 0; i < 16; i++)
    {
        V[i] = 0;
        stack[i] = 0;
    }
    delayTimer = 0;
    soundTimer = 0;

    /*
    #pragma push();
    #pragma pack(1)
    union{
        Uint16 opcode;
        struct PACKED{
            Uint8 two:2;
            Uint8 four:2;
            Uint8 six:2;
            Uint8 eight:2;
            Uint8 ten:2;
            Uint8 twelve:2;
            Uint8 forteen:2;
            Uint8 sixteen:2;
        }bits;
    } opcode;
    #pragma pop();
    */

    Uint16 opcode;

    //cpu loop
    while (true)
    {
        //fetch opcode
        if(isLittleEndian)
        {
            opcode = memory[PC] << 8 | memory[PC + 1];
        }
        else
        {
            opcode = *(Uint16*)&memory[PC];
        }

        printf("%04x %04x\n", opcode, PC);
        
        //decode and execute opcode

        /* superchip 8 instructions block */
        if(chip8SuperMode)
        {
            //0x00FB
            if(opcode == 0x00FB)
            {
                //todo later

            }
            
            //0x00FC
            if(opcode == 0x00FC)
            {
                //todo later

            }

            //0x00FD
            if(opcode == 0x00FD)
            {
                //todo later

            }

            //0x00FE
            if(opcode == 0x00FE)
            {
                //todo later

            }

            //0x00FF
            if(opcode == 0x00FF)
            {
                //todo later

            }
        }
         /* end superchip 8 instructions block */

        //0x00E0
        else if(opcode == 0x00E0)
        {
            //todo later

        }

        //0x00EE
        else if(opcode == 0x00EE)
        {
            PC = stack[SP];
            SP--;

        }

        //0x00CN - superchip 8 instruction
        else if( ((opcode >> 4) << 4) == 0x00C0 && chip8SuperMode)
        {
            //todo later

        }

        //0NNN - checkme
        else if(opcode >> 12 == 0x0)
        {
            PC += 1;
            
            PC = memory[PC];

        }

        //1NNN
        else if(opcode >> 12 == 0x1)
        {
            PC = opcode & 0x0FFF;
        }

        //2NNN
        else if(opcode >> 12 == 0x2)
        {
            SP += 1;
            stack[SP] = PC;
            PC = opcode & 0x0FFF;
        }

        //3XKK
        else if(opcode >> 12 == 0x3)
        {
            PC += 1;

            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
            {
                PC += 1;
            }
        }

        //4XKK
        else if(opcode >> 12 == 0x4)
        {
            PC += 1;

            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
            {
                PC += 1;
            }
        }

        //5XY0
        else if(opcode >> 12 == 0x5 && (opcode & 0x000F) == 0x0)
        {
            //todo later

        }
        
        //6XNN
        else if(opcode >> 12 == 0x6)
        {
            //todo later

        }

        //7XNN
        else if(opcode >> 12 == 0x7)
        {
            //todo later

        }

        //8XY0
        else if(opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x0)
        {
            //todo later

        }

        //8XY1
        else if(opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x1)
        {
            //todo later

        }

        //8XY2
        else if(opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x2)
        {
            //todo later

        }

        //8XY3
        else if(opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x3)
        {
            //todo later

        }

        //8XY4
        else if(opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x4)
        {
            //todo later

        }

        //8XY5
        else if(opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x5)
        {
            //todo later

        }

        //8XY6
        else if(opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x6)
        {
            //todo later

        }

        //8XY7
        else if(opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x7)
        {
            //todo later

        }

        //8XYE
        else if(opcode >> 12 == 0x8 && (opcode & 0x000F) == 0xE)
        {
            //todo later

        }
        
        //9XY0
        else if(opcode >> 12 == 0x9 && (opcode & 0x000F) == 0x0)
        {
            //todo later

        }

        //ANNN
        else if(opcode >> 12 == 0xA)
        {
            //todo later

        }

        //BNNN
        else if(opcode >> 12 == 0xB)
        {
            //todo later

        }

        //CXNN
        else if(opcode >> 12 == 0xC)
        {
            //todo later

        }

        //0xDXY0 - superchip 8 instruction
        else if(opcode >> 12 == 0xD && (opcode & 0x000F) == 0x0 && chip8SuperMode)
        {
            //todo later

        }


        //DXYN
        else if(opcode >> 12 == 0xD)
        {
            //todo later

        }

        //EX9E
        else if(opcode >> 12 == 0xE && (opcode & 0x00FF) == 0x9E)
        {
            //todo later

        }

        //EXA1
        else if(opcode >> 12 == 0xE && (opcode & 0x00FF) == 0xA1)
        {
            //todo later

        }

        //FX07
        else if(opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x07)
        {
            //todo later

        }

        //FX0A
        else if(opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x0A)
        {
            //todo later

        }

        //FX15
        else if(opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x15)
        {
            //todo later

        }

        //FX18
        else if(opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x18)
        {
            //todo later

        }

        //FX1E
        else if(opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x1E)
        {
            //todo later

        }

        //FX29
        else if(opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x29)
        {
            //todo later

        }

        //FX30 - superchip 8 instruction
        else if(opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x30 && chip8SuperMode)
        {
            //todo later

        }

        //FX33
        else if(opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x33)
        {
            //todo later

        }

        //FX55
        else if(opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x55)
        {
            //todo later

        }

        //FX65
        else if(opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x65)
        {
            //todo later

        }

        //FX75 - superchip 8 instruction
        else if(opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x75 && chip8SuperMode)
        {
            //todo later

        }

        //FX85 - superchip 8 instruction
        else if(opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x85 && chip8SuperMode)
        {
            //todo later

        }

        else
        {
            printf("Unhandled opcode: %X\n At address: %X\n", opcode, PC);
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

    Uint8* fileContent = malloc(file.size) + 1;

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