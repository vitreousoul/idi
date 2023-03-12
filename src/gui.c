#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720

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

static b32 InitGL()
{
    b32 success = 1;
    GLenum error = GL_NO_ERROR;
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error initializing OpenGL!\n");
        success = 0;
    }
    error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error initializing OpenGL!\n");
        success = 0;
    }
    return success;
}

static void DeInit(SDL_Window *Window)
{
    printf("DeInit\n");
    SDL_DestroyWindow(Window);
    SDL_Quit();
}

static gui_state InitGuiState()
{
    gui_state Result;
    Result.Running = 1;
    Result.FontData.Size = 36;
    Result.Cursor.BufferIndex = 0;
    Result.Cursor.X = 0;
    Result.Cursor.Y = 24;
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
    printf("baseline %d", baseline);

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
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    assert(Window);
    SDL_GLContext GLContext = SDL_GL_CreateContext(Window);
    if(GLContext == 0)
    {
        printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
        return;
    }
    if( SDL_GL_SetSwapInterval( 1 ) < 0 )
    {
        printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
    }
    InitGL();
    gui_state State = InitGuiState();
    s32 TexWidth, TexHeight, TexBitWidth;
    u8 *ImageData = stbi_load("../assets/images/pigeon.png", &TexWidth, &TexHeight, &TexBitWidth, 0);
    printf("%d %d %d\n", TexWidth, TexHeight, TexBitWidth);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLuint TextureHandle = 0;
    glGenTextures(1, &TextureHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA);
    glEnable(GL_BLEND);
    /* glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TexWidth, TexHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, ImageData); */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TexWidth, TexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, ImageData);
    u32 DelayInMilliseconds = 16;

    SDL_ShowWindow(Window);

    while(State.Running)
    {
        b32 EventNeedsRenderUpdate = HandleEvents(&State);

        if(1 || EventNeedsRenderUpdate)
        {
            glClearColor(0.5, 0.6, 0.6, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            glEnable(GL_TEXTURE_2D);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBegin(GL_TRIANGLES);
            glBindTexture(GL_TEXTURE_2D, TextureHandle);
            f32 P = 0.14;
            // lower triangle
            glTexCoord2f(0.0f, 0.0f); glVertex2f(-P, P);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(P, P);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(P, -P);
            // upper triangle
            glTexCoord2f(0.0f, 0.0f); glVertex2f(-P, P);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(P, -P);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(-P, -P);
            glEnd();
            glFlush();
            glDisable(GL_TEXTURE_2D);
            SDL_GL_SwapWindow(Window);
        }
        SDL_Delay(DelayInMilliseconds); // TODO: figure out when to sleep, only when non-vsync?
    }
    free(ImageData);
    DeInit(Window);
}
