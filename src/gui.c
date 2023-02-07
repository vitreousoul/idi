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
                                          SDL_WINDOW_HIDDEN);
    assert(Window);
    SDL_Renderer *Renderer = SDL_CreateRenderer(Window, -1, 0);
    assert(Renderer);

    gui_state State = InitGuiState();

    gui_font_render_data FontRender;
    FontRender.PixelHeight = 26;
    State.Cursor.Y = FontRender.Scale - FontRender.BoundingRect.Y0;
    State.Cursor.X = 0;

    u32 DelayInMilliseconds = 16;

    SDL_ShowWindow(Window);

    while(State.Running)
    {
        b32 EventNeedsRenderUpdate = HandleEvents(&State);

        if(EventNeedsRenderUpdate)
        {
            SDL_RenderClear(Renderer);

            SDL_RenderPresent(Renderer);
        }
        SDL_Delay(DelayInMilliseconds);
    }

    DeInit(Window, Renderer);
}
