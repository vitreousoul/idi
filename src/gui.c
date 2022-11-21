#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

#define MAX_KEY_CODE_BUFFER 1 << 12
u32 KEY_CODE_BUFFER[MAX_KEY_CODE_BUFFER];

#define MIN_KEY_CODE 20
#define MAX_KEY_CODE 128
#define MAX_TEXTURE_BUFFER (MAX_KEY_CODE - MIN_KEY_CODE)
SDL_Texture *TEXTURE_BUFFER[MAX_TEXTURE_BUFFER];

char *FONT_PATH = "src/PTMono-Regular.ttf";

static result Init()
{
    printf("Init\n");
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

static void InitTextureCache(SDL_Renderer *Renderer)
{
    u32 I;
    TTF_Font *Font = TTF_OpenFont(FONT_PATH, 12);
    if(Font == 0) PrintError("TTF_OpenFont");
    SDL_Color FontColorFG = {220,200,0,255};
    SDL_Color FontColorBG = {0,0,0,255};

    for (I = MIN_KEY_CODE; I < MAX_KEY_CODE; I++)
    {
        assert(Font);
        SDL_Surface *Surface = TTF_RenderGlyph_Shaded(Font, I, FontColorFG, FontColorBG);
        assert(Surface);
        TEXTURE_BUFFER[I - MIN_KEY_CODE] = SDL_CreateTextureFromSurface(Renderer, Surface);
        SDL_FreeSurface(Surface);
    }
}

static void DeInit(SDL_Window *Window, SDL_Renderer *Renderer)
{
    printf("DeInit\n");
    u32 I;
    for (I = 0; I < MAX_TEXTURE_BUFFER; I++)
    {
        SDL_DestroyTexture(TEXTURE_BUFFER[I]);
    }

    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    TTF_Quit();
    SDL_Quit();
}

static SDL_Rect CreateRect(u32 x, u32 y, u32 w, u32 h)
{
    printf("CreateRect\n");
    SDL_Rect Result;
    Result.x = x;
    Result.y = y;
    Result.w = w;
    Result.h = h;

    return(Result);
}

void DisplayWindow()
{
    printf("DisplayWindow\n");
    result InitResult = Init();

    if(InitResult == result_Error)
    {
        return;
    }

    SDL_Rect DEBUG_Rect = CreateRect(10, 10, 12, 20);

    SDL_Window *Window = SDL_CreateWindow("idi", 0, 0,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_HIDDEN);
    SDL_Renderer *Renderer = SDL_CreateRenderer(Window, -1, 0);

    assert(Renderer);
    InitTextureCache(Renderer);

    cursor Cursor;
    Cursor.BufferIndex = 0;
    Cursor.X = 0;
    Cursor.Y = 0;

    SDL_ShowWindow(Window);

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
                if(Event.key.keysym.sym >= MIN_KEY_CODE && Event.key.keysym.sym <= MAX_KEY_CODE)
                {
                    KEY_CODE_BUFFER[Cursor.BufferIndex] = Event.key.keysym.sym - MIN_KEY_CODE;
                    ++Cursor.BufferIndex;
                    assert(Cursor.BufferIndex < MAX_KEY_CODE_BUFFER);
                }
            } break;
            case SDL_QUIT:
            {
                Running = 0;
            } break;
            }
        }

        SDL_RenderClear(Renderer);

        #if 1
        {
            u32 ScaleX = 12;
            u32 ScaleY = 20;
            DEBUG_Rect.x = 0;
            DEBUG_Rect.y = 0;
            u32 I;

            for(I = 0; I < Cursor.BufferIndex; ++I)
            {
                DEBUG_Rect.x = (I * ScaleX) % SCREEN_WIDTH;
                DEBUG_Rect.y = ((I * ScaleX) / SCREEN_WIDTH) * ScaleY;
                SDL_Texture *Texture = TEXTURE_BUFFER[KEY_CODE_BUFFER[I]];
                SDL_RenderCopy(Renderer, Texture, NULL, &DEBUG_Rect);
                DEBUG_Rect.x += ScaleX;
            }
        }
        #endif

        SDL_RenderPresent(Renderer);
        SDL_Delay(DelayInMilliseconds);
    }

    DeInit(Window, Renderer);
}
