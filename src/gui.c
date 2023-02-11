#define SCREEN_WIDTH 880
#define SCREEN_HEIGHT 400

#define MAX_KEY_CODE_CACHE 1 << 12
u32 KEY_CODE_CACHE[MAX_KEY_CODE_CACHE];

#define MIN_KEY_CODE 20
#define MAX_KEY_CODE 128

#define MAX_TEXTURE_CACHE_COUNT 256
bounded_texture TEXTURE_CACHE[MAX_TEXTURE_CACHE_COUNT];
gui_char_data CHAR_DATA_CACHE[MAX_TEXTURE_CACHE_COUNT];

char ttf_buffer[1<<25];

#define KeyCodeIsAlpha(Code) ((Code) >= SDLK_a && (Code) <= SDLK_z)
#define KeyModShift(Mod) (((Mod) & (KMOD_LSHIFT | KMOD_RSHIFT)) ? 1 : 0)
#define KeyModCaps(Mod) (((Mod) & KMOD_CAPS) ? 1 : 0)

char *FONT_PATHS[] = {
    "../src/PTMono-Regular.ttf",
    "../src/ZapfDingbats.ttf",
    "../src/Bodoni Ornaments.ttf",
    "../src/Monaco.ttf",
    "../src/Arial Black.ttf",
};

#define FONT_PATH_INDEX 0
#define FONT_PATH (FONT_PATHS[FONT_PATH_INDEX])
#define FONT_HEIGHT_IN_PIXELS 24

static result Init()
{
    printf("Init\n");
    result Result = result_Ok;

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        Result = result_Error;
        PrintError("SDL_Init error");
    }

    return Result;
}

static void DeInit(SDL_Window *Window, SDL_Renderer *Renderer)
{
    printf("DeInit\n");
    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    SDL_Quit();
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

    return Result;
}

static void InitTextureCache(SDL_Renderer *Renderer)
{
    stbtt_fontinfo font;
    unsigned char *bitmap;
    int w,h,x,y,c;
    u8 *PixelData;
    s32 Pitch;
    s32 baseline, ascent;
    int ix0, iy0, ix1, iy1;
    f32 scale;
    fread(ttf_buffer, 1, 1<<25, fopen(FONT_PATH, "rb"));

    stbtt_InitFont(&font, (const unsigned char *)ttf_buffer, stbtt_GetFontOffsetForIndex((const unsigned char *)ttf_buffer,0));
    scale = stbtt_ScaleForPixelHeight(&font, FONT_HEIGHT_IN_PIXELS);
    stbtt_GetFontVMetrics(&font, &ascent,0,0);
    baseline = (int) (ascent*scale);

    for(c = 0; c < MAX_TEXTURE_CACHE_COUNT; ++c)
    {
        stbtt_GetCodepointBitmapBox(&font, c, scale, scale, &ix0, &iy0, &ix1, &iy1);
        bitmap = stbtt_GetCodepointBitmap(&font, 0, scale, c, &w, &h, 0,0);

        SDL_Texture *Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888,
                                                 SDL_TEXTUREACCESS_STREAMING, w, h);

        SDL_LockTexture(Texture, 0, (void **)&PixelData, &Pitch);
        for (y=0; y < h; ++y)
        {
            for (x=0; x < w; ++x)
            {
                PixelData[4*x + Pitch*y + 0] = bitmap[y*w+x];
                PixelData[4*x + Pitch*y + 1] = bitmap[y*w+x];
                PixelData[4*x + Pitch*y + 2] = bitmap[y*w+x];
                PixelData[4*x + Pitch*y + 3] = bitmap[y*w+x];
            }
        }
        SDL_UnlockTexture(Texture);
        SDL_UpdateTexture(Texture, 0, PixelData, Pitch);
        TEXTURE_CACHE[c].Rect.x = ix0;
        TEXTURE_CACHE[c].Rect.y = iy0;
        TEXTURE_CACHE[c].Rect.w = w;
        TEXTURE_CACHE[c].Rect.h = h;
        TEXTURE_CACHE[c].Texture = Texture;
    }
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

static void RenderTextLine(SDL_Renderer *Renderer, char *Text, s32 Begin, s32 End, s32 X, s32 Y)
{
    s32 I;
    SDL_Rect DestRect, TextureRect;
    s32 CurrentX = X;
    for(I = Begin; I < End; ++I)
    {
        bounded_texture BoundedTexture = TEXTURE_CACHE[(u8)Text[I]];
        TextureRect.x = 0;
        TextureRect.y = 0;
        TextureRect.w = BoundedTexture.Rect.w;
        TextureRect.h = BoundedTexture.Rect.h;
        DestRect.x = BoundedTexture.Rect.x + CurrentX;
        DestRect.y = Y + BoundedTexture.Rect.y;
        DestRect.w = BoundedTexture.Rect.w;
        DestRect.h = BoundedTexture.Rect.h;
        SDL_RenderCopy(Renderer, BoundedTexture.Texture, &TextureRect, &DestRect);

        CurrentX += DestRect.w;
    }
    SDL_SetRenderDrawColor(Renderer, 255, 10, 20, 100);
    SDL_RenderDrawLine(Renderer, 0, Y, SCREEN_WIDTH, Y);
}

/* static SDL_Color SDLColor(u8 R, u8 G, u8 B, u8 A) */
/* { */
/*     SDL_Color Result; */
/*     Result.r = R; */
/*     Result.g = G; */
/*     Result.b = B; */
/*     Result.a = A; */
/*     return Result; */
/* } */
void DisplayWindow()
{
    printf("DisplayWindow\n");
    result InitResult = Init();

    if(InitResult == result_Error)
    {
        PrintError("Init error");
        return;
    }

    SDL_Window *Window = SDL_CreateWindow("idi", 0, 0,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_HIDDEN);
    assert(Window);
    SDL_Renderer *Renderer = SDL_CreateRenderer(Window, -1, 0);
    assert(Renderer);
    gui_state State = InitGuiState();
    InitTextureCache(Renderer);

    SDL_Rect Rect;
    Rect.x = 10;
    Rect.y = 10;
    Rect.w = 24;
    Rect.h = 32;

    gui_font_render_data FontRender;
    FontRender.PixelHeight = 26;
    State.Cursor.Y = FontRender.Scale - FontRender.BoundingRect.Y0;
    State.Cursor.X = 0;

    u32 DelayInMilliseconds = 16;

    SDL_ShowWindow(Window);

    while(State.Running)
    {
        b32 EventNeedsRenderUpdate = HandleEvents(&State);

        if(1 || EventNeedsRenderUpdate)
        {
            SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
            SDL_RenderClear(Renderer);
            /* SDL_RenderCopy(Renderer, Texture, 0, 0); */
            RenderTextLine(Renderer, "foo", 0, 3, 50, 50);

            /* Rect.w = TEXTURE_CACHE['y'].Width; */
            /* Rect.h = TEXTURE_CACHE['y'].Height; */
            /* SDL_RenderCopy(Renderer, TEXTURE_CACHE['y'].Texture, 0, &Rect); */
            SDL_RenderPresent(Renderer);
        }
        SDL_Delay(DelayInMilliseconds);
    }

    DeInit(Window, Renderer);
}
