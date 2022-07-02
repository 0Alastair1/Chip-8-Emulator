
#include <main.h>


static inline bool isLittleEndian()
{
    Uint16 val = 0xFF00;
    return *(Uint8*)&val == 0xFF;
}

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

/* https://en.wikipedia.org/wiki/CHIP-8 */
/* http://devernay.free.fr/hacks/chip8/C8TECH10.HTM */
/* https://tobiasvl.github.io/blog/write-a-chip-8-emulator */
/* http://johnearnest.github.io/Octo/docs/SuperChip.html */
/* https://www.freecodecamp.org/news/creating-your-very-own-chip-8-emulator/ */
/* http://www.emulator101.com/chip-8-sprites.html */
/* https://github.com/trapexit/chip-8_documentation/blob/master/README.md */
/* https://github.com/JohnEarnest/Octo/blob/gh-pages/docs/XO-ChipSpecification.md */                                              


/* font */
const Uint8 font[180] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
    0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
    0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
    0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
    0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
    0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
    0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
    0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
    0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
    0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
    0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
    0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
    0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
    0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
    0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
    0xF0, 0x80, 0xF0, 0x80, 0x80,  /* F */

    0x3C, 0x7E, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0x7E, 0x3C, /* 0 */
    0x18, 0x38, 0x58, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, /* 1 */
    0x3E, 0x7F, 0xC3, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xFF, 0xFF, /* 2 */
    0x3C, 0x7E, 0xC3, 0x03, 0x0E, 0x0E, 0x03, 0xC3, 0x7E, 0x3C, /* 3 */
    0x06, 0x0E, 0x1E, 0x36, 0x66, 0xC6, 0xFF, 0xFF, 0x06, 0x06, /* 4 */
    0xFF, 0xFF, 0xC0, 0xC0, 0xFC, 0xFE, 0x03, 0xC3, 0x7E, 0x3C, /* 5 */
    0x3E, 0x7C, 0xC0, 0xC0, 0xFC, 0xFE, 0xC3, 0xC3, 0x7E, 0x3C, /* 6 */
    0xFF, 0xFF, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x60, 0x60, /* 7 */
    0x3C, 0x7E, 0xC3, 0xC3, 0x7E, 0x7E, 0xC3, 0xC3, 0x7E, 0x3C, /* 8 */
    0x3C, 0x7E, 0xC3, 0xC3, 0x7F, 0x3F, 0x03, 0x03, 0x3E, 0x7C, /* 9 */
};

Uint8 lastLastKeyPressed = 255;
Uint8 lastKeyPressed = 255;
Uint8 lastKeyUp = 255;
bool keyToggle = false;

TTF_Font* efont;

SDL_Window* window;
SDL_Renderer* renderer;

SDL_Window* debugWindow;
SDL_Renderer* debugRenderer;

bool showDebug = false;

/* #pragma push() */
/* #pragma pack(1) */
union
{
    Uint16 keyboard;

    struct /* PACKED */
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
/* #pragma pop() */


/* todo make io window - maybe console? */

int windowSizeX;
int windowSizeY;
void createDebugWindow()
{
    int ex = 0;
    int ey = 0;
    int w = 0;
    int h = 0;
    SDL_GetWindowPosition(window, &ex, &ey);
    SDL_GetWindowSize(window, &w, &h);

    windowSizeX = (d_width_s * d_scale);
    windowSizeY = (d_height_s * d_scale);

    debugWindow = SDL_CreateWindow("Debug window", ex + w , (ey + (h/2)) - windowSizeY/2 , windowSizeX, windowSizeY, SDL_WINDOW_SHOWN);
    debugRenderer = SDL_CreateRenderer(debugWindow, -1, SDL_RENDERER_ACCELERATED);
}

void initSDL(SDL_Window** window, SDL_Renderer** renderer)
{
    SDL_Init(SDL_INIT_VIDEO);
    *window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_s * scale, height_s * scale, SDL_WINDOW_SHOWN);
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

    SDL_ShowCursor(SDL_ENABLE);
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_SetWindowGrab(*window, SDL_FALSE);

    return;
}

Uint8* data;
uint32_t size;

void startingUi(SDL_Window* window,SDL_Renderer* renderer,bool* sChip8Mode,bool* xoChipMode,bool* chip8HdMode,bool* chip10Mode,bool* chip8IMode,bool* chip8EMode,bool* chip8XMode)
{

    int width = width_s * scale;
    int height = height_s * scale;

    SDL_Surface* textSurface;
    SDL_Texture* textTexture;
    
    SDL_Surface* textSurface2;
    SDL_Texture* textTexture2;

    SDL_Surface* textSurface3;
    SDL_Texture* textTexture3;

    SDL_Surface* textSurface4;
    SDL_Texture* textTexture4;

    SDL_Surface* textSurface5;
    SDL_Texture* textTexture5;

    SDL_Surface* textSurface6;
    SDL_Texture* textTexture6;

    SDL_Surface* textSurface7;
    SDL_Texture* textTexture7;

    SDL_Surface* textSurface8;
    SDL_Texture* textTexture8;

    SDL_Texture* textTexture9;
    SDL_Surface* textSurface9;

    SDL_Texture* textTexture10;
    SDL_Surface* textSurface10;

    SDL_Texture* warnTexture1;
    SDL_Surface* warnSurface1;

    SDL_Texture* warnTexture2;
    SDL_Surface* warnSurface2;

    SDL_Texture* showDebugTexture;
    SDL_Surface* showDebugSurface;

    SDL_Texture* showDebugTexture1;
    SDL_Surface* showDebugSurface1;

    SDL_Color textColor = {255, 255, 255};

    /* create ui tick buttons and text */
    int spacingy = 35;
    int spacingx = 20;
    int buttonsx = width /2 + width/8;

    SDL_Rect noModebutton = {buttonsx, spacingy, 10, 10};

    SDL_Rect sChip8ModeButton = {buttonsx, spacingy*2, 10, 10};

    SDL_Rect xoChipModeButton = {buttonsx, spacingy*3, 10, 10};

    SDL_Rect chip8HdModeButton = {buttonsx, spacingy*4, 10, 10};

    SDL_Rect chip10ModeButton = {buttonsx, spacingy*5, 10, 10};

    SDL_Rect chip8IModeButton = {buttonsx, spacingy*6 , 10, 10};

    SDL_Rect chip8EModeButton = {buttonsx, spacingy*7 , 10, 10};

    SDL_Rect chip8XModeButton = {buttonsx, spacingy*8 , 10, 10};


    textSurface = TTF_RenderText_Solid(efont, "CHIP8 MODE", textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {buttonsx + spacingx, spacingy, textSurface->w, textSurface->h};

    textSurface2 = TTF_RenderText_Solid(efont, "SCHIP MODE", textColor);
    textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
    SDL_Rect textRect2 = {buttonsx + spacingx, spacingy*2, textSurface2->w, textSurface2->h};

    textSurface3 = TTF_RenderText_Solid(efont, "XOCHIP MODE", textColor);
    textTexture3 = SDL_CreateTextureFromSurface(renderer, textSurface3);
    SDL_Rect textRect3 = {buttonsx + spacingx, spacingy*3, textSurface3->w, textSurface3->h};

    textSurface4 = TTF_RenderText_Solid(efont, "CHIP8HD MODE", textColor);
    textTexture4 = SDL_CreateTextureFromSurface(renderer, textSurface4);
    SDL_Rect textRect4 = {buttonsx + spacingx, spacingy*4, textSurface4->w, textSurface4->h};

    textSurface5 = TTF_RenderText_Solid(efont, "CHIP10 MODE", textColor);
    textTexture5 = SDL_CreateTextureFromSurface(renderer, textSurface5);
    SDL_Rect textRect5 = {buttonsx + spacingx, spacingy*5, textSurface5->w, textSurface5->h};

    textSurface6 = TTF_RenderText_Solid(efont, "CHIP8I MODE", textColor);
    textTexture6 = SDL_CreateTextureFromSurface(renderer, textSurface6);
    SDL_Rect textRect6 = {buttonsx + spacingx, spacingy*6, textSurface6->w, textSurface6->h};

    textSurface7 = TTF_RenderText_Solid(efont, "CHIP8E MODE", textColor);
    textTexture7 = SDL_CreateTextureFromSurface(renderer, textSurface7);
    SDL_Rect textRect7 = {buttonsx + spacingx, spacingy*7, textSurface7->w, textSurface7->h};

    textSurface8 = TTF_RenderText_Solid(efont, "CHIP8X MODE", textColor);
    textTexture8 = SDL_CreateTextureFromSurface(renderer, textSurface8);
    SDL_Rect textRect8 = {buttonsx + spacingx, spacingy*8, textSurface8->w, textSurface8->h};

    SDL_Rect* buttons[9] = {&noModebutton, &sChip8ModeButton, &xoChipModeButton, &chip8HdModeButton, &chip10ModeButton, &chip8IModeButton, &chip8EModeButton, &chip8XModeButton};

    SDL_Rect* textRects[9] = {&textRect, &textRect2, &textRect3, &textRect4, &textRect5, &textRect6, &textRect7, &textRect8};

    SDL_Texture* textTextures[9] = {textTexture, textTexture2, textTexture3, textTexture4, textTexture5, textTexture6, textTexture7, textTexture8};

    bool selectedMode[9] = {true, false, false, false, false, false, false, false, false};

    #undef x
    #undef y

    warnSurface1 = TTF_RenderText_Solid(efont, "DEBUG WINDOW", textColor);
    warnTexture1 = SDL_CreateTextureFromSurface(debugRenderer, warnSurface1);
    SDL_Rect warnRect1 = {(windowSizeX/2)-(warnSurface1->w/2),(windowSizeY/2)-(warnSurface1->h/2), warnSurface1->w, warnSurface1->h};

    warnSurface2 = TTF_RenderText_Solid(efont, "DECREASES FPS", textColor);
    warnTexture2 = SDL_CreateTextureFromSurface(debugRenderer, warnSurface2);
    SDL_Rect warnRect2 = {(windowSizeX/2)-(warnSurface2->w/2),(windowSizeY/2)-(warnSurface2->h/2) + warnSurface2->h, warnSurface2->w, warnSurface2->h};

    showDebugSurface = TTF_RenderText_Solid(efont, "DEBUG", textColor);
    showDebugTexture = SDL_CreateTextureFromSurface(renderer, showDebugSurface);
    SDL_Rect showDebugRect = {0,0, showDebugSurface->w, showDebugSurface->h};

    showDebugSurface1 = TTF_RenderText_Solid(efont, "DEBUG", (SDL_Color){0, 255, 0});
    showDebugTexture1 = SDL_CreateTextureFromSurface(renderer, showDebugSurface1);
    SDL_Rect showDebugRect1 = {0,0, showDebugSurface1->w, showDebugSurface1->h};

    

    textSurface9 = TTF_RenderText_Solid(efont, "OPEN FILE", textColor);
    textTexture9 = SDL_CreateTextureFromSurface(renderer, textSurface9);
    SDL_Rect openFileButton = {width /2 - width/3, height/4,textSurface9->w, textSurface9->h};

    SDL_Rect playButton = {width /2 - width/4, height - height/4, 50, 50};

    textSurface10 = TTF_RenderText_Solid(efont, "PLAY", textColor);
    textTexture10 = SDL_CreateTextureFromSurface(renderer, textSurface10);

    bool ready = false;

    /* todo recently used files */
    /* todo save config */

    SDL_Event test_event;
    while(true)
    {
        /* draw ui */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        /* draw all buttons in the list */
        {
            Uint8 i;
            for(i = 0; i < 8; i++)
            {
                if(isTrue(selectedMode[i]))
                {
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                }
                SDL_RenderFillRect(renderer, buttons[i]);
            }
            /* draw all text in the list */
            for(i = 0; i < 8; i++)
            {
                SDL_RenderCopy(renderer, textTextures[i], NULL, textRects[i]);
            }
        }

        
        SDL_SetRenderDrawColor(renderer, 0, 20, 0, 255);
        SDL_RenderFillRect(renderer, &openFileButton);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderCopy(renderer, textTexture9, NULL, &openFileButton);

        if(isTrue(ready))
        {
            SDL_RenderCopy(renderer, textTexture10, NULL, &playButton);
        }

        SDL_Point mouse;

        /* change the color of openfile and play button when mouse is clicked on them */
        if(SDL_GetMouseState(&mouse.x, &mouse.y) & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if(mouse.x >= openFileButton.x && mouse.x <= openFileButton.x + openFileButton.w && mouse.y >= openFileButton.y && mouse.y <= openFileButton.y + openFileButton.h)
            {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderFillRect(renderer, &openFileButton);
            }
            if(mouse.x >= playButton.x && mouse.x <= playButton.x + playButton.w && mouse.y >= playButton.y && mouse.y <= playButton.y + playButton.h)
            {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderFillRect(renderer, &playButton);
            }
        }

        /* if no buttons are clicked set chip8 mode to clicked */
        bool noneSelected = true;
        {
            size_t i;
            for (i = 0; i < 8; i++)
            {
                if(isTrue(selectedMode[i]))
                {
                    noneSelected = false;
                }
            }
        }
        if(isTrue(noneSelected))
        {
            selectedMode[0] = true;
        }

        /* draw debug button */
        SDL_SetRenderDrawColor(renderer, 0, 20, 0, 20);
        
        SDL_RenderFillRect(renderer, &showDebugRect);

        if(showDebug)
        {
            SDL_RenderCopy(renderer, showDebugTexture1, NULL, &showDebugRect1);
        }
        else
        {
            SDL_RenderCopy(renderer, showDebugTexture, NULL, &showDebugRect);
        }
        

        SDL_RenderPresent(renderer);

    
        /* update debug window in starting ui */
        if(isTrue(showDebug))
        {
            SDL_SetRenderDrawColor(debugRenderer, 0, 15, 0, 15);
            SDL_RenderClear(debugRenderer);

            SDL_RenderCopy(debugRenderer, warnTexture1, NULL, &warnRect1);
            SDL_RenderCopy(debugRenderer, warnTexture2, NULL, &warnRect2);
        
            SDL_RenderPresent(debugRenderer);
        }

        /* input */
        /* get mouse clicks */
        while (SDL_PollEvent(&test_event)) {
            
            switch (test_event.type) {

                case SDL_QUIT:
                    exit(0);
                    break;

                case SDL_WINDOWEVENT:
                    switch (test_event.window.event) {
                        case SDL_WINDOWEVENT_CLOSE:
                            if(test_event.window.windowID == SDL_GetWindowID(window))
                            {
                                exit(0);
                            }
                            else if(test_event.window.windowID == SDL_GetWindowID(debugWindow))
                            {
                                showDebug = false;
                            }
                            break;

                        default:
                            break;
                    }
                    break;

                case SDL_KEYDOWN:
                    if(test_event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        exit(0);
                    }
                    break;

                case SDL_MOUSEBUTTONUP:
                    int mouseX; 
                    int mouseY;
                    /* handle inputs */
                        SDL_GetMouseState(&mouseX, &mouseY);
                        if(mouseX >= 0 && mouseX <= width_s * scale && mouseY >= 0 && mouseY <= height_s * scale)
                        {
                            /* check if any button was clicked */

                            if(mouseX >= showDebugRect.x && mouseX <= showDebugRect.x + showDebugRect.w && mouseY >= showDebugRect.y && mouseY <= showDebugRect.y + showDebugRect.h)
                            {
                                showDebug = !showDebug;
                            }

                            Uint8 i;
                            for(i = 0; i < 8; i++)
                            {
                                if(mouseX >= buttons[i]->x - 10 && mouseX <= buttons[i]->x + buttons[i]->w + 200 && mouseY >= buttons[i]->y - 10 && mouseY <= buttons[i]->y + buttons[i]->h + 10)
                                {
                                    selectedMode[i] = !selectedMode[i];
                                    Uint8 j;
                                    for(j = 0; j < 8; j++)
                                    {
                                        if(j != i)
                                        {
                                            selectedMode[j] = false;
                                        }
                                    }
                                }
                            }
                            

                            if(mouseX >= openFileButton.x - 10 && mouseX <= openFileButton.x + openFileButton.w && mouseY >= openFileButton.y - 10 && mouseY <= openFileButton.y + openFileButton.h + 10)
                            {
                                char* filepath;

                                filepath = openFile();
                                if(filepath == 0x0)
                                {
                                    break;
                                }
                                struct file file = readFile(filepath);
                                data = file.data;
                                size = file.size;
                                ready = true;       
                            }

                            if(mouseX >= playButton.x - 10 && mouseX <= playButton.x + playButton.w && mouseY >= playButton.y - 10 && mouseY <= playButton.y + playButton.h + 10 && ready)
                            {
                                if(isTrue(selectedMode[0]))
                                {
                                    SDL_SetWindowTitle(window, "Chip-8 Emulator: Original Chip8 Mode");
                                }
                                if(isTrue(selectedMode[1]))
                                {
                                    /* sChip mode */
                                    *sChip8Mode = true;
                                    SDL_SetWindowTitle(window, "Chip-8 Emulator: SuperChip Mode");

                                }
                                if(isTrue(selectedMode[2]))
                                {
                                    /* xoChip mode */
                                    *xoChipMode = true;
                                    SDL_SetWindowTitle(window, "Chip-8 Emulator: XoChip Mode");

                                }
                                if(isTrue(selectedMode[3]))
                                {
                                    /* chip8HD mode */
                                    *chip8HdMode = true;
                                    SDL_SetWindowTitle(window, "Chip-8 Emulator: Chip8HD Mode");
                                    
                                }
                                if(isTrue(selectedMode[4]))
                                {
                                    /* chip10 mode */
                                    *chip10Mode = true;
                                    SDL_SetWindowTitle(window, "Chip-8 Emulator: Chip10 Mode");
                                    
                                }
                                if(isTrue(selectedMode[5]))
                                {
                                    /* chip8I mode */
                                    *chip8IMode = true;
                                    SDL_SetWindowTitle(window, "Chip-8 Emulator: Chip8I Mode");
                                    
                                }
                                if(isTrue(selectedMode[6]))
                                {
                                    /* chip8E mode */
                                    *chip8EMode = true;
                                    SDL_SetWindowTitle(window, "Chip-8 Emulator: Chip8E Mode");
                                    
                                }
                                if(isTrue(selectedMode[7]))
                                {
                                    /* chip8X mode */
                                    *chip8XMode = true;
                                    SDL_SetWindowTitle(window, "Chip-8 Emulator: Chip8X Mode");
                                    
                                }

                                SDL_RenderClear(renderer);
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

                                SDL_FreeSurface(textSurface9);
                                SDL_DestroyTexture(textTexture9);

                                SDL_FreeSurface(textSurface10);
                                SDL_DestroyTexture(textTexture10);

                                SDL_FreeSurface(warnSurface1);
                                SDL_DestroyTexture(warnTexture1);
                                
                                SDL_FreeSurface(warnSurface2);
                                SDL_DestroyTexture(warnTexture2);
                                
                                SDL_FreeSurface(showDebugSurface);
                                SDL_DestroyTexture(showDebugTexture);
                                
                                SDL_FreeSurface(showDebugSurface1);
                                SDL_DestroyTexture(showDebugTexture1);

                                if(!isTrue(showDebug))
                                {
                                    SDL_DestroyWindow(debugWindow);
                                }
                                
                                return;
                                
                            }
                        }
                        break;

                default:
                    break;

            }

        }
        if(isTrue(showDebug))
        {
            SDL_ShowWindow(debugWindow);
        }
        else
        {
            SDL_HideWindow(debugWindow);
        }
    }
}

void inputs()
{

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            exit(0);
        }

        if(event.window.event == SDL_WINDOWEVENT_CLOSE)
        {

            if(event.window.windowID == SDL_GetWindowID(window))
            {
                exit(0);
            }
            else
            {
                showDebug = false;
                /* SDL_HideWindow(debugWindow); */
                SDL_DestroyWindow(debugWindow);
            }         
        }

        if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_1:
                    keyboard.keys.key1 = 1;
                    lastKeyPressed = 0x1;
                    break;

                case SDLK_2:
                    keyboard.keys.key2 = 1;
                    lastKeyPressed = 0x2;
                    break;
                
                case SDLK_3:
                    keyboard.keys.key3 = 1;
                    lastKeyPressed = 0x3;
                    break;
                    
                case SDLK_4:
                    keyboard.keys.keyC = 1;
                    lastKeyPressed = 0xC;
                    break;

                case SDLK_q:
                    keyboard.keys.key4 = 1;
                    lastKeyPressed = 0x4;
                    break;

                case SDLK_w:
                    keyboard.keys.key5 = 1;
                    lastKeyPressed = 0x5;
                    break;

                case SDLK_e:
                    keyboard.keys.key6 = 1;
                    lastKeyPressed = 0x6;
                    break;

                case SDLK_r:
                    keyboard.keys.keyD = 1;
                    lastKeyPressed = 0xD;
                    break;

                case SDLK_a:
                    keyboard.keys.key7 = 1;
                    lastKeyPressed = 0x7;
                    break;

                case SDLK_s:
                    keyboard.keys.key8 = 1;
                    lastKeyPressed = 0x8;
                    break;

                case SDLK_d:
                    keyboard.keys.key9 = 1;
                    lastKeyPressed = 0x9;
                    break;
                
                case SDLK_f:
                    keyboard.keys.keyE = 1;
                    lastKeyPressed = 0xE;
                    break;

                case SDLK_z:
                    keyboard.keys.keyA = 1;
                    lastKeyPressed = 0xA;
                    break;

                case SDLK_x:
                    keyboard.keys.key0 = 1;
                    lastKeyPressed = 0;
                    break;

                case SDLK_c:
                    keyboard.keys.keyB = 1;
                    lastKeyPressed = 0xB;
                    break;

                case SDLK_v:
                    keyboard.keys.keyF = 1;
                    lastKeyPressed = 0xF;
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
                    lastKeyUp = 0x1;
                    break;

                case SDLK_2:
                    keyboard.keys.key2 = 0;
                    lastKeyUp = 0x2;
                    break;
                
                case SDLK_3:
                    keyboard.keys.key3 = 0;
                    lastKeyUp = 0x3;
                    break;
                    
                case SDLK_4:
                    keyboard.keys.keyC = 0;
                    lastKeyUp = 0xC;
                    break;

                case SDLK_q:
                    keyboard.keys.key4 = 0;
                    lastKeyUp = 0x4;
                    break;

                case SDLK_w:
                    keyboard.keys.key5 = 0;
                    lastKeyUp = 0x5;
                    break;

                case SDLK_e:
                    keyboard.keys.key6 = 0;
                    lastKeyUp = 0x6;
                    break;

                case SDLK_r:
                    keyboard.keys.keyD = 0;
                    lastKeyUp = 0xD;
                    break;

                case SDLK_a:
                    keyboard.keys.key7 = 0;
                    lastKeyUp = 0x7;
                    break;

                case SDLK_s:
                    keyboard.keys.key8 = 0;
                    lastKeyUp = 0x8;
                    break;

                case SDLK_d:
                    keyboard.keys.key9 = 0;
                    lastKeyUp = 0x9;
                    break;
                
                case SDLK_f:
                    keyboard.keys.keyE = 0;
                    lastKeyUp = 0xE;
                    break;

                case SDLK_z:
                    keyboard.keys.keyA = 0;
                    lastKeyUp = 0xA;
                    break;

                case SDLK_x:
                    keyboard.keys.key0 = 0;
                    lastKeyUp = 0;
                    break;

                case SDLK_c:
                    keyboard.keys.keyB = 0;
                    lastKeyUp = 0xB;
                    break;

                case SDLK_v:
                    keyboard.keys.keyF = 0;
                    lastKeyUp = 0xF;
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

/* fixme debugger really slow */
SDL_Rect PCRect;
SDL_Rect SPRect;
SDL_Rect iRegRect;
bool firstRun = true;
SDL_Rect VSCountRect[16];

SDL_Texture* PCTexture;
SDL_Texture* SPTexture;
SDL_Texture* iRegTexture;
SDL_Texture* VSTexture[16];

static void updateDebugWindow(Uint16* PC,Uint8* SP,Uint16* iReg,Uint8* memory,Uint16* V,Uint16* stack,Uint8* delayTimer,Uint16* soundTimer)
{


    SDL_SetRenderDrawColor(debugRenderer, 0, 15, 0, 15);
    SDL_RenderClear(debugRenderer);

    static const SDL_Color textColor = {0xFF, 0xFF, 0xFF, 0xFF};

    Uint8 spacing = 100;
    Uint8 lineSpacing = 30;

    SDL_Surface* surface;
    SDL_Texture* texture;

    if(firstRun)
    {
        firstRun = false;

        
        surface = TTF_RenderText_Solid(efont, "PC   ", textColor);
        PCTexture = SDL_CreateTextureFromSurface(debugRenderer, surface);
        SDL_Rect PCRect1 = {0, 40, surface->w, surface->h};
        PCRect = PCRect1;

        SDL_FreeSurface(surface);


        surface = TTF_RenderText_Solid(efont, "SP   ", textColor);
        SPTexture = SDL_CreateTextureFromSurface(debugRenderer, surface);
        SDL_Rect SPRect1 = {0, PCRect.y + lineSpacing, surface->w, surface->h};
        SPRect = SPRect1;

        SDL_FreeSurface(surface);


        surface = TTF_RenderText_Solid(efont, "I    ", textColor);
        iRegTexture = SDL_CreateTextureFromSurface(debugRenderer, surface);
        SDL_Rect iRegRect1 = {0, SPRect.y + lineSpacing, SPRect.w + lineSpacing, SPRect.h};
        iRegRect = iRegRect1;


        SDL_FreeSurface(surface);


        Uint8 i;
        for(i = 0; i < 16; i++)
        {
            char VCount =0;
            sprintf(&VCount, "V%d", i);

            surface = TTF_RenderText_Solid(efont, &VCount, textColor);
            VSTexture[i] = SDL_CreateTextureFromSurface(debugRenderer, surface);

            VSCountRect[i].x = 0;
            VSCountRect[i].y = iRegRect.y + lineSpacing * (i + 1);
            VSCountRect[i].w = surface->w;
            VSCountRect[i].h = surface->h;


            SDL_FreeSurface(surface);
        }

    }

    SDL_RenderCopy(debugRenderer, PCTexture, NULL, &PCRect);
    SDL_RenderCopy(debugRenderer, SPTexture, NULL, &SPRect);
    SDL_RenderCopy(debugRenderer, iRegTexture, NULL, &iRegRect);

    {
        Uint8 i;
        for(i = 0; i < 16; i++)
        {
            SDL_RenderCopy(debugRenderer, VSTexture[i], NULL, &VSCountRect[i]);
        }
    }


    /* fixme below expensive */
    char PCValS[5];
    sprintf(PCValS, "%04X", *PC);

    surface = TTF_RenderText_Solid(efont, &PCValS[0], textColor);
    texture = SDL_CreateTextureFromSurface(debugRenderer, surface);
    SDL_Rect PCVal = {spacing, 40, surface->w, surface->h};

    SDL_RenderCopy(debugRenderer, texture, NULL, &PCVal);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    
    char SPValS[5];
    sprintf(SPValS, "%04X", *SP);
    surface = TTF_RenderText_Solid(efont, SPValS, textColor);
    texture = SDL_CreateTextureFromSurface(debugRenderer, surface);
    SDL_Rect SPVal = {spacing, SPRect.y, surface->w, surface->h};

    SDL_RenderCopy(debugRenderer, texture, NULL, &SPVal);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);


    
    char iRegValS[5];
    sprintf(iRegValS, "%04X", *iReg);
    surface = TTF_RenderText_Solid(efont, iRegValS, textColor);
    texture = SDL_CreateTextureFromSurface(debugRenderer, surface);
    SDL_Rect iRegVal = {spacing, iRegRect.y, surface->w, surface->h};

    SDL_RenderCopy(debugRenderer, texture, NULL, &iRegVal);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);


    char VValS[5];
    int i;
    for (i = 0; i < 16; i++)
    {        

        sprintf(VValS, "%02X", V[i]);

        surface = TTF_RenderText_Solid(efont, VValS, textColor);
        texture = SDL_CreateTextureFromSurface(debugRenderer, surface);
        SDL_Rect VVal = {spacing, VSCountRect[i].y , surface->w, surface->h};

        SDL_RenderCopy(debugRenderer, texture, NULL, &VVal);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

    }

    /* stack */

    /* delayTimer */


    /* soundTimer */

    /* todo make pause button, maybe rewind button, and forward button, and simulate w key press button */

    /* Draw */
    SDL_RenderPresent(debugRenderer);

}
Uint16 delayTimer =0;
Uint16 soundTimer =0;

PMutex* mutex;
bool audioPlaying = false;
void checkAudio()
{
    initAudio();
    startAudio();
    muteAudio();
    

    while(true)
    {
        if(delayTimer > 0)
        {
            p_mutex_lock(mutex);
            delayTimer--;
            p_mutex_unlock(mutex);
        }

        if(soundTimer > 0)
        {
            p_mutex_lock(mutex);
            soundTimer--;
            p_mutex_unlock(mutex);
        }

        if(isTrue(soundTimer > 0) && !isTrue(audioPlaying))
        {
            unmuteAudio();
            audioPlaying = true;
        }
        else if(isTrue(audioPlaying) && !isTrue(soundTimer > 0))
        {
            muteAudio();
            audioPlaying = false;
        }
        SDL_Delay(10); /* fix timing */
    }
}

void cpuLoop()
{
    Uint16 PC =0;
    Uint8 SP =0;
    Uint8* memory;
    Uint16 V[16];
    Uint16 stack[32];
    Uint16 flags[16];

    Uint8* patternBuffer = malloc(16);
    
    struct strangeTypeSizes typeSizesStruct = {0, 0, 1, 0, 0}; /* starts in black mode? */
    bool ETI660Mode = false;

    

    bool sChip8Mode = false;

    bool xoChipMode = false;

    bool chip8HdMode = false;

    bool chip10Mode = false;

    bool chip8IMode = false;

    bool chip8EMode = false;

    bool chip8XMode = false;

    bool mode3232 = false;
    bool mode6464 = false;
    bool mode12864 = false;
    bool iToggle = false;
    
    Uint16 opcode;

    
    /* initialize SDL and store the result */
    initSDL(&window, &renderer);

    SDL_ShowWindow(window);

    writeFont();
    TTF_Init();
    efont = TTF_OpenFont("chip8emulatorfont.ttf", 24);

    mutex = p_mutex_new();
    PUThread* audioThread = p_uthread_create((void*)checkAudio, NULL, true, NULL);
    
    /* create debugger window */
    createDebugWindow();
    if(isTrue(showDebug))
    {
        SDL_ShowWindow(debugWindow);
    }
    else
    {
        SDL_HideWindow(debugWindow);
    }

    Uint16 iReg = I;

    /* draw starting ui screen */
    startingUi(window, renderer, &sChip8Mode, &xoChipMode, &chip8HdMode, &chip10Mode, &chip8IMode, &chip8EMode, &chip8XMode);

    #define x (opcode & 0x0F00) >> 8
    #define y (opcode & 0x00F0) >> 4

    /* configure chip8 versions according to flags */
    if(isTrue(sChip8Mode))
    {
        mode12864 = false;
    }
    if(isTrue(xoChipMode))
    {
        mode12864 = false;
    }

    if(isTrue(chip10Mode))
    {
        mode12864 = true;
    }

    if(isTrue(chip8HdMode))
    {
        mode3232 = true;
    }

    /* set all pixels to 0 */
    Uint8 screen[width_s][height_s];

    {
        size_t i;
        size_t ii;
        for (i = 0; i < width_s; i++)
        {   
            for (ii = 0; ii < height_s; ii++)
            {
                screen[i][ii] = 0;
            }
        }
    }

    /* copy file data to memory */
    if(size + 1024 < 8192)
    {
        memory = alloca(size + 1024);
    }
    else
    {
        memory = malloc(size + size/2);
    }
    
    if(!isTrue(ETI660Mode))
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

    /* copy font to memory */
    memcpy(memory, font, 80);
    {
        Uint8 ii;
        for(ii = 0; ii < 16; ii++)
        {
            V[ii] = 0;
            stack[ii] = 0;
        }
    }

    delayTimer = 0;
    soundTimer = 0;


    Uint8 Width = 128;
    Uint8 Height = 64;

    /* cpu loop */
    /* todo - check conflics caused by overlapping instructions */
    while (true)
    {

        /* sleep fixme */
        SDL_Delay(1);

        /* fetch opcode */
        if(isTrue(isLittleEndian))
        {
            opcode = swap_16( (*(Uint16*)&memory[PC]) & 0xFFFF);
        }
        else
        {
            opcode = (*(Uint16*)&memory[PC] & 0xFFFF);
        }

        /* printf("%04x %04x\n", opcode, PC-0x200); */
        if(showDebug)
        {
            updateDebugWindow(&PC, &SP, &iReg, memory, &V, &stack, &delayTimer, &soundTimer);
        }

        /* decode and execute opcode */

        {
            /* 001N - aditional */
            if(byteFirst == 0x00 && numThird == 0x1)
            {
                exit(n);
            }

            /* 0x00CN */
            /* move each pixel in screen down by n pixels */
            else if(byteFirst == 0x00 && numThird == 0xC && isTrue(sChip8Mode))
            {
                PC += 2;

                size_t i;
                /* check me */
                for (i = height_s; i > 0; i--)
                {   
                    size_t iii;
                    size_t ii;
                    if(i - n < 0) /* check me */
                    {
                        for (iii = 0; iii < width_s; iii++)
                        {
                            /* screen[ii][i] = screen[ii][height_s - (i - n)]; check fix, does this wrap here? */
                            screen[iii][i] = 0;
                        }
                         
                    }
                    else{
                        for (ii = 0; ii < width_s; ii++)
                        {
                            screen[ii][i] = screen[ii][i-n];
                        }
                    }   
                }
            }

            /* 0x00DN */
            else if(byteFirst == 0x00 && numThird == 0xD && isTrue(xoChipMode))
            {
                PC += 2;

                /* todo later */

            }

            /* 0x00E0 */
            else if(opcode == 0x00E0)
            {
                PC += 2;
                
                size_t i;
                size_t ii;
                for (i = 0; i < width_s; i++)
                {   
                    for (ii = 0; ii < height_s; ii++)
                    {
                        screen[i][ii] = 0;
                    }
                }
            }

            /* 0x00EE */
            else if(opcode == 0x00EE)
            {
                PC = stack[SP];
                SP--;
            
            }

            else if(opcode == 0x00FA)
            {
                PC += 2;

                iToggle = !isTrue(iToggle);
                break;
            }
            /* 0x00FB - chip8 super instrcution */
            else if(opcode == 0x00FB && isTrue(sChip8Mode))
            {
                PC += 2;

                /* scroll display 4 pixels to the right  */
                size_t i;
                for(i = 0; i < width_s; i++)
                {
                    if(i - 4 <= 0)
                    {
                        size_t ii;
                        for(ii = 0; ii < height_s; ii++)
                        {
                            screen[i][ii] = 0;
                        }
                    }
                    else
                    {
                        size_t ii;
                        for(ii = 0; ii < height_s; ii++)
                        {
                            screen[i][ii] = screen[i-4][ii];
                        }
                    }
                }
            }
            
            /* 0x00FC - chip8 super instruction  */
            else if(opcode == 0x00FC && isTrue(sChip8Mode))
            {
                PC += 2;

                /* scroll display 4 pixels to the left  */
                size_t i;
                for(i = 0; i < width_s; i++)
                {
                    if(i + 4 >= width_s)
                    {
                        size_t ii;
                        for(ii = 0; ii < height_s; ii++)
                        {
                            screen[i][ii] = 0;
                        }
                    }
                    else
                    {
                        size_t ii;
                        for(ii = 0; ii < height_s; ii++)
                        {
                            screen[i][ii] = screen[i+4][ii];
                        }
                    }
                    
                }

            }

            /* 0x00FD - chip8 super instruction  */
            else if(opcode == 0x00FD && isTrue(sChip8Mode))
            {
                exit(0);
            }

            /* 0x00FE - chip8 super instruction  */
            else if(opcode == 0x00FE && isTrue(sChip8Mode))
            {
                PC += 2;

                
                mode12864 = false;
                /* todo later  */

            }

            /* 0x00FF - chip8 super instruction  */
            else if(opcode == 0x00FF && isTrue(sChip8Mode))
            {
                /* checkme  */
                PC += 2;

                mode12864 = true;
            }
            
            else if(opcode == 0x0000)
            {
                /* checkme  */
                PC += 2;
            }
            

           /* 0x02A0 - extended  */
            else if(opcode == 0x02A0 && isTrue(chip8XMode))
            {
                PC += 2;
                
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

            /* 0NNN - checkme  */
            else if(numFirst == 0x0)
            {
                PC += 2;
                /*  */

                /* todo later  */

            }

            /* 1NNN  */
            else if(numFirst == 0x1)
            {
                PC = nnn;
            }

            /* 2NNN  */
            else if(numFirst == 0x2)
            {
                PC += 2;

                SP += 1;
                stack[SP] = PC;

                PC = nnn;
            }

            /* 3XKK  */
            else if(numFirst == 0x3)
            {
                PC += 2;

                if(V[x] == kk)
                {
                    PC += 2;
                }
            }

            /* 4XKK  */
            else if(numFirst == 0x4)
            {
                PC += 2;

                if(V[x] != kk)
                {
                    PC += 2;
                }
            }

            /* 5XY0  */
            else if(numFirst == 0x5 && numLast == 0x0)
            {
                PC += 2;

                if(V[x] == V[y])
                {
                    PC += 2;
                }

            }

            /* 5XY1  */
            else if(numFirst == 0x5 && numLast == 0x1 && isTrue(chip8EMode))
            {
                PC += 2;

                if(V[x] > V[y])
                {
                    PC += 2;
                }
            }

            /* 5XY1  */
            else if(numFirst == 0x5 && numLast == 0x1 && isTrue(chip8XMode))
            {
                /* checkme /fixme  */

                if(isTrue(chip8XMode))
                {
                    PC += 2;
        
                    V[x] = (V[x] + V[y]) & 0xFF;
                }

            }

            /* 5XY2  */
            else if(numFirst == 0x5 && numLast == 0x2 && (isTrue(chip8EMode)))
            {
                PC += 2;

                if(V[x] < V[y])
                {
                    PC += 2;
                }
            }

            /* 5XY2  */
            else if(numFirst == 0x5 && numLast == 0x2 && (isTrue(xoChipMode)))
            {
                PC += 2;

                memcpy( memory + (sizeof(Uint8) * I), &V[x], (y - x) + 1);
                
            }
                
            /* 5XY3  */
            else if(numFirst == 0x5 && numLast == 0x2 && (isTrue(xoChipMode)))
            {
                PC += 2;

                memcpy( &V[x] ,memory + (sizeof(Uint8) * I), (y - x) + 1);
                
            }
            
            /* 5XY3  */
            else if(numFirst == 0x5 && numLast == 0x3 && (isTrue(chip8EMode)))
            {
                PC += 2;

                if(V[x] != V[y])
                {
                    PC += 2;
                }
            }

            /* 6XNN  */
            else if(numFirst == 0x6)
            {
                PC += 2;

                V[x] = kk & 0xFF;

            }

            /* 7XNN  */
            else if(numFirst == 0x7)
            {
                PC += 2;

                V[x] = (V[x] + kk) & 0xFF;
            }

            /* 8XY0  */
            else if(numFirst == 0x8 && numLast == 0x0)
            {
                PC += 2;

                V[x] = V[y];

            }

            /* 8XY1  */
            else if(numFirst == 0x8 && numLast == 0x1)
            {
                PC += 2;

                if(!isTrue(sChip8Mode) && !isTrue(xoChipMode)) /* xoChipMode supposed to be here? && vf order??  */
                {
                    V[0xF] = 0;
                }

                V[x] = V[x] | V[y];
            }

            /* 8XY2  */
            else if(numFirst == 0x8 && numLast == 0x2)
            {
                PC += 2;

                if(!isTrue(sChip8Mode) && !isTrue(xoChipMode)) /* vf order??  */
                {
                    V[0xF] = 0;
                }

                V[x] = V[x] & V[y];
            }

            /* 8XY3  */
            else if(numFirst == 0x8 && numLast == 0x3)
            {
                PC += 2;

                if(!isTrue(sChip8Mode) && !isTrue(xoChipMode)) /* vf order?? */
                {
                    V[0xF] = 0;
                }

                V[x] = V[x] ^ V[y];
            }

            /* 8XY4  */
            else if(numFirst == 0x8 && numLast == 0x4)
            {
                PC += 2;

                Uint16 ex = V[x];
                Uint16 ey = V[y];

                V[x] = (V[x] + V[y]) & 0xFF; /* checkme */
                /* vf order checkme */
                V[0xF] = ((ex + ey) > 0xFF) ? 1 : 0;
                
            }


            /* 8XY5  */
            else if(numFirst == 0x8 && numLast == 0x5)
            { 
                PC += 2;

                Uint16 ex = V[x];
                Uint16 ey = V[y];

                /* vf order checkme  */
                V[x] = (V[x] - V[y]) & 0xFF;
                V[0xF] = (ex >= ey) ? 1 : 0;
                
                
            }

            /* 8XY6  */
            else if(numFirst == 0x8 && numLast == 0x6)
            {
                PC += 2;

                if(!isTrue(sChip8Mode) && !isTrue(xoChipMode)) /* check  */
                {
                    V[x] = V[y];
                }

                Uint16 ex = V[x];
                
                /* vf order checkme  */
                V[x] >>= 1;
                V[0xF] = (ex & 0x1) ? 1 : 0;
            }

            /* 8XY7  */
            else if(numFirst == 0x8 && numLast == 0x7)
            {
                PC += 2;

                Uint16 ex = V[x];
                Uint16 ey = V[y];

                /* vf order checkme  */
                V[x] = (V[y] - V[x]) & 0xFF;
                V[0xF] = (ey >= ex) ? 1 : 0;
                
            }

            /* 8XYE  */
            else if(numFirst == 0x8 && numLast == 0xE)
            {
                PC += 2;

                if(!isTrue(sChip8Mode) && isTrue(!xoChipMode))/* check  */
                {
                    V[x] = V[y];
                }

                Uint16 ex = V[x];


                /* vf order checkme  */
                V[x] = (V[x] << 1) &  0xFF;
                V[0xF] = (ex >> 7) & 0x1;
            }
            
            /* 9XY0  */
            else if(numFirst == 0x9 && numLast == 0x0)
            {
                PC += 2;

                if(V[x] != V[y])
                {
                    PC += 2;
                }
            }

            /* 9XY1  */
            else if(numFirst == 0x9 && numLast == 0x1 && isTrue(chip8EMode))
            {
                PC += 2;
                
                /* check me */
                V[0xF] = (V[x] * V[y]) >> 8;
                V[x] = (V[x] * V[y]) & 0xFF;

            }

            /* 9XY2 - extended  */
            else if(numFirst == 0x9 && numLast == 0x2 && isTrue(chip8EMode))
            {
                PC += 2;

                V[0xF] = V[x] % V[y];
                V[x] = (V[x] / V[y]) & 0xFF;
            }

            /* 9XY3 - extended  */
            else if(numFirst == 0x9 && numLast == 0x3 && isTrue(chip8EMode))
            {
                PC += 2;
                
                /* check me fixme */
                Uint16 temp = V[x] << 8 | V[y];

                memory[I] = (temp / 10000) % 10;
                memory[I + 1] = (temp / 1000) % 10;
                memory[I + 2] = (temp / 100) % 10;
                memory[I + 3] = (temp / 10) % 10;
                memory[I + 4] = (temp / 0x1) % 10;
            }

            /* ANNN  */
            else if(numFirst == 0xA)
            {
                PC += 2;

                I = nnn;
            }

            /* B0NN  */
            else if(byteFirst == 0xB && isTrue(chip8IMode))
            {
                PC += 2;

                /* todo later  */
            }

            /* B1Y0  */
            else if(byteFirst == 0xB1 && numLast == 0x0 && isTrue(chip8IMode))
            {
                PC += 2;
                /* todo later  */
            }

            /* B1Y1  */
            else if(byteFirst == 0xB1 && numLast == 0x1 && isTrue(chip8IMode))
            {
                PC += 2;
                /* todo later  */
            }

            /* BXY0 - extended  */
            else if(numFirst == 0xB && n == 0x0 && isTrue(chip8XMode))
            {
                PC += 2;
                
                /* todo later */
            }

            /* BXYN  */
            else if(numFirst == 0xB && isTrue(chip8XMode))
            {
                PC += 2;
                /*todo later */
            }

            /* BNNN  */
            else if(numFirst == 0xB)
            {
                if(!isTrue(sChip8Mode))
                {
                    PC = nnn + V[0];
                }
                else{
                    PC = nnn + V[x];
                }
                
            }

            /* CXNN  */
            else if(numFirst == 0xC)
            {
                PC += 2;

                V[x] = (rand() % 0xFF + 0x1) & kk;

            }

            /* DXY0  */
            else if(numFirst == 0xD && numLast == 0x0 && isTrue(sChip8Mode))
            {
                PC += 2;
                
                if(mode12864)
                {
                    V[0xF] = 0;

                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

                    /* draw 16x16 sprite from memory  */
                    size_t row;
                    for(row = 0; row < 16; row++)
                    {
                        size_t col;
                        for(col = 0; col < 16; col++)
                        {
                            if((isTrue(*(Uint16*)&memory[I + row] >> (0xF - col)) & 0x1)) /* checkme */
                            {
                                if( (V[y] + row) >= 64) 
                                {
                                    goto endInstruction;
                                }
                                else if( (V[y] + col) >= 128) 
                                {
                                    break;
                                }
                                else if( (V[y] + row) < 0) 
                                {
                                    break;
                                }
                                else if( (V[y] + col) < 0) 
                                {
                                    continue;
                                }
                                    
                                if(screen[V[x] + col][V[y] + row] == 0x1)
                                {
                                    V[0xF] = 0x1;
                                    screen[V[x] + col][V[y] + row] = 0x0;
                                }
                                else
                                {
                                    screen[V[x] + col][V[y] + row] = 0x1;
                                }
                            }
                        }
                    }
                }

            }


            /* DXYN  */
            else if(numFirst == 0xD)
            {
                PC += 2;

                V[0xF] = 0;

                /* render sprite  */
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
       
                /* draw sprite from memory  */

                /* foreach line/byte  */
                size_t row;
                for(row = 0; row < n; row++)
                {
                    /* foreach pixel/bit in line  */
                    size_t col;
                    for(col = 0; col < 8; col++)
                    {
                        /* checks if the bit at index col in the byte is set  */
                        if(isTrue((*(Uint8*)&memory[I + row] >> (0x7 - col)) & 0x1)) 
                        {
                            if(!isTrue(xoChipMode))
                            {
                                /* check if pixel is out of bounds  */
                                if(isTrue(mode12864)) 
                                { /* if out of bounds of 128 64 screen fixme  */
                                    /* if row of sprite goes out below screen stop drawing sprite  */
                                    if( (V[y] + row) >= 64) 
                                    {
                                        goto endInstruction;
                                    }
                                    /* if the pixel goes out of bounds go to next row  */
                                    else if( (V[y] + col) >= 128) 
                                    {
                                        break;
                                    }
                                    /* if current row is out of bounds go to next row  */
                                    else if( (V[y] + row) < 0) 
                                    {
                                        break;
                                    }
                                    /* if the current pixel is out of bounds go to next pixel  */
                                    else if( (V[y] + col) < 0) 
                                    {
                                        continue;
                                    }
                                } 
                                    
                                /* the same but 64 32 size screen  */
                                else 
                                { 
                                    if( (V[y] + row) >= 32) 
                                    {
                                        goto endInstruction;
                                    }
                                    else if( (V[x] + col) >= 64) 
                                    {
                                        break;
                                    }
                                    else if( (V[y] + row) <= 0) 
                                    {
                                        break;
                                    }
                                    else if( (V[x] + col) <= 0) 
                                    {
                                        continue;
                                    }
                                }

                                /* xor  */
                                /* check if pixel is already set  */
                                if(screen[V[x] + col][V[y] + row] == 0x1)
                                {
                                    V[0xF] = 1;
                                    screen[V[x] + col][V[y] + row] = 0x0;
                                }
                                else
                                {
                                    screen[V[x] + col][V[y] + row] = 0x1;
                                }
                            }
                            else
                            {
                                /* xor  */
                                /* check if pixel is already set  */
                                if(!isTrue(mode12864)) {
                                    if(screen[(V[x] + col) % 64][(V[y] + row) % 32] == 0x1)
                                    {
                                        V[0xF] = 1;
                                        screen[(V[x] + col) % 64][(V[y] + row) % 32] = 0x0;
                                    }
                                    else
                                    {
                                        screen[(V[x] + col) % 64][(V[y] + row) % 32] = 0x1;
                                    }
                                }
                                else
                                {
                                    if(screen[(V[x] + col) % 128][(V[y] + row) % 64] == 0x1)
                                    {
                                        V[0xF] = 1;
                                        screen[(V[x] + col) % 128][(V[y] + row) % 64] = 0x0;
                                    }
                                    else
                                    {
                                        screen[(V[x] + col) % 128][(V[y] + row) % 64] = 0x1;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            /* EX9E  */
            else if(numFirst == 0xE && byteLast == 0x9E)
            {
                PC += 2;

                /* input handling  */
                inputs();
                /* if(keyboard.keyboard & ( V[x]))  */
                if(V[x] == 0x1 && isTrue(keyboard.keys.key1) || V[x] == 0x2 && isTrue(keyboard.keys.key2) || V[x] == 0x3 && isTrue(keyboard.keys.key3) || V[x] == 0xC && isTrue(keyboard.keys.keyC) || V[x] == 0x4 && isTrue(keyboard.keys.key4) || V[x] == 0x5 && isTrue(keyboard.keys.key5) || V[x] == 0x6 && isTrue(keyboard.keys.key6) || V[x] == 0xD && isTrue(keyboard.keys.keyD) || V[x] == 0x7 && isTrue(keyboard.keys.key7) || V[x] == 0x8 && isTrue(keyboard.keys.key8) || V[x] == 0x9 && isTrue(keyboard.keys.key9) || V[x] == 0xE && isTrue(keyboard.keys.keyE) || V[x] == 0xA && isTrue(keyboard.keys.keyA) || V[x] == 0x0 && isTrue(keyboard.keys.key0) || V[x] == 0xB && isTrue(keyboard.keys.keyB) || V[x] == 0xF && isTrue(keyboard.keys.keyF))
                {
                    PC += 2;
                }

            }

            /* EXA1  */
            else if(numFirst == 0xE && byteLast == 0xA1)
            {

                PC += 2;
                /* input handling  */
                inputs();
                /* if(!(keyboard.keyboard & ( V[x])))  */
                if(V[x] == 0x1 && isTrue(!keyboard.keys.key1) || V[x] == 0x2 && isTrue(!keyboard.keys.key2) || V[x] == 0x3 && isTrue(!keyboard.keys.key3) || V[x] == 0xC && isTrue(!keyboard.keys.keyC) || V[x] == 0x4 && isTrue(!keyboard.keys.key4) || V[x] == 0x5 && isTrue(!keyboard.keys.key5) || V[x] == 0x6 && isTrue(!keyboard.keys.key6) || V[x] == 0xD && isTrue(!keyboard.keys.keyD) || V[x] == 0x7 && isTrue(!keyboard.keys.key7) || V[x] == 0x8 && isTrue(!keyboard.keys.key8) || V[x] == 0x9 && isTrue(!keyboard.keys.key9) || V[x] == 0xE && isTrue(!keyboard.keys.keyE) || V[x] == 0xA && isTrue(!keyboard.keys.keyA) || V[x] == 0x0 && isTrue(!keyboard.keys.key0) || V[x] == 0xB && isTrue(!keyboard.keys.keyB) || V[x] == 0xF && isTrue(!keyboard.keys.keyF))
                {
                    PC += 2;
                }
            }

            /* EXF2  */
            else if(numFirst == 0xE && byteLast == 0xF2)
            {
                PC += 2;
                /* todo later  */
            }

            /* EXF5  */
            else if(numFirst == 0xE && byteLast == 0xF5)
            {
                PC += 2;
                /* todo later  */
            }

            /* F000 */
            else if(opcode == 0xF000)
            {
                PC += 2;

                I = *(Uint16*)&memory[PC];
                PC += 2;
            }

            /* FX01  */
            else if(numFirst == 0xF && byteLast == 0x1)
            {
                PC += 2;
                /* todo later  */
            }

            /* F002 */
            else if(opcode == 0xF002)
            {
                PC += 2;
                
                memcpy(&patternBuffer, &memory[I], 16); /* CHECKME */
                updateAudioPattern(&patternBuffer);
            }

            /* FX07  */
            else if(numFirst == 0xF && byteLast == 0x07)
            {
                PC += 2;

                p_mutex_lock(mutex);
                V[x] = delayTimer;
                p_mutex_unlock(mutex);
            }

            /* FX0A  */
            else if(numFirst == 0xF && byteLast == 0x0A)
            {
                if(!isTrue(sChip8Mode) && !isTrue(xoChipMode)) /* fixme - does xoChipMode and other modes use original input here?  */
                {
                    inputs();
                    if(keyToggle == false)
                    {
                        lastKeyPressed = 255;
                        lastKeyUp = 255;
                        keyToggle = true;
                    }
                    else
                    {
                        if(lastKeyPressed != 255 && lastKeyUp != 255)
                        {
                            if(lastKeyPressed == lastKeyUp)
                            {
                                V[x] = lastKeyPressed;
                                keyToggle = false;
                                PC += 2;
                            }
                        }
                    }
                }
                else{
                    inputs();
                    
                    if(isTrue(keyboard.keys.key1))
                    {
                        V[x] = 0x1;
                        PC += 2;
                        
                    }
                    else if(isTrue(keyboard.keys.key2))
                    {
                        V[x] = 0x2;
                        PC += 2;
                        
                    }
                    /*
                    if(lastLastKeyPressed != 255 && lastLastKeyPressed != 117)
                    {
                        lastLastKeyPressed = lastKeyPressed;
                    }
                    
                    if(lastKeyUp == lastLastKeyPressed)
                    {
                        lastLastKeyPressed = 117;
                    }

                    if(lastKeyPressed != 255)
                    {
                        V[x] = lastKeyPressed;
                        PC += 2;
                        lastKeyPressed = 255;
                        lastLastKeyPressed = 255;
                        
                    }
                    */
                    else if(isTrue(keyboard.keys.key3))
                    {
                        V[x] = 0x3;
                        PC += 2;
                        
                    }
                    else if(isTrue(keyboard.keys.keyC))
                    {
                        V[x] = 0xC;
                        PC += 2;
                        
                    }
                    else if(isTrue(keyboard.keys.key4))
                    {
                        V[x] = 0x4;
                        PC += 2;
                        
                    }
                    else if(isTrue(keyboard.keys.key5))
                    {
                        V[x] = 0x5;
                        PC += 2;
                        
                    }
                    else if(isTrue(keyboard.keys.key6))
                    {
                        V[x] = 0x6;
                        PC += 2;
                        
                    }
                    else if(isTrue(keyboard.keys.keyD))
                    {
                        V[x] = 0xD;
                        PC += 2;
                        
                    }
                    else if(isTrue(keyboard.keys.key7))
                    {
                        V[x] = 0x7;
                        PC += 2;
                        
                    }
                    else if(isTrue(keyboard.keys.key8))
                    {
                        V[x] = 0x8;
                        PC += 2;
                        
                    }
                    else if(isTrue(keyboard.keys.key9))
                    {
                        V[x] = 0x9;
                        PC += 2;
                        
                    }
                    else if(isTrue(keyboard.keys.keyE))
                    {
                        V[x] = 0xE;
                        PC += 2;
                        
                    }
                    else if(isTrue(keyboard.keys.keyA))
                    {
                        V[x] = 0xA;
                        PC += 2;
                        
                    }
                    else if(isTrue(keyboard.keys.key0))
                    {
                        V[x] = 0x0;
                        PC += 2;
                        
                    }
                    else if(isTrue(keyboard.keys.keyB))
                    {
                        V[x] = 0xB;
                        PC += 2;
                        
                    }
                    else if(isTrue(keyboard.keys.keyF))
                    {
                        V[x] = 0xF;
                        PC += 2;
                        
                    }
                    
                }
                
            }

            /* FX15  */
            else if(numFirst == 0xF && byteLast == 0x15)
            {
                PC += 2;

                p_mutex_lock(mutex);
                delayTimer = V[x];
                p_mutex_unlock(mutex);
            }

            /* FX18  */
            else if(numFirst == 0xF && byteLast == 0x18)
            {
                PC += 2;

                p_mutex_lock(mutex);
                soundTimer = V[x];
                p_mutex_unlock(mutex);
            }

            /* FX1E  */
            else if(numFirst == 0xF && byteLast == 0x1E)
            {
                PC += 2;

                V[0xF] = (I + V[x]) > 0xFFF ? 1 : 0; /* qwerk checkme  */
                I += V[x];
            }

            /* FX29  */
            else if(numFirst == 0xF && byteLast == 0x29)
            {
                PC += 2;
                
                I = V[x] * 5;

            }

            /* FX30  */
            else if(numFirst == 0xF && byteLast == 0x30 && isTrue(sChip8Mode))
            {
                PC += 2;

                I = 80 + (V[x] * 5);

            }

            /* FX33  */
            else if(numFirst == 0xF && byteLast == 0x33)
            {
                PC += 2;
                /* checkme  */
                memory[I] = (V[x] / 100) % 10;
                memory[I + 1] = (V[x] / 10) % 10;
                memory[I + 2] = (V[x] / 0x1) % 10;

            }

            /* FX3A  */
            else if(numFirst == 0xF && byteLast == 0x3A)
            {
                PC += 2;
                
                changeAudioData(V[x]);

            }

            /* FX55 checkme  */
            else if(numFirst == 0xF && byteLast == 0x55)
            {
                PC += 2;

                int ii;
                for(ii = 0; ii <= x; ii++)
                {
                    memory[I + ii] = V[ii];
                }

                if(!isTrue(sChip8Mode) || isTrue(iToggle))
                {
                    I = I + x + 1;
                }

            }

            /* FX65  */
            else if(numFirst == 0xF && byteLast == 0x65)
            {
                PC += 2;

                int ii;
                for(ii = 0; ii <= x; ii++)
                {
                    V[ii] = memory[I + ii];
                }

                if(!isTrue(sChip8Mode) || isTrue(iToggle))
                {
                    I = I + x + 1;
                }
                

            }

            /* FX75 */
            else if(numFirst == 0xF && byteLast == 0x75)
            {
                if(chip8EMode)
                {
                    PC += 2;

                    /* todo later */
                }
                else if(isTrue(sChip8Mode))
                {
                    PC += 2;

                    if(x < 8)
                    {
                        memcpy(flags, V, (x + 1) * (sizeof(V[0])/sizeof(Uint8))); /* checkme */
                    }
                }
                else if(isTrue(xoChipMode))
                {
                    PC += 2;

                    memcpy(flags, V, (x + 1) * (sizeof(V[0])/sizeof(Uint8))); /* checkme */
                }
            }


            /* FX85 */
            else if(numFirst == 0xF && byteLast == 0x85 && (isTrue(sChip8Mode) || isTrue(xoChipMode)))
            {
 
                if(isTrue(sChip8Mode))
                {
                    PC += 2;

                    if(x < 8)
                    {
                        memcpy(V, flags, (x + 1) * (sizeof(V[0])/sizeof(Uint8))); /* checkme */
                    }
                }
                else if(isTrue(xoChipMode))
                {
                    PC += 2;

                    memcpy(V, flags, (x + 1) * (sizeof(V[0])/sizeof(Uint8))); /* checkme */
                }

            }

            /* FX94 */
            else if(numFirst == 0xF && byteLast == 0x94 && chip8EMode)
            {
                PC += 2;
                /* todo later */
                /* fix me */
                /* checkme */

                switch(V[x])
                {
                    case 0x00:

                        break;

                    case 0x01:
                        I = 0x80;
                        break;

                    case 0x02:
                        I = 0x85;
                        break;

                    case 0x03:
                        I = 0x8A;
                        break;

                    case 0x04:
                        I = 0x8F;
                        break;

                    case 0x05:
                        I = 0x94;
                        break;

                    case 0x06:
                        I = 0x99;
                        break;

                    case 0x07:
 
                        break;

                    case 0x08:

                        break;

                    case 0x09:

                        break;

                    case 0x0A:

                        break;

                    case 0x0B:

                        break;

                    case 0x0C:

                        break;

                    case 0x0D:

                        break;

                    case 0x0E:

                        break;

                    case 0x0F:

                        break;

                    case 0x10:

                        break;

                    case 0x11:

                        break;

                    case 0x12:

                        break;

                    case 0x13:

                        break;

                    case 0x14:

                        break;

                    case 0x15:

                        break;

                    case 0x16:

                        break;

                    case 0x17:

                        break;

                    case 0x18:

                        break;

                    case 0x19:

                        break;

                    case 0x1A:/**/

                        break;

                    case 0x1B:

                        break;

                    case 0x1C:

                        break;

                    case 0x1D:

                        break;

                    case 0x1E:

                        break;

                    case 0x1F:

                        break;

                    case 0x20:

                        break;

                    case 0x21:

                        break;

                    case 0x22:

                        break;

                    case 0x23:

                        break;

                    case 0x24:

                        break;

                    case 0x25:

                        break;

                    case 0x26:

                        break;

                    case 0x27:

                        break;

                    case 0x28:

                        break;

                    case 0x29:

                        break;

                    case 0x2A:

                        break;

                    case 0x2B:

                        break;

                    case 0x2C:

                        break;

                    case 0x2D:

                        break;

                    case 0x2E:

                        break;

                    case 0x2F:

                        break;

                    case 0x30:

                        I = 0x0;
                        break;

                    case 0x31:
                        I = 0x5;
                        break;

                    case 0x32:
                        I = 0xA;
                        break;

                    case 0x33:
                        I = 0xF;
                        break;

                    case 0x34:
                        I = 0x14;
                        break;

                    case 0x35:
                        I = 0x19;
                        break;

                    case 0x36:
                        I = 0x1E;
                        break;

                    case 0x37:
                        I = 0x23;
                        break;

                    case 0x38:
                        I = 0x28;
                        break;

                    case 0x39:
                        I = 0x2D;
                        break;

                    case 0x3A:

                        break;

                    case 0x3B:

                        break;

                    case 0x3C:

                        break;

                    case 0x3D:

                        break;

                    case 0x3E:

                        break;

                    case 0x3F:

                        break;

                    default:
                        break;

                    
                }
            }

            /* FXFB   */
            else if(numFirst == 0xF && byteLast == 0xFB && chip8XMode)
            {
                PC += 2;
                /*todo later */
            }

            /* FXF8 */
            else if(numFirst == 0xF && byteLast == 0xF8 && chip8XMode)
            {
                PC += 2;
                /*todo later  */
            }

            else
            {
                if(!ETI660Mode)
                {
                    printf("Unhandled opcode: %X\n At address: %X\n", opcode, PC - 0x200);
                }
                else
                {
                    printf("Unhandled opcode: %X\n At address: %X\n", opcode, PC-0x600);
                }
                
            }
        }

        endInstruction:;

        /* GPU */
        int scaled = scale;
        /* check if super mode is enabled */
        /* if 64 32 aka not super chip scale to 128 64 */
        /* fixme handle other display modes */
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

        /* pixel color */
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        int row;
        for(row = 0; row < 64; row++)
        {
            int col;
            for(col = 0; col < 128; col++)
            {
                if(screen[col][row] == 0x1)
                {
                    /* draw pixel */
                    SDL_Rect rect = { col * scaled, row * scaled, scaled, scaled};
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
        SDL_RenderPresent(renderer);

        
    }

    return;
}


