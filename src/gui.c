#define SCREEN_WIDTH 880
#define SCREEN_HEIGHT 400

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

#define MAX_DIALOG_COUNT 26
char *DIALOG[MAX_DIALOG_COUNT] = {
    "any time is present",
    "bathing in a bowl of dirt",
    "crisps are just chips",
    "dead of winter",
    "eastward wind",
    "forgotten songs",
    "grubs in the dirt",
    "happy chirping sounds",
    "iridescent hues",
    "jaunty geese",
    "kelp pancakes",
    "little baby birds",
    "misty mountains",
    "neural pathways",
    "oily doilies",
    "present company",
    "querent envy",
    "really really big",
    "sacred words",
    "tinny biddies",
    "unheard birds",
    "vulture chores",
    "wasp nest",
    "x-ray play",
    "yonder over there",
    "zzz so sleepy",
};

char *FONT_PATH[] = {
    "src/PTMono-Regular.ttf",
    "src/ZapfDingbats.ttf",
    "src/Bodoni Ornaments.ttf",
    "src/Monaco.ttf",
    "src/Arial Black.ttf",
};

#define FONT_PATH_INDEX 4
#define FONT_HEIGHT_IN_PIXELS 20

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

static stbtt_fontinfo InitTextureCache(SDL_Renderer *Renderer, f32 *Scale, f32 PixelHeight)
{
    u32 I;
    s32 Y, X;
    stbtt_fontinfo FontInfo;
    gui_char_data CharData;
    gui_stb_bitmap Bitmap;
    buffer *Buffer = ReadFileIntoBuffer(FONT_PATH[FONT_PATH_INDEX]);

    stbtt_InitFont(&FontInfo, Buffer->Data, stbtt_GetFontOffsetForIndex(Buffer->Data, 0));
    *Scale = stbtt_ScaleForPixelHeight(&FontInfo, PixelHeight);

    for (I = MIN_KEY_CODE; I < MAX_KEY_CODE; I++)
    {
        u32 CacheIndex = I - MIN_KEY_CODE;

        stbtt_GetCodepointBox(&FontInfo, I,
                              &CharData.X0, &CharData.Y0,
                              &CharData.X1, &CharData.Y1);
        Bitmap.At = stbtt_GetCodepointBitmap(&FontInfo, 0, *Scale, I,
                                             &Bitmap.Width, &Bitmap.Height,
                                             &CharData.XOffset, &CharData.YOffset);
        u32 Pixels[Bitmap.Width * Bitmap.Height];

        if(!(Bitmap.Width && Bitmap.Height)) continue;

        for(Y = 0; Y < Bitmap.Height; ++Y)
        {
            for(X = 0; X < Bitmap.Width; ++X)
            {
                u32 I = Y * Bitmap.Width + X;
                u8 Value = Bitmap.At[I];
                if(Value > 200)
                {
                    Value = 255;
                }
                else if(Value > 127)
                {
                    u8 Brighten = Value + (Value >> 1);
                    Value = Value + Brighten > 255 ? 255 : Value + Brighten;
                }
                Pixels[I] =
                    (Value << 24) |
                    (Value << 16) |
                    (Value << 8) |
                    (Value << 0);
            }
        }
        SDL_Texture *Texture = SDL_CreateTexture(Renderer,
                                                 SDL_PIXELFORMAT_RGBA32,
                                                 SDL_TEXTUREACCESS_STATIC,
                                                 Bitmap.Width,
                                                 Bitmap.Height);
        int UpdateTextureError = SDL_UpdateTexture(Texture, 0, Pixels, Bitmap.Width * 4);
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
    Result.Color.R = 255;
    Result.Color.G = 255;
    Result.Color.B = 255;
    Result.Dialog.Index = 0;
    Result.Dialog.CharIndex = 0;
    Result.Dialog.Writing = 1;

    return(Result);
}

static b32 HandleEvents(gui_state *State)
{
    SDL_Event Event;
    b32 EventNeedsRenderUpdate = 0;

    while(SDL_PollEvent(&Event))
    {
        switch(Event.type)
        {
        case SDL_KEYDOWN:
        {
            EventNeedsRenderUpdate = 1;
            State->Dialog.CharIndex = 0;
            State->Dialog.Index = (State->Dialog.Index + 1) % MAX_DIALOG_COUNT;
            State->Dialog.Writing = 1;

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
        case SDL_MOUSEBUTTONDOWN:
        {
            State->Color.R = 255;
            State->Color.G = 20;
            State->Color.B = 255;
        } break;
        case SDL_MOUSEBUTTONUP:
        {
            State->Color.R = 20;
            State->Color.G = 255;
            State->Color.B = 255;
        } break;
        case SDL_QUIT:
        {
            State->Running = 0;
        } break;
        }
    }

    return EventNeedsRenderUpdate;
}

static void RenderChar(SDL_Renderer *Renderer, gui_font_render_data FontRender,
                       gui_state *State, SDL_Rect DEBUG_Rect, u32 KeyCode)
{
    u32 KeyCodeIndex = KeyCode - MIN_KEY_CODE;
    SDL_Texture *Texture = TEXTURE_CACHE[KeyCodeIndex];
    gui_char_data CharData = CHAR_DATA_CACHE[KeyCodeIndex];

    DEBUG_Rect.x = State->Cursor.X + FontRender.Scale * CharData.X0;
    DEBUG_Rect.y = State->Cursor.Y + FontRender.Scale * CharData.Y0;
    DEBUG_Rect.w = (State->Cursor.X + FontRender.Scale * CharData.X1) - DEBUG_Rect.x;
    DEBUG_Rect.h = (State->Cursor.Y + FontRender.Scale * CharData.Y1) - DEBUG_Rect.y;

    stbtt_GetGlyphHMetrics(&FontRender.Info, KeyCodeIndex,
                           &FontRender.AdvanceWidth, &FontRender.LeftSideBearing);
    s32 KernAdvance = 0;
    char NextChar = DIALOG[State->Dialog.Index][State->Dialog.CharIndex + 1];
    if(NextChar != 0)
    {
        KernAdvance = stbtt_GetCodepointKernAdvance(&FontRender.Info, KeyCodeIndex, NextChar);
    }
    s32 NextX = State->Cursor.X + KernAdvance + FontRender.Scale * FontRender.AdvanceWidth;

    if(NextX > SCREEN_WIDTH)
    {
        State->Cursor.X = 0;
        State->Cursor.Y += 28;
    }
    else
    {
        State->Cursor.X = NextX;
    }

    DEBUG_Rect.x = State->Cursor.X + CharData.XOffset;
    DEBUG_Rect.y = State->Cursor.Y + CharData.YOffset;
    SDL_SetTextureColorMod(Texture, State->Color.R, State->Color.G, State->Color.B);
    SDL_RenderCopy(Renderer, Texture, NULL, &DEBUG_Rect);
}

static void RenderDialog(SDL_Renderer *Renderer, gui_font_render_data FontRender,
                         gui_state *State, SDL_Rect DEBUG_Rect)
{
    u32 I;
    for (I = 0; I < State->Dialog.CharIndex; I++)
    {
        char Char = DIALOG[State->Dialog.Index][I];
        RenderChar(Renderer, FontRender, State, DEBUG_Rect, Char);
    }
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

    SDL_Rect DEBUG_Rect = CreateRect(0, 0, 0, 0);
    SDL_Window *Window = SDL_CreateWindow("idi", 0, 0,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_HIDDEN);
    assert(Window);
    SDL_Renderer *Renderer = SDL_CreateRenderer(Window, -1, 0);
    assert(Renderer);

    gui_state State = InitGuiState();

    gui_font_render_data FontRender;
    FontRender.PixelHeight = 26;
    FontRender.Info = InitTextureCache(Renderer, &FontRender.Scale, FontRender.PixelHeight);
    stbtt_GetFontBoundingBox(&FontRender.Info,
                             &FontRender.BoundingRect.X0, &FontRender.BoundingRect.Y0,
                             &FontRender.BoundingRect.X1, &FontRender.BoundingRect.Y1);
    State.Cursor.Y = FontRender.Scale - FontRender.BoundingRect.Y0;
    State.Cursor.X = 0;

    u32 DelayInMilliseconds = 16;

    SDL_ShowWindow(Window);

    while(State.Running)
    {
        b32 EventNeedsRenderUpdate = HandleEvents(&State);

        {
            State.Cursor.X = 0;
            State.Cursor.Y = 32;
        }

        if(State.Dialog.Writing || EventNeedsRenderUpdate)
        {
            SDL_RenderClear(Renderer);

            char Char = DIALOG[State.Dialog.Index][State.Dialog.CharIndex];
            if(Char != 0)
            {
                ++State.Dialog.CharIndex;
            }
            else
            {
                State.Dialog.Writing = 0;
            }

            RenderDialog(Renderer, FontRender, &State, DEBUG_Rect);
#if 0
            {
                u32 I;
                for(I = 0; I < State.Cursor.BufferIndex; ++I)
                {
                    u32 KeyCodeIndex = KEY_CODE_CACHE[I];
                    RenderChar(Renderer, Dialog, FontRender, &State, DEBUG_Rect, KeyCodeIndex);
                }
            }
#endif

            SDL_RenderPresent(Renderer);
        }
        SDL_Delay(DelayInMilliseconds);
    }

    DeInit(Window, Renderer);
}
