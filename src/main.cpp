
#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
//#include <windows.h>
#include <SDL2/SDL.h>
#include "sdlcleanup.h"

class MyApp 
{

public:
    MyApp()
    {

    }

    int run()
    {
        std::cout << "Yay." << std::endl;
        SDL_Delay(3000);

        return 0;
    }

};

int main(int argc, char *argv[])
{

    // http://wiki.libsdl.org/SDL_Init
    int result = SDL_Init(SDL_INIT_EVERYTHING);

    if (result < 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 100;
    }

    // http://wiki.libsdl.org/SDL_CreateWindow
    SDL_Window *win = SDL_CreateWindow("Musicbox", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (win == nullptr) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 100;
    }

    // http://wiki.libsdl.org/SDL_CreateRenderer
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
    if (ren == nullptr) {
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 100;
    }

    // http://wiki.libsdl.org/SDL_CreateTexture
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 640, 480);
    if (tex == nullptr) {
        std::cout << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 100;
    }

    Uint32 * pixels = new Uint32[640 * 480];

    // MyApp myApp;
    // int ret = myApp.run();

    bool running = true;
    while (running) {
        SDL_RenderClear(ren);
        SDL_RenderPresent(ren);    
        SDL_Delay(10);

        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {

        case SDL_QUIT:
            running = false;
            break;

        case SDL_KEYUP:
            SDL_KeyboardEvent *kev = (SDL_KeyboardEvent*)( &event );
            // std::cout << "    key: " << kev->keysym.sym << std::endl;
            if (kev->keysym.sym == SDLK_ESCAPE)
                running = false;
            break;

        }
    }

    sdlcleanup(tex, ren, win);
    SDL_Quit();

    std::cout << "Bye." << std::endl;

    return 0;
    // return ret;
}
