/***************************************************************************************************
    Our "platform" library.
    Currently uses SDL for everything.
***************************************************************************************************/

#include "platform.h"

#include <exception>
#include <sstream>
#include <iostream>

#include <string.h>     // memset

#include <SDL2/SDL.h>
#include "sdlcleanup.h"

namespace MusicBox {

/***************************************************************************************************
    SDL error handling using exceptions
***************************************************************************************************/
class SDLException : public std::exception {

protected:
    std::string m_message;

public:
    SDLException(std::string message) :
        m_message(message)
    {
    }

    virtual const char* what() const throw() {
        return m_message.c_str();
    }
};

SDLException sdl_error(const std::string &fname) {
    std::ostringstream msgs;
    //msgs << "[ERROR] " << fname << ": " SDL_GetError();
    msgs << fname << "(): " << SDL_GetError();
    return SDLException(msgs.str());
}

/***************************************************************************************************
    Private fields
***************************************************************************************************/
/* We don't want this stuff to show up in the header, so we put it in a private class here */

class Platform_Private {
public:

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;

    // TODO
    //Uint32 * pixels = new Uint32[640 * 480];

    bool running;
    Uint32* pixels;

    Platform_Private() {
        pixels = new Uint32[640 * 480];
        memset(pixels, 255, 640 * 480 * sizeof(Uint32)); 
    }

    virtual ~Platform_Private() {
        delete pixels;
    }

    void init_everything() {
        init_window();
        init_renderer();
        init_texture();
    }

    void cleanup_everything() {
        sdlcleanup(texture);
        sdlcleanup(renderer);
        sdlcleanup(window);
    }

    void init_window() {

        if (window)
            return;
        
        // http://wiki.libsdl.org/SDL_CreateWindow
        window = SDL_CreateWindow(
            "Musicbox",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            640, 480,
            SDL_WINDOW_SHOWN
            );

        if (window == nullptr)
            throw sdl_error("SDL_CreateWindow");
    }

    void init_renderer() {

        if (renderer)
            return;

        // http://wiki.libsdl.org/SDL_CreateRenderer
        renderer = SDL_CreateRenderer(window, -1, 0);

        if (renderer == nullptr)
            throw sdl_error("SDL_CreateRenderer");
    }

    void init_texture() {

        if (texture)
            return;

        // http://wiki.libsdl.org/SDL_CreateTexture
        texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STATIC,
            640, 480
            );

        if (texture == nullptr)
            throw sdl_error("SDL_CreateTexture");
    }

    void render_pixels() {

    }

    void render_frame() {
        render_pixels();

        // http://wiki.libsdl.org/SDL_UpdateTexture
        SDL_UpdateTexture(texture, NULL, pixels, 640 * sizeof(Uint32));

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);    
        // SDL_Delay(10);  // Bremse rein... why?
    }

    bool mdown = false;

    void handle_events() {
        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {

        case SDL_QUIT:
            running = false;
            break;

        case SDL_KEYUP:
            // std::cout << "    key: " << event.key.keysym.sym << std::endl;
            if (event.key.keysym.sym == SDLK_ESCAPE)
                running = false;
            break;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
                mdown = false;
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
                mdown = true;
            break;

        case SDL_MOUSEMOTION:
            if (mdown) {
                int x = event.motion.x;
                int y = event.motion.y;

                // Sanity pls.
                if (x < 0) break;
                if (x >= 640) break;
                if (y < 0) break;
                if (y >= 480) break;

                std::cout << "    mouse: " << x << " " << y << std::endl;
                pixels[y * 640 + x] = 0;
            }
            break;
        }

    }
};

/***************************************************************************************************
    Main implementation
***************************************************************************************************/

Platform::Platform() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        throw sdl_error("SDL_Init");
    p = new Platform_Private();
}

Platform::~Platform() {
    p->cleanup_everything();
    SDL_Quit();
    delete p;
}

void Platform::run() {
    p->init_everything();
    p->running = true;

    while (p->running) {
        p->handle_events();
        p->render_frame();
    }

    p->cleanup_everything();
}

}
