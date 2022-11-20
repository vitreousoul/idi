#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

char *FONT_PATH = "src/PTMono-Regular.ttf";

static result Init()
{
    result Result = result_Ok;
    int TTFResult;

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        Result = result_Error;
        PrintError("SDL_Init error");
    }

    if((TTFResult = TTF_Init()) < 0)
    {
        Result = result_Error;
        PrintError("TTF_Init error");
    }

    return(Result);
}

static SDL_Rect CreateRect(u32 x, u32 y, u32 w, u32 h)
{
    SDL_Rect Result;
    Result.x = x;
    Result.y = y;
    Result.w = w;
    Result.h = h;

    return(Result);
}

static char *CharFromKeyCode(u32 KeyCode)
{
    switch(KeyCode)
    {
    case SDLK_0: return "0";
    case SDLK_1: return "1";
    case SDLK_2: return "2";
    case SDLK_3: return "3";
    case SDLK_4: return "4";
    case SDLK_5: return "5";
    case SDLK_6: return "6";
    case SDLK_7: return "7";
    case SDLK_8: return "8";
    case SDLK_9: return "9";
    case SDLK_a: return "a";
    case SDLK_b: return "b";
    case SDLK_c: return "c";
    case SDLK_d: return "d";
    case SDLK_e: return "e";
    case SDLK_f: return "f";
    case SDLK_g: return "g";
    case SDLK_h: return "h";
    case SDLK_i: return "i";
    case SDLK_j: return "j";
    case SDLK_k: return "k";
    case SDLK_l: return "l";
    case SDLK_m: return "m";
    case SDLK_n: return "n";
    case SDLK_o: return "o";
    case SDLK_p: return "p";
    case SDLK_q: return "q";
    case SDLK_r: return "r";
    case SDLK_s: return "s";
    case SDLK_t: return "t";
    case SDLK_u: return "u";
    case SDLK_v: return "v";
    case SDLK_w: return "w";
    case SDLK_x: return "x";
    case SDLK_y: return "y";
    case SDLK_z: return "z";
    default: return " ";
    }
}

void DisplayWindow()
{
    result InitResult = Init();

    if(InitResult == result_Error)
    {
        return;
    }

    SDL_Rect DEBUG_Rect = CreateRect(10, 10, 40, 40);

    SDL_Window *Window = SDL_CreateWindow("idi", 0, 0,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_HIDDEN);
    SDL_Renderer *Renderer = SDL_CreateRenderer(Window, -1, 0);

    TTF_Font *Font = TTF_OpenFont(FONT_PATH, 24);
    if(Font == 0) PrintError("TTF_OpenFont");
    SDL_Color FontColor = {220,200,0,255};

    SDL_Surface *Surface = TTF_RenderText_Blended(Font, " ", FontColor);
    SDL_Texture *Texture = SDL_CreateTextureFromSurface(Renderer, Surface);

    SDL_ShowWindow(Window);
    SDL_RenderCopy(Renderer, Texture, NULL, &DEBUG_Rect);
    SDL_RenderPresent(Renderer);

    SDL_Event Event;
    u32 Running = 1;
    u32 DelayInMilliseconds = 32;

    while(Running)
    {
        while(SDL_PollEvent(&Event))
        {
            switch(Event.type)
            {
            case SDL_KEYDOWN:
            {
                char *KeyString = CharFromKeyCode(Event.key.keysym.sym);
                SDL_Surface *Surface = TTF_RenderText_Blended(Font, KeyString, FontColor);
                SDL_Texture *Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
                SDL_RenderCopy(Renderer, Texture, NULL, &DEBUG_Rect);
                SDL_DestroyTexture(Texture);
                SDL_FreeSurface(Surface);
            } break;
            case SDL_QUIT:
            {
                Running = 0;
            } break;
            }
        }

        SDL_RenderPresent(Renderer);
        SDL_Delay(DelayInMilliseconds);
    }

    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    TTF_Quit();
    SDL_DestroyTexture(Texture);
    SDL_FreeSurface(Surface);
    SDL_Quit();
}
