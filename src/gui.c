#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

char *FONT_PATH = "src/PTMono-Regular.ttf";

void DisplayWindow()
{
    int TTFResult;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        PrintError("SDL_Init error");
    }

    if((TTFResult = TTF_Init()) < 0)
    {
        PrintError("TTF_Init error");
    }

    SDL_Window *Window = SDL_CreateWindow("idi", 0, 0,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_HIDDEN);
    SDL_Renderer *Renderer = SDL_CreateRenderer(Window, -1, 0);
    TTF_Font *Font = TTF_OpenFont(FONT_PATH, 24);
    if(Font == 0) PrintError("TTF_OpenFont");
    SDL_Color FontColor = {220,200,0,255};
    SDL_Surface *Surface = TTF_RenderText_Blended(Font, "foo", FontColor);
    SDL_Texture *Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
    SDL_ShowWindow(Window);
    SDL_RenderCopy(Renderer, Texture, NULL, NULL);
    SDL_RenderPresent(Renderer);

    SDL_Event Event;
    u32 Running = 1;
    u32 DelayInMilliseconds = 32;

    while(Running)
    {
        while(SDL_PollEvent(&Event))
        {
            if(Event.type == SDL_QUIT)
            {
                Running = 0;
            }
        }

        SDL_Delay(DelayInMilliseconds);
    }

    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    TTF_Quit();
    SDL_DestroyTexture(Texture);
    SDL_FreeSurface(Surface);
    SDL_Quit();
}
