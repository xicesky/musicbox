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
    Audio buffer
***************************************************************************************************/

AudioBuffer::AudioBuffer() 
    : AudioBuffer(nullptr, 0)
{
}

AudioBuffer::AudioBuffer(SAMPLE *xraw, int xlength)
{
    raw = xraw;
    length = xlength;
}

AudioBuffer::~AudioBuffer() {
}

void AudioBuffer::clear() {
    memset(raw, 0, length * sizeof(SAMPLE));
}

/***************************************************************************************************
    Audio renderer
***************************************************************************************************/

AudioRenderer::AudioRenderer() {}
AudioRenderer::~AudioRenderer() {}

// Default impl
void AudioRenderer::render(AudioBuffer& buf) {
    buf.clear();
}

/***************************************************************************************************
    SDL Audio IO
***************************************************************************************************/

// Have to declare callback ahead ...
void platform_audiocallback(void*  userdata, Uint8* stream, int len);

class SDLAudioIO {

public:
    SDL_AudioDeviceID audio_device;
    AudioBuffer audio_buffer;
    AudioRenderer *audio_renderer = nullptr;

    int sample_rate = -1;
    int channels = 2;

    SDLAudioIO() {
        SDL_AudioSpec want, have;
        SDL_zero(want);

        want.freq = 48000;
        want.format = AUDIO_S16;
        want.channels = 2;
        want.samples = 512;
        want.callback = &platform_audiocallback;
        want.userdata = this;

        audio_device = SDL_OpenAudioDevice(
            nullptr,    // Default device
            0,          // No capture (unsupported in SDL2 atm)
            &want,
            &have,
            0
            );

        if (audio_device == 0)
            throw sdl_error("SDL_OpenAudioDevice");

        if (have.format != AUDIO_S16)
            throw SDLException("Invalid audio format.");
        if (have.channels != 2)
            throw SDLException("Could not get stereo output.");

        sample_rate = have.freq;
        channels = have.channels;

        SDL_PauseAudioDevice(audio_device, 0);
    }

    virtual ~SDLAudioIO() {
        if (audio_device > 0)
            SDL_CloseAudioDevice(audio_device);
    }

    void audio_callback(Uint8* stream, int len) {
        audio_buffer.raw = (SAMPLE*)stream;
        audio_buffer.length = len / sizeof(SAMPLE) / channels;
        if (audio_renderer)
            audio_renderer->render(audio_buffer);
        else
            audio_buffer.clear();
    }

};

// C callback for SDL
void platform_audiocallback(void*  userdata, Uint8* stream, int len)
{
    if (userdata == nullptr)
        return;
    SDLAudioIO* p = (SDLAudioIO*)userdata;
    p->audio_callback(stream, len);
}

/***************************************************************************************************
    Video buffer
***************************************************************************************************/

VideoBuffer::VideoBuffer()
    : VideoBuffer(nullptr, 0, 0)
{
}

VideoBuffer::VideoBuffer(PIXEL *xraw, int xwidth, int xheight)
{
    raw = xraw;
    width = xwidth;
    height = xheight;
    length = width * height;
}

VideoBuffer::~VideoBuffer()
{
}

void VideoBuffer::clear() {
    memset(raw, 0, length * sizeof(PIXEL));
}

/***************************************************************************************************
    Video renderer
***************************************************************************************************/

VideoRenderer::VideoRenderer() {}
VideoRenderer::~VideoRenderer() {}

// Default impl
void VideoRenderer::render(VideoBuffer& buf) {
    buf.clear();
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
    SDLAudioIO *audio_io = nullptr;

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
        init_audio();
    }

    void cleanup_everything() {
        if (audio_io) {
            delete audio_io;
            audio_io = nullptr;
        }
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

    void init_audio() {
        if (audio_io)
            return;
        audio_io = new SDLAudioIO();
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

                // std::cout << "    mouse: " << x << " " << y << std::endl;
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
    p->init_everything();
}

Platform::~Platform() {
    p->cleanup_everything();
    SDL_Quit();
    delete p;
}

void Platform::run() {
    p->running = true;

    while (p->running) {
        p->handle_events();
        p->render_frame();
    }
}

int Platform::getAudioSamplerate() {
    if (!p->audio_io)
        throw SDLException("Audio not initialized.");
    return p->audio_io->sample_rate;
}

void Platform::setAudioRenderer(AudioRenderer& renderer) {
    if (!p->audio_io)
        throw SDLException("Audio not initialized.");
    p->audio_io->audio_renderer = &renderer;
}

}
