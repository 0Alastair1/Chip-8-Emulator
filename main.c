
#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <locale.h>

#include "Vendor/tinyfd/tinyfiledialogs.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define Uint8 uint8_t
#define Uint16 uint16_t
#define Uint32 uint32_t
#define Uint64 uint64_t
#define Uint128 uint128_t

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

inline bool isLittleEndian()
{
    Uint16 val = 0xFF00;
    return *(Uint8*)&val == 0xFF;
}

struct file{
    Uint8* data;
    uint32_t size;
};

struct strangeTypeSizes
{
    struct
    {
        Uint16 i: 12;
        bool blue: 1;
        bool black: 1;
        bool green: 1;
        bool red: 1;
    };
    
};
#define I (typeSizesStruct.i)


//https://en.wikipedia.org/wiki/CHIP-8
//http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
//https://tobiasvl.github.io/blog/write-a-chip-8-emulator
//http://johnearnest.github.io/Octo/docs/SuperChip.html
//https://www.freecodecamp.org/news/creating-your-very-own-chip-8-emulator/
//http://www.emulator101.com/chip-8-sprites.html
//https://github.com/trapexit/chip-8_documentation/blob/master/README.md
//https://github.com/JohnEarnest/Octo/blob/gh-pages/docs/XO-ChipSpecification.md

//font
const Uint8 font[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};                                              


void cpuLoop();
char* openFile();
struct file readFile(char*);


#define width_s 128
#define height_s 64
#define scale 5

#pragma push()
#pragma pack(1)
union
{
    Uint16 keyboard;

    struct PACKED
    {
        Uint8 key1: 1; 
        Uint8 key2: 1; 
        Uint8 key3: 1; 
        Uint8 keyC: 1;
        Uint8 key4: 1; 
        Uint8 key5: 1; 
        Uint8 key6: 1; 
        Uint8 keyD: 1;
        Uint8 key7: 1; 
        Uint8 key8: 1; 
        Uint8 key9: 1; 
        Uint8 keyE: 1;
        Uint8 keyA: 1; 
        Uint8 key0: 1; 
        Uint8 keyB: 1; 
        Uint8 keyF: 1;
    } keys;
} keyboard;
#pragma pop()



void initSDL(SDL_Window** window, SDL_Renderer** renderer)
{
    SDL_Init(SDL_INIT_VIDEO);
    *window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_s * scale, height_s * scale, SDL_WINDOW_SHOWN);
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

    return;
}

Uint8* data;
uint32_t size;

void startingUi(SDL_Window* window, SDL_Renderer* renderer, bool* sChip8Mode, bool* xoChipMode,
    bool* chip8HdMode, bool* chip10Mode, bool* chip8IMode, bool* chip8EMode, bool* chip8XMode)
{
    int width = width_s * scale;
    int height = height_s * scale;
    while(true)
    {


        //create ui tick buttons and text
        int spacingy = 35;
        int spacingx = 35;
        int buttonsx = width /2 + width /6;

        SDL_Rect noModebutton = {buttonsx, spacingy, 10, 10};

        SDL_Rect sChip8ModeButton = {buttonsx, spacingy*2, 10, 10};

        SDL_Rect xoChipModeButton = {buttonsx, spacingy*3, 10, 10};

        SDL_Rect chip8HdModeButton = {buttonsx, spacingy*4, 10, 10};

        SDL_Rect chip10ModeButton = {buttonsx, spacingy*5, 10, 10};

        SDL_Rect chip8IModeButton = {buttonsx, spacingy*6 , 10, 10};

        SDL_Rect chip8EModeButton = {buttonsx, spacingy*7 , 10, 10};

        SDL_Rect chip8XModeButton = {buttonsx, spacingy*8 , 10, 10};


        TTF_Font* Sans;// = TTF_OpenFont("", 24);.//fixme

        if(!font) {
            printf("font not found\n");
            exit(1);
        }
        SDL_Color textColor = {255, 255, 255};

        SDL_Surface* textSurface = TTF_RenderText_Solid(Sans, "Original mode", textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {buttonsx + spacingx, spacingy, textSurface->w, textSurface->h};

        SDL_Surface* textSurface2 = TTF_RenderText_Solid(Sans, "sChip mode", textColor);
        SDL_Texture* textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
        SDL_Rect textRect2 = {buttonsx + spacingx, spacingy*2, textSurface2->w, textSurface2->h};

        SDL_Surface* textSurface3 = TTF_RenderText_Solid(Sans, "XOChip mode", textColor);
        SDL_Texture* textTexture3 = SDL_CreateTextureFromSurface(renderer, textSurface3);
        SDL_Rect textRect3 = {buttonsx + spacingx, spacingy*3, textSurface3->w, textSurface3->h};

        SDL_Surface* textSurface4 = TTF_RenderText_Solid(Sans, "Chip8HD mode", textColor);
        SDL_Texture* textTexture4 = SDL_CreateTextureFromSurface(renderer, textSurface4);
        SDL_Rect textRect4 = {buttonsx + spacingx, spacingy*4, textSurface4->w, textSurface4->h};

        SDL_Surface* textSurface5 = TTF_RenderText_Solid(Sans, "Chip10 mode", textColor);
        SDL_Texture* textTexture5 = SDL_CreateTextureFromSurface(renderer, textSurface5);
        SDL_Rect textRect5 = {buttonsx + spacingx, spacingy*5, textSurface5->w, textSurface5->h};

        SDL_Surface* textSurface6 = TTF_RenderText_Solid(Sans, "Chip8I mode", textColor);
        SDL_Texture* textTexture6 = SDL_CreateTextureFromSurface(renderer, textSurface6);
        SDL_Rect textRect6 = {buttonsx + spacingx, spacingy*6, textSurface6->w, textSurface6->h};

        SDL_Surface* textSurface7 = TTF_RenderText_Solid(Sans, "Chip8E mode", textColor);
        SDL_Texture* textTexture7 = SDL_CreateTextureFromSurface(renderer, textSurface7);
        SDL_Rect textRect7 = {buttonsx + spacingx, spacingy*7, textSurface7->w, textSurface7->h};

        SDL_Surface* textSurface8 = TTF_RenderText_Solid(Sans, "Chip8X mode", textColor);
        SDL_Texture* textTexture8 = SDL_CreateTextureFromSurface(renderer, textSurface8);
        SDL_Rect textRect8 = {buttonsx + spacingx, spacingy*8, textSurface8->w, textSurface8->h};
        

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        SDL_FreeSurface(textSurface2);
        SDL_DestroyTexture(textTexture2);

        SDL_FreeSurface(textSurface3);
        SDL_DestroyTexture(textTexture3);

        SDL_FreeSurface(textSurface4);
        SDL_DestroyTexture(textTexture4);

        SDL_FreeSurface(textSurface5);
        SDL_DestroyTexture(textTexture5);

        SDL_FreeSurface(textSurface6);
        SDL_DestroyTexture(textTexture6);

        SDL_FreeSurface(textSurface7);
        SDL_DestroyTexture(textTexture7);

        SDL_FreeSurface(textSurface8);
        SDL_DestroyTexture(textTexture8);
        
        
        TTF_CloseFont(Sans);

        

        SDL_Rect* buttons[9] = {&noModebutton, &sChip8ModeButton, &xoChipModeButton, &chip8HdModeButton, &chip10ModeButton, &chip8IModeButton, &chip8EModeButton, &chip8XModeButton};

        SDL_Rect* textRects[9] = {&textRect, &textRect2, &textRect3, &textRect4, &textRect5, &textRect6, &textRect7, &textRect8};

        SDL_Texture* textTextures[9] = {textTexture, textTexture2, textTexture3, textTexture4, textTexture5, textTexture6, textTexture7, textTexture8};

        //draw ui
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        //draw all buttons in the list
        for(int i = 0; i < 8; i++)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, buttons[i]);
        }
        //draw all text in the list
        for(int i = 0; i < 8; i++)
        {
            SDL_RenderCopy(renderer, textTextures[i], NULL, textRects[i]);
        }


        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);


        SDL_RenderPresent(renderer);



        //input
        //get mouse clicks
        SDL_Event event;
        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT)
        {
            exit(0);
        }
        //mouse left click
        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
            int mouseX; 
            int mouseY;
            if(event.button.button == SDL_BUTTON_LEFT)
            {
                //handle inputs
                SDL_GetMouseState(&mouseX, &mouseY);
                if(mouseX >= 0 && mouseX <= width_s * scale && mouseY >= 0 && mouseY <= height_s * scale)
                {

                    char* filepath = openFile();
                    struct file file = readFile(filepath);
                    data = file.data;
                    size = file.size;

                    SDL_RenderClear(renderer);

                    return;
                }
            }
        }
    }
}
int main(int argc, char const *argv[])
{

    
    cpuLoop();
    return 0;
}

void inputs()
{

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
            exit(0);
        

        if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_1:
                    keyboard.keys.key1 = 1;
                    break;

                case SDLK_2:
                    keyboard.keys.key2 = 1;
                    break;
                
                case SDLK_3:
                    keyboard.keys.key3 = 1;
                    break;
                    
                case SDLK_4:
                    keyboard.keys.keyC = 1;
                    break;

                case SDLK_q:
                    keyboard.keys.key4 = 1;
                    break;

                case SDLK_w:
                    keyboard.keys.key5 = 1;
                    break;

                case SDLK_e:
                    keyboard.keys.key6 = 1;
                    break;

                case SDLK_r:
                    keyboard.keys.keyD = 1;
                    break;

                case SDLK_a:
                    keyboard.keys.key7 = 1;
                    break;

                case SDLK_s:
                    keyboard.keys.key8 = 1;
                    break;

                case SDLK_d:
                    keyboard.keys.key9 = 1;
                    break;
                
                case SDLK_f:
                    keyboard.keys.keyE = 1;
                    break;

                case SDLK_z:
                    keyboard.keys.keyA = 1;
                    break;

                case SDLK_x:
                    keyboard.keys.key0 = 1;
                    break;

                case SDLK_c:
                    keyboard.keys.keyB = 1;
                    break;

                case SDLK_v:
                    keyboard.keys.keyF = 1;
                    break;

                case SDLK_ESCAPE:
                    exit(0);
                    break;

                default:
                    break;
            }
        }
        if(event.type == SDL_KEYUP)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_1:
                    keyboard.keys.key1 = 0;
                    break;

                case SDLK_2:
                    keyboard.keys.key2 = 0;
                    break;
                
                case SDLK_3:
                    keyboard.keys.key3 = 0;
                    break;
                    
                case SDLK_4:
                    keyboard.keys.keyC = 0;
                    break;

                case SDLK_q:
                    keyboard.keys.key4 = 0;
                    break;

                case SDLK_w:
                    keyboard.keys.key5 = 0;
                    break;

                case SDLK_e:
                    keyboard.keys.key6 = 0;
                    break;

                case SDLK_r:
                    keyboard.keys.keyD = 0;
                    break;

                case SDLK_a:
                    keyboard.keys.key7 = 0;
                    break;

                case SDLK_s:
                    keyboard.keys.key8 = 0;
                    break;

                case SDLK_d:
                    keyboard.keys.key9 = 0;
                    break;
                
                case SDLK_f:
                    keyboard.keys.keyE = 0;
                    break;

                case SDLK_z:
                    keyboard.keys.keyA = 0;
                    break;

                case SDLK_x:
                    keyboard.keys.key0 = 0;
                    break;

                case SDLK_c:
                    keyboard.keys.keyB = 0;
                    break;

                case SDLK_v:
                    keyboard.keys.keyF = 0;
                    break;

                case SDLK_ESCAPE:
                    exit(0);
                    break;

                default:
                    break;
            }
        }
    }
}

void cpuLoop()
{
    Uint16 PC;
    Uint8 SP;
    Uint8* memory;
    Uint8 V[16];
    Uint16 stack[32];
    Uint8 delayTimer;
    Uint8 soundTimer;
    struct strangeTypeSizes typeSizesStruct;
    bool ETI660Mode = false;

    SDL_Window* window;
    SDL_Renderer* renderer;

    bool sChip8Mode = false;

    bool xoChipMode = true;

    bool chip8HdMode = false;

    bool chip10Mode = false;

    bool chip8IMode = false;

    bool chip8EMode = false;

    bool chip8XMode = false;

    bool mode3232 = false;
    bool mode6464 = false;
    bool mode12864 = false;
    bool iToggle = false;
    
    
    
    Uint8 currentKeyPressed = 255;
    Uint16 opcode;

    //initialize SDL and store the result
    initSDL(&window, &renderer);

    SDL_ShowWindow(window);

    //fixme get flags from user input
    //draw starting ui screen
    Uint8* objects;
    startingUi(window, renderer, &sChip8Mode, &xoChipMode, &chip8HdMode, &chip10Mode, &chip8IMode, &chip8EMode, &chip8XMode);

    //configure chip8 versions according to flags
    if(sChip8Mode)
    {   //im guessing super chip doesnt start in high res
        mode12864 = false;
    }

    if(chip10Mode)
    {
        mode12864 = true;
    }

    if(chip8HdMode)
    {
        mode3232 = true;
    }

    //set all pixels to 0
    Uint8 screen[width_s][height_s];
    for (size_t i = 0; i < width_s; i++)
    {   
        for (size_t ii = 0; ii < height_s; ii++)
        {
            screen[i][ii] = 0;
        }
    }

    ///

    //copy file data to memory
    if(size + 1024 < 8192)
    {
        memory = alloca(size + 1024);
    }
    else
    {
        memory = malloc(size + size/2);
    }
    
    if(!ETI660Mode)
    {
        memcpy(memory+0x200, data, size);
        PC = (Uint16)0x200;
    }
    else
    {
        memcpy(memory+0x600, data, size);
        PC = (Uint16)0x600;
    }

    SP = 0x00;
    
    //copy font to memory
    memcpy(memory, font, 80);

    for(size_t ii = 0; ii < 16; ii++)
    {
        V[ii] = 0;
        stack[ii] = 0;
    }
    delayTimer = 0;
    soundTimer = 0;


    Uint8 Width = 128;
    Uint8 Height = 64;

    //cpu loop
    //todo - check conflics caused by overlapping instructions
    while (true)
    {

        //sleep fixme
        SDL_Delay(1);

        //fetch opcode
        if(isLittleEndian)
        {
            opcode = swap_16( (*(Uint16*)&memory[PC]) & 0xFFFF);
        }
        else
        {
            opcode = (*(Uint16*)&memory[PC] & 0xFFFF);
        }

        printf("%04x %04x\n", opcode, PC);
        
        //decode and execute opcode

        {
            //001N - aditional
            if(byteFirst == 0x00 && numThird == 0x1)
            {
                exit(n);
            }

            //0x00CN
            //move each pixel in screen down by n pixels
            else if(byteFirst == 0x00 && numThird == 0xC && sChip8Mode)
            {
                PC += 2;

                //check me
                for (size_t i = height_s; i > 0; i--)
                {   
                    if(i - n < 0)//check me
                    {
                        for (size_t ii = 0; ii < width_s; ii++)
                        {
                            //screen[ii][i] = screen[ii][height_s - (i - n)]; // check fix, does this wrap here?
                            screen[ii][i] = 0;
                        }
                         
                    }
                    else{
                        for (size_t ii = 0; ii < width_s; ii++)
                        {
                            screen[ii][i] = screen[ii][i-n];
                        }
                    }   
                }
            }

            //0x00DN
            else if(byteFirst == 0x00 && numThird == 0xD && xoChipMode)
            {
                PC += 2;

                //todo later

            }

            //0x00E0
            else if(opcode == 0x00E0)
            {
                PC += 2;
                
                for (size_t i = 0; i < width_s; i++)
                {   
                    for (size_t ii = 0; ii < height_s; ii++)
                    {
                        screen[i][ii] = 0;
                    }
                }
            }

            //0x00EE
            else if(opcode == 0x00EE)
            {
                PC = stack[SP];
                SP--;
            
            }

            else if(opcode == 0x00FA) //
            {
                PC += 2;

                iToggle = !iToggle;
                break;
            }
            //0x00FB - chip8 super instrcution
            else if(opcode == 0x00FB && sChip8Mode)
            {
                PC += 2;

                //scroll display 4 pixels to the right
                for(size_t i = 0; i < width_s; i++)
                {
                    if(i - 4 <= 0)
                    {
                        for(size_t ii = 0; ii < height_s; ii++)
                        {
                            screen[i][ii] = 0;
                        }
                    }
                    else
                    {
                        for(size_t ii = 0; ii < height_s; ii++)
                        {
                            screen[i][ii] = screen[i-4][ii];
                        }
                    }
                }
            }
            
            //0x00FC - chip8 super instruction
            else if(opcode == 0x00FC && sChip8Mode)
            {
                PC += 2;

                //scroll display 4 pixels to the left
                for(size_t i = 0; i < width_s; i++)
                {
                    if(i + 4 >= width_s)
                    {
                        for(size_t ii = 0; ii < height_s; ii++)
                        {
                            screen[i][ii] = 0;
                        }
                    }
                    else
                    {
                        for(size_t ii = 0; ii < height_s; ii++)
                        {
                            screen[i][ii] = screen[i+4][ii];
                        }
                    }
                    
                }

            }

            //0x00FD - chip8 super instruction
            else if(opcode == 0x00FD && sChip8Mode)
            {
                exit(0);
            }

            //0x00FE - chip8 super instruction
            else if(opcode == 0x00FE && sChip8Mode)
            {
                PC += 2;

                
                mode12864 = false;
                //todo later

            }

            //0x00FF - chip8 super instruction
            else if(opcode == 0x00FF && sChip8Mode)
            {
                //checkme
                PC += 2;

                mode12864 = true;
            }
            
            /*
            else if(opcode == 0x0000)
            {
                PC += 2;
            }
            */

           //0x02A0 - extended
            else if(opcode == 0x02A0 && chip8XMode)
            {
                //PC += 2;
                
                if(typeSizesStruct.blue == true)
                {
                    typeSizesStruct.blue = false;
                    typeSizesStruct.black = true;
                }
                else if(typeSizesStruct.black == true)
                {
                    typeSizesStruct.black = false;
                    typeSizesStruct.green = true;
                }
                else if(typeSizesStruct.green == true)
                {
                    typeSizesStruct.green = false;
                    typeSizesStruct.red = true;
                }
                else if(typeSizesStruct.red == true)
                {
                    typeSizesStruct.red = false;
                    typeSizesStruct.blue = true;
                }
            }

            //0NNN - checkme
            else if(numFirst == 0x0)
            {
                PC += 2;
                //

                //todo later

            }

            //1NNN
            else if(numFirst == 0x1)
            {
                PC = nnn;
            }

            //2NNN
            else if(numFirst == 0x2)
            {
                PC += 2;

                SP += 1;
                stack[SP] = PC;

                PC = nnn;
            }

            //3XKK
            else if(numFirst == 0x3)
            {
                PC += 2;

                if(V[x] == kk)
                {
                    PC += 2;
                }
            }

            //4XKK
            else if(numFirst == 0x4)
            {
                PC += 2;

                if(V[x] != kk)
                {
                    PC += 2;
                }
            }

            //5XY0
            else if(numFirst == 0x5 && numLast == 0x0)
            {
                PC += 2;

                if(V[x] == V[y])
                {
                    PC += 2;
                }

            }

            //5XY1
            else if(numFirst == 0x5 && numLast == 0x1 && chip8EMode)
            {
                PC += 2;

                if(V[x] > V[y])
                {
                    PC += 2;
                }
            }

            //5XY1
            else if(numFirst == 0x5 && numLast == 0x1 && chip8XMode)
            {
                //checkme /fixme

                PC += 2;
    
                V[x] += V[y];
            }

            //5XY2
            else if(numFirst == 0x5 && numLast == 0x2 && (chip8EMode || chip8XMode))
            {
                if(chip8EMode)
                {
                    PC += 2;

                    if(V[x] < V[y])
                    {
                        PC += 2;
                    }
                }
                else{
                    PC += 2;

                    //todo later
                }
            }
                
            
            //5XY3
            else if(numFirst == 0x5 && numLast == 0x3 && (chip8EMode || chip8XMode))
            {
                if(chip8EMode)
                {
                    PC += 2;

                    if(V[x] != V[y])
                    {
                        PC += 2;
                    }
                }
                else{

                    PC += 2;

                    //todo later
                }
            }

            //6XNN
            else if(numFirst == 0x6)
            {
                PC += 2;

                V[x] = kk;

            }

            //7XNN
            else if(numFirst == 0x7)
            {
                PC += 2;

                V[x] += kk;
            }

            //8XY0
            else if(numFirst == 0x8 && numLast == 0x0)
            {
                PC += 2;

                V[x] = V[y];

            }

            //8XY1
            else if(numFirst == 0x8 && numLast == 0x1)
            {
                PC += 2;

                if(!sChip8Mode && !xoChipMode)
                {
                    V[0xF] = 0;
                }

                V[x] = V[x] | V[y];
            }

            //8XY2
            else if(numFirst == 0x8 && numLast == 0x2)
            {
                PC += 2;

                if(!sChip8Mode && !xoChipMode)
                {
                    V[0xF] = 0;
                }

                V[x] = V[x] & V[y];
            }

            //8XY3
            else if(numFirst == 0x8 && numLast == 0x3)
            {
                PC += 2;

                if(!sChip8Mode && !xoChipMode)
                {
                    V[0xF] = 0;
                }

                V[x] = V[x] ^ V[y];
            }

            //8XY4
            else if(numFirst == 0x8 && numLast == 0x4)
            {
                PC += 2;

                V[0xF] = ((V[x] + V[y]) > 0xFF) ? 1 : 0;
                V[x] = (V[x] + V[y]); //checkme
            }

            //8XY5
            else if(numFirst == 0x8 && numLast == 0x5)
            {
                PC += 2;

                V[0xF] = (V[y] > V[x]) ? 0 : 1;
                V[x] = V[x] - V[y];
            }

            //8XY6
            else if(numFirst == 0x8 && numLast == 0x6)
            {
                PC += 2;

                if(!sChip8Mode)
                {
                    V[x] = V[y];
                }
                
                V[0xF] = (V[x] & 0x1) ? 1 : 0;
                V[x] >>= 1;
            }

            //8XY7
            else if(numFirst == 0x8 && numLast == 0x7)
            {
                PC += 2;

                V[0xF] = (V[x] > V[y]) ? 0 : 1;
                V[x] = V[y] - V[x];
            }

            //8XYE
            else if(numFirst == 0x8 && numLast == 0xE)
            {
                PC += 2;

                if(!sChip8Mode)
                {
                    V[x] = V[y];
                }

                V[0xF] = (V[x] >> 7) & 0x1;
                V[x] <<= 1;
            }
            
            //9XY0
            else if(numFirst == 0x9 && numLast == 0x0)
            {
                PC += 2;

                if(V[x] != V[y])
                {
                    PC += 2;
                }
            }

            //9XY1
            else if(numFirst == 0x9 && numLast == 0x1 && chip8EMode)
            {
                PC += 2;
                
                //check me
                V[0xF] = (V[x] * V[y]) >> 8;
                V[x] = V[x] * V[y];

            }

            //9XY2 - extended
            else if(numFirst == 0x9 && numLast == 0x2 && chip8EMode)
            {
                PC += 2;

                V[0xF] = V[x] % V[y];
                V[x] = V[x] / V[y];
            }

            //9XY3 - extended
            else if(numFirst == 0x9 && numLast == 0x3 && chip8EMode)
            {
                PC += 2;
                
                //check me/fixme
                Uint16 temp = V[x] << 8 | V[y];

                memory[I] = (temp / 10000) % 10;
                memory[I + 1] = (temp / 1000) % 10;
                memory[I + 2] = (temp / 100) % 10;
                memory[I + 3] = (temp / 10) % 10;
                memory[I + 4] = (temp / 1) % 10;
            }

            //ANNN
            else if(numFirst == 0xA)
            {
                PC += 2;

                I = nnn;
            }

            //B0NN
            else if(byteFirst == 0xB && chip8IMode)
            {
                PC += 2;

                //todo later
            }

            //B1Y0
            else if(byteFirst == 0xB1 && numLast == 0x0 && chip8IMode)
            {
                PC += 2;
                //todo later
            }

            //B1Y1
            else if(byteFirst == 0xB1 && numLast == 0x1 && chip8IMode)
            {
                PC += 2;
                //todo later
            }

            //BXY0 - extended 
            else if(numFirst == 0xB && n == 0x0 && chip8XMode)
            {
                PC += 2;
                
                //todo later
            }

            //BXYN
            else if(numFirst == 0xB && chip8XMode)
            {
                PC += 2;
                //todo later
            }

            //BNNN
            else if(numFirst == 0xB)
            {
                if(!sChip8Mode)
                {
                    PC = nnn + V[0];
                }
                else{
                    PC = nnn + V[x];
                }
                
            }

            //CXNN
            else if(numFirst == 0xC)
            {
                PC += 2;

                V[x] = (rand() % 0xFF + 1) & kk;

            }

            //DXY0
            else if(numFirst == 0xD && numLast == 0x0 && sChip8Mode)
            {
                PC += 2;
                
                //todo later

            }


            //DXYN
            else if(numFirst == 0xD)
            {
                PC += 2;

                V[0xF] = 0;

                //render sprite
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
       

                //draw sprite from memory

                //foreach line/byte
                for(size_t row = 0; row < n; row++)
                {
                    //foreach pixel/bit in line
                    for(size_t col = 0; col < 8; col++)
                    {
                        
                        //checks if the bit at index col in the byte is set
                        if( (memory[I + row] >> (7 - col)) & 0x1 ) {

                            if(!xoChipMode)
                            {
                                //check if pixel is out of bounds
                                if(mode12864) { //if out of bounds of 128 64 screen fixme
                                //if row of sprite goes out below screen stop drawing sprite
                                    if( (V[y] + row) >= 64) {
                                        goto spriteBreak;
                                    }
                                    //if the pixel goes out of bounds go to next row
                                    else if( (V[y] + col) >= 128) {
                                        goto rowBreak;
                                    }
                                    //if current row is out of bounds go to next row
                                    else if( (V[y] + row) < 0) {
                                        goto rowBreak;
                                    }
                                    //if the current pixel is out of bounds go to next pixel
                                    else if( (V[y] + col) < 0) {
                                        continue;
                                    }
                                    } else { //the same but 64 32 size screen
                                    if( (V[y] + row) >= 32) {
                                        goto spriteBreak;
                                    }
                                    else if( (V[x] + col) >= 64) {
                                        goto rowBreak;
                                    }
                                    else if( (V[y] + row) < 0) {
                                        goto rowBreak;
                                    }
                                    else if( (V[x] + col) < 0) {
                                        continue;
                                    }

                                    //xor
                                    //check if pixel is already set
                                    if(screen[V[x] + col][V[y] + row] == 1)
                                    {
                                        V[0xF] = 1;
                                        screen[V[x] + col][V[y] + row] = 0;
                                    }
                                    else
                                    {
                                        screen[V[x] + col][V[y] + row] = 1;
                                    }
                                }
                            }
                            else{
                                //xor
                                //check if pixel is already set
                                if(!mode12864) {
                                    if(screen[(V[x] + col) % 64][(V[y] + row) % 32] == 1)
                                    {
                                        V[0xF] = 1;
                                        screen[(V[x] + col) % 64][(V[y] + row) % 32] = 0;
                                    }
                                    else
                                    {
                                        screen[(V[x] + col) % 64][(V[y] + row) % 32] = 1;
                                    }
                                }
                                else{
                                    if(screen[(V[x] + col) % 128][(V[y] + row) % 64] == 1)
                                    {
                                        V[0xF] = 1;
                                        screen[(V[x] + col) % 128][(V[y] + row) % 64] = 0;
                                    }
                                    else
                                    {
                                        screen[(V[x] + col) % 128][(V[y] + row) % 64] = 1;
                                    }
                                }
                            }
                            
                        }
                    }
                    rowBreak:
                }
                spriteBreak:
            }

            //EX9E
            else if(numFirst == 0xE && byteLast == 0x9E)
            {
                PC += 2;

                //input handling
                inputs();
                //if(keyboard.keyboard & ( V[x]))
               if(V[x] == 0x1 && keyboard.keys.key1 || V[x] == 0x2 && keyboard.keys.key2 || V[x] == 0x3 && keyboard.keys.key3 || V[x] == 0xC && keyboard.keys.keyC || V[x] == 0x4 && keyboard.keys.key4 || V[x] == 0x5 && keyboard.keys.key5 || V[x] == 0x6 && keyboard.keys.key6 || V[x] == 0xD && keyboard.keys.keyD || V[x] == 0x7 && keyboard.keys.key7 || V[x] == 0x8 && keyboard.keys.key8 || V[x] == 0x9 && keyboard.keys.key9 || V[x] == 0xE && keyboard.keys.keyE || V[x] == 0xA && keyboard.keys.keyA || V[x] == 0x0 && keyboard.keys.key0 || V[x] == 0xB && keyboard.keys.keyB || V[x] == 0xF && keyboard.keys.keyF)
                {
                    PC += 2;
                }

            }

            //EXA1
            else if(numFirst == 0xE && byteLast == 0xA1)
            {

                PC += 2;
                //input handling
                inputs();
                //if(!(keyboard.keyboard & ( V[x])))
                if(V[x] == 0x1 && !keyboard.keys.key1 || V[x] == 0x2 && !keyboard.keys.key2 || V[x] == 0x3 && !keyboard.keys.key3 || V[x] == 0xC && !keyboard.keys.keyC || V[x] == 0x4 && !keyboard.keys.key4 || V[x] == 0x5 && !keyboard.keys.key5 || V[x] == 0x6 && !keyboard.keys.key6 || V[x] == 0xD && !keyboard.keys.keyD || V[x] == 0x7 && !keyboard.keys.key7 || V[x] == 0x8 && !keyboard.keys.key8 || V[x] == 0x9 && !keyboard.keys.key9 || V[x] == 0xE && !keyboard.keys.keyE || V[x] == 0xA && !keyboard.keys.keyA || V[x] == 0x0 && !keyboard.keys.key0 || V[x] == 0xB && !keyboard.keys.keyB || V[x] == 0xF && !keyboard.keys.keyF)
                {
                    PC += 2;
                }
            }

            //EXF2
            else if(numFirst == 0xE && byteLast == 0xF2)
            {
                PC += 2;
                //todo later
            }

            //EXF5
            else if(numFirst == 0xE && byteLast == 0xF5)
            {
                PC += 2;
                //todo later
            }

            //FX07
            else if(numFirst == 0xF && byteLast == 0x07)
            {
                PC += 2;

                V[x] = delayTimer;
            }

            //FX0A
            else if(numFirst == 0xF && byteLast == 0x0A)
            {
                /* broken
                //original behavior, fixme for superchip
                if(currentKeyPressed == 255)
                {
                    for(size_t i = 0; i < 16; i++)
                    {
                        if(keyboard.keyboard >> i & 0x1)
                        {
                            currentKeyPressed = i;
                        }
                    }
                }
                else{
                    if(!(keyboard.keyboard >> currentKeyPressed & 0x1))
                    {
                        PC += 2;
                        V[x] = currentKeyPressed;
                        currentKeyPressed = 255;
                        break;
                    }
                }
                */
                //input handling
                inputs();
                for(size_t i = 0; i < 16; i++)
                {
                    if(keyboard.keyboard >> i & 0x1)
                    {
                        PC += 2;
                        V[x] = i;
                    }
                }
                
            }

            //FX15
            else if(numFirst == 0xF && byteLast == 0x15)
            {
                PC += 2;

                delayTimer = V[x];
            }

            //FX18
            else if(numFirst == 0xF && byteLast == 0x18)
            {
                PC += 2;

                soundTimer = V[x];
            }

            //FX1E
            else if(numFirst == 0xF && byteLast == 0x1E)
            {
                PC += 2;

                V[0xF] = (I + V[x]) > 0xFFF ? 1 : 0; //qwerk checkme
                I += V[x];
            }

            //FX29
            else if(numFirst == 0xF && byteLast == 0x29)
            {
                PC += 2;
                
                I = V[x] * 5;

            }

            //FX30
            else if(numFirst == 0xF && byteLast == 0x30 && sChip8Mode)
            {
                PC += 2;

                //todo later

            }

            //FX33
            else if(numFirst == 0xF && byteLast == 0x33)
            {
                PC += 2;
                //checkme
                memory[I] = (V[x] / 100) % 10;
                memory[I + 1] = (V[x] / 10) % 10;
                memory[I + 2] = (V[x] / 1) % 10;

            }

            //FX55 checkme
            else if(numFirst == 0xF && byteLast == 0x55)
            {
                PC += 2;

                for(int ii = 0; ii <= x; ii++)
                {
                    memory[I + ii] = V[ii];
                }

                if((!sChip8Mode) || iToggle)
                {
                    I = I + x + 1;
                }

            }

            //FX65
            else if(numFirst == 0xF && byteLast == 0x65)
            {
                PC += 2;

                for(int ii = 0; ii <= x; ii++)
                {
                    V[ii] = memory[I + ii];
                }

                if((!sChip8Mode) || iToggle)
                {
                    I = I + x + 1;
                }
                

            }

            //FX75
            else if(numFirst == 0xF && byteLast == 0x75)
            {
                if(chip8EMode)
                {
                    PC += 2;

                    //todo later
                }
                else if((sChip8Mode || xoChipMode))
                {
                    PC += 2;

                    //todo later
                }
            }


            //FX85 
            else if(numFirst == 0xF && byteLast == 0x85 && (sChip8Mode || xoChipMode))
            {
                PC += 2;   
                //todo later

            }

            //FX94 
            else if(numFirst == 0xF && byteLast == 0x94 && chip8EMode)
            {
                PC += 2;
                //todo later
            }

            //FXFB 
            else if(numFirst == 0xF && byteLast == 0xFB && chip8XMode)
            {
                PC += 2;
                //todo later
            }

            //FXF8
            else if(numFirst == 0xF && byteLast == 0xF8 && chip8XMode)
            {
                PC += 2;
                //todo later
            }

            else
            {
                printf("Unhandled opcode: %X\n At address: %X\n", opcode, PC);
            }
        }

        //GPU
        int scaled = scale;
        //check if super mode is enabled
        //if 64 32 aka not super chip scale to 128 64
        //fixme handle other display modes
        if(!mode12864)
        {
            scaled = scale * 2;
        }    

        if(typeSizesStruct.blue)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        }
        else if(typeSizesStruct.black)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        }
        else if(typeSizesStruct.green)
        {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        }
        else if(typeSizesStruct.red)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        }
        else{
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        }
        
        SDL_RenderClear(renderer);

        //pixel color
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for(int row = 0; row < 64; row++)
        {
            for(int col = 0; col < 128; col++)
            {
                if(screen[col][row] == 1)
                {
                    //draw pixel
                    SDL_Rect rect = { col * scaled, row * scaled, scaled, scaled};
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
        SDL_RenderPresent(renderer);



        //decrement timers
        if(delayTimer > 0)
        delayTimer--;

        if(soundTimer > 0)
            soundTimer--;
        
        
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
        //no gui D:fixme
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
    file.size = ftell(fp) + 1;

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
