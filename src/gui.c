#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

void DisplayWindow()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        PrintError("SDL_Init error");
    }

    /*
    if (TTF_Init() < 0)
    {
        PrintError("TTF_Init error");
    }
    */

    SDL_Window *Window = SDL_CreateWindow("idi", 0, 0,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_HIDDEN);
    /* SDL_Surface *Surface = SDL_GetWindowSurface(Window); */
    SDL_ShowWindow(Window);

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
    /* TTF_Quit(); */
    SDL_Quit();
}
