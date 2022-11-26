#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

#define MAX_KEY_CODE_CACHE 1 << 12
u32 KEY_CODE_CACHE[MAX_KEY_CODE_CACHE];

#define MIN_KEY_CODE 20
#define MAX_KEY_CODE 128

#define MAX_TEXTURE_CACHE_SIZE (MAX_KEY_CODE - MIN_KEY_CODE)
SDL_Texture *TEXTURE_CACHE[MAX_TEXTURE_CACHE_SIZE];
gui_glyph_metric GLYPH_METRIC_CACHE[MAX_TEXTURE_CACHE_SIZE];

#define KeyCodeIsAlpha(Code) ((Code) >= SDLK_a && (Code) <= SDLK_z)
#define KeyModShift(Mod) (((Mod) & (KMOD_LSHIFT | KMOD_RSHIFT)) ? 1 : 0)
#define KeyModCaps(Mod) (((Mod) & KMOD_CAPS) ? 1 : 0)

char *FONT_PATH[] = {
    "src/PTMono-Regular.ttf",
    "src/ZapfDingbats.ttf",
    "src/Bodoni Ornaments.ttf",
};

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

static void InitTextureCache(SDL_Renderer *Renderer, gui_state *State)
{
    u32 I;

    TTF_Font *Font = TTF_OpenFont(FONT_PATH[0], State->FontData.Size);
    if(Font == 0) PrintError("TTF_OpenFont");

    State->FontData.Ascent = TTF_FontAscent(Font);
    State->FontData.Descent = TTF_FontDescent(Font);
    State->FontData.Height = TTF_FontHeight(Font);

    SDL_Color FontColorFG = {220,200,0,255};
    SDL_Color FontColorBG = {0,0,0,255};

    for (I = MIN_KEY_CODE; I < MAX_KEY_CODE; I++)
    {
        u32 CacheIndex = I - MIN_KEY_CODE;
        s32 MinX = 0;
        s32 MaxX = 0;
        s32 MinY = 0;
        s32 MaxY = 0;
        s32 Advance = 0;
        assert(Font);
        SDL_Surface *Surface = TTF_RenderGlyph_Shaded(Font, I, FontColorFG, FontColorBG);
        /* SDL_Surface *Surface = TTF_RenderGlyph_Solid(Font, I, FontColorFG); */


        assert(Surface);
        TEXTURE_CACHE[CacheIndex] = SDL_CreateTextureFromSurface(Renderer, Surface);

        TTF_GlyphMetrics(Font, I, &MinX, &MaxX, &MinY, &MaxY, &Advance);
        GLYPH_METRIC_CACHE[CacheIndex].MinX = MinX;
        GLYPH_METRIC_CACHE[CacheIndex].MaxX = MaxX;
        GLYPH_METRIC_CACHE[CacheIndex].MinY = MinY;
        GLYPH_METRIC_CACHE[CacheIndex].MaxY = MaxY;
        GLYPH_METRIC_CACHE[CacheIndex].Advance = Advance;

        SDL_FreeSurface(Surface);
    }
}

static void DeInit(SDL_Window *Window, SDL_Renderer *Renderer)
{
    printf("DeInit\n");
    u32 I;
    for (I = 0; I < MAX_TEXTURE_CACHE_SIZE; I++)
    {
        SDL_DestroyTexture(TEXTURE_CACHE[I]);
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

static gui_state InitGuiState()
{
    gui_state Result;
    Result.Running = 1;
    Result.FontData.Size = 36;
    Result.Cursor.BufferIndex = 0;
    Result.Cursor.X = 0;
    Result.Cursor.Y = 0;

    return(Result);
}

static b32 HandleEvents(gui_state *State)
{
    SDL_Event Event;
    b32 HadKeyboardEvent = 0;

    while(SDL_PollEvent(&Event))
    {
        switch(Event.type)
        {
        case SDL_KEYDOWN:
        {
            HadKeyboardEvent = 1;

            if(Event.key.keysym.mod)
            {
                switch(Event.key.keysym.mod)
                {
                case KMOD_LCTRL:
                {
                    switch(Event.key.keysym.sym)
                    {
                    case SDLK_MINUS:
                    {

                    } break;
                    case SDLK_EQUALS:
                    {

                    } break;
                    }
                } break;
                }

            }

            if(Event.key.keysym.sym >= MIN_KEY_CODE &&
               Event.key.keysym.sym <= MAX_KEY_CODE)
            {
                s32 UppercaseOffset = 0;
                if(KeyCodeIsAlpha(Event.key.keysym.sym) &&
                   (KeyModShift(Event.key.keysym.mod) ^ KeyModCaps(Event.key.keysym.mod)))
                {
                    UppercaseOffset = -32;
                }
                KEY_CODE_CACHE[State->Cursor.BufferIndex] = (Event.key.keysym.sym + UppercaseOffset) - MIN_KEY_CODE;
                ++State->Cursor.BufferIndex;
                if(State->Cursor.BufferIndex >= MAX_KEY_CODE_CACHE)
                {
                    PrintError("key code buffer overflow");
                    State->Running = 0;
                }
            }
        } break;
        case SDL_QUIT:
        {
            State->Running = 0;
        } break;
        }
    }

    return HadKeyboardEvent;
}

void DisplayWindow()
{
    printf("DisplayWindow\n");
    result InitResult = Init();

    if(InitResult == result_Error)
    {
        PrintError("Init error");
        return;
    }

    SDL_Rect DEBUG_Rect = CreateRect(10, 10, 12, 20);

    SDL_Window *Window = SDL_CreateWindow("idi", 0, 0,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_HIDDEN);
    SDL_Renderer *Renderer = SDL_CreateRenderer(Window, -1, 0);
    assert(Renderer);

    gui_state State = InitGuiState();
    InitTextureCache(Renderer, &State);

    SDL_ShowWindow(Window);

    u32 DelayInMilliseconds = 32;

    while(State.Running)
    {
        b32 HadKeyboardEvent = HandleEvents(&State);

        if(HadKeyboardEvent)
        {
            SDL_RenderClear(Renderer);

#if 1
            {
                DEBUG_Rect.x = 0;
                u32 I;
                s32 Baseline = 12;

                for(I = 0; I < State.Cursor.BufferIndex; ++I)
                {
                    u32 KeyCodeIndex = KEY_CODE_CACHE[I];
                    gui_glyph_metric GlyphMetric = GLYPH_METRIC_CACHE[KeyCodeIndex];
                    SDL_Texture *Texture = TEXTURE_CACHE[KeyCodeIndex];


                    if((DEBUG_Rect.x + GlyphMetric.Advance) > SCREEN_WIDTH)
                    {
                        DEBUG_Rect.x = 0;
                        Baseline += State.FontData.Height;
                    }

                    DEBUG_Rect.x += GlyphMetric.Advance / 2;
                    DEBUG_Rect.y = Baseline - State.FontData.Descent;
                    DEBUG_Rect.y += 5;
                    SDL_RenderCopy(Renderer, Texture, NULL, &DEBUG_Rect);
                }
            }
#endif

            SDL_RenderPresent(Renderer);
        }
        SDL_Delay(DelayInMilliseconds);
    }

    DeInit(Window, Renderer);
}
