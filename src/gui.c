#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

#define MAX_KEY_CODE_CACHE 1 << 12
u32 KEY_CODE_CACHE[MAX_KEY_CODE_CACHE];

#define MIN_KEY_CODE 20
#define MAX_KEY_CODE 128

#define MAX_TEXTURE_CACHE_SIZE (MAX_KEY_CODE - MIN_KEY_CODE)
SDL_Texture *TEXTURE_CACHE[MAX_TEXTURE_CACHE_SIZE];
gui_char_data CHAR_DATA_CACHE[MAX_TEXTURE_CACHE_SIZE];

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

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        Result = result_Error;
        PrintError("SDL_Init error");
    }

    return(Result);
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
    SDL_Quit();
}

static stbtt_fontinfo InitTextureCache(SDL_Renderer *Renderer, gui_state *State, f32 *Scale, f32 PixelHeight)
{
    stbtt_fontinfo FontInfo;
    gui_char_data CharData;
    gui_stb_bitmap Bitmap;
    buffer *Buffer = ReadFileIntoBuffer(FONT_PATH[0]);

    stbtt_InitFont(&FontInfo, Buffer->Data, stbtt_GetFontOffsetForIndex(Buffer->Data, 0));
    *Scale = stbtt_ScaleForPixelHeight(&FontInfo, PixelHeight);
    Bitmap.Scale = *Scale;

    for (u32 I = MIN_KEY_CODE; I < MAX_KEY_CODE; I++)
    {
        u32 CacheIndex = I - MIN_KEY_CODE;

        stbtt_GetCodepointBox(&FontInfo, I,
                              &CharData.X0, &CharData.Y0,
                              &CharData.X1, &CharData.Y1);
        Bitmap.At = stbtt_GetCodepointBitmap(&FontInfo, 0, *Scale, I,
                                             &Bitmap.Width, &Bitmap.Height,
                                             &CharData.XOffset, &CharData.YOffset);
        printf("Offset (x,y): (%d,%d)\n", CharData.XOffset, CharData.YOffset);
        u32 Pixels[Bitmap.Width * Bitmap.Height];

        if(!(Bitmap.Width && Bitmap.Height)) continue;

        for(s32 Y = 0; Y < Bitmap.Height; ++Y)
        {
            for(s32 X = 0; X < Bitmap.Width; ++X)
            {
                u32 I = Y * Bitmap.Width + X;
                u8 Value = Bitmap.At[I];
                Pixels[I] =
                    (Value << 24) |
                    (Value << 16) |
                    (Value << 8) |
                    (Value << 0);
            }
        }

        SDL_Texture *Texture = SDL_CreateTexture(Renderer,
                                                 SDL_PIXELFORMAT_RGBA8888,
                                                 SDL_TEXTUREACCESS_STATIC,
                                                 Bitmap.Width,
                                                 Bitmap.Height);
        int UpdateTextureError =  SDL_UpdateTexture(Texture, 0, Pixels, Bitmap.Width * 4);
        assert(!UpdateTextureError);
        TEXTURE_CACHE[CacheIndex] = Texture;
        CHAR_DATA_CACHE[CacheIndex] = CharData;
        stbtt_FreeBitmap(Bitmap.At, 0);
    }

    return(FontInfo);
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

    SDL_Rect DEBUG_Rect = CreateRect(0, 0, 12, 20);
    SDL_Window *Window = SDL_CreateWindow("idi", 0, 0,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_HIDDEN);
    assert(Window);
    SDL_Renderer *Renderer = SDL_CreateRenderer(Window, -1, 0);
    assert(Renderer);

    gui_state State = InitGuiState();

    f32 PixelHeight = 32;
    f32 Scale;
    s32 AdvanceWidth;
    s32 LeftSideBearing;
    stbtt_fontinfo FontInfo = InitTextureCache(Renderer, &State, &Scale, PixelHeight);
    gui_rect FontBoundingRect;
    stbtt_GetFontBoundingBox(&FontInfo,
                             &FontBoundingRect.X0, &FontBoundingRect.Y0,
                             &FontBoundingRect.X1, &FontBoundingRect.Y1);
    printf("Scale %f\n", Scale);
    printf("Y0 %d\n", FontBoundingRect.Y0);
    State.Cursor.Y = Scale - FontBoundingRect.Y0;
    State.Cursor.X = 0;

    u32 DelayInMilliseconds = 32;

    SDL_ShowWindow(Window);

    while(State.Running)
    {
        State.Cursor.X = 0;
        State.Cursor.Y = 32;
        b32 HadKeyboardEvent = HandleEvents(&State);

        if(HadKeyboardEvent)
        {
            SDL_RenderClear(Renderer);

            {
                DEBUG_Rect.x = 0;
                u32 I;

                for(I = 0; I < State.Cursor.BufferIndex; ++I)
                {
                    u32 KeyCodeIndex = KEY_CODE_CACHE[I];
                    SDL_Texture *Texture = TEXTURE_CACHE[KeyCodeIndex];
                    gui_char_data CharData = CHAR_DATA_CACHE[KeyCodeIndex];

                    DEBUG_Rect.x = State.Cursor.X + Scale * CharData.X0;
                    DEBUG_Rect.y = State.Cursor.Y + Scale * CharData.Y0;
                    DEBUG_Rect.w = (State.Cursor.X + Scale * CharData.X1) - DEBUG_Rect.x;
                    DEBUG_Rect.h = (State.Cursor.Y + Scale * CharData.Y1) - DEBUG_Rect.y;

                    stbtt_GetGlyphHMetrics(&FontInfo, KeyCodeIndex, &AdvanceWidth, &LeftSideBearing);
                    s32 NextX = State.Cursor.X + Scale * AdvanceWidth;

                    if(NextX > SCREEN_WIDTH)
                    {
                        State.Cursor.X = 0;
                        State.Cursor.Y += 28;
                    }
                    else
                    {
                        State.Cursor.X = NextX;
                    }

                    DEBUG_Rect.x = State.Cursor.X + CharData.XOffset;
                    DEBUG_Rect.y = State.Cursor.Y + CharData.YOffset;

                    SDL_RenderCopy(Renderer, Texture, NULL, &DEBUG_Rect);
                }
            }

            SDL_RenderPresent(Renderer);
        }
        SDL_Delay(DelayInMilliseconds);
    }

    DeInit(Window, Renderer);
}
