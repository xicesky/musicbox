
#include <math.h>
#include <stdlib.h>
#include <SDL.h>

int AUDIOIO_audioActive = 0;

//void (*callback)(void *userdata, Uint8 *stream, int len);
double (*AUDIOIO_produceSampleCallback)();

SDL_Surface* screen;

void AUDIOIO_callback(void *userdata, Uint8 *stream, int len) {
    int nSamples = len/4;
    short int *smpData = (short int *)stream;
    int i;
    int clip = 0;

    for (i = 0; i < nSamples; i += 1) {
        double x = AUDIOIO_produceSampleCallback();
        double a = fabs(x);
        if (a > 1.0) {
            x /= a; // Clipping
            clip++;
        }
        x = x * 32766;
        smpData[(i<<1) + 0] = x; // Left
        smpData[(i<<1) + 1] = x; // Right

    }
    if (clip > 0)
        printf("%d samples clipped\n", clip);
}

int AUDIOIO_init(int sampleRate, double (*produceSampleCallback)()) {
    AUDIOIO_produceSampleCallback = produceSampleCallback;

    if ( SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0 ) { //|SDL_INIT_VIDEO
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        return 0;
    }

    SDL_AudioSpec fmt;

    fmt.freq = sampleRate;  // Sample rate
    fmt.format = AUDIO_S16; // 16 bit singed
    fmt.channels = 2;       // Stereo
    fmt.samples = 512;      // Buffer size
    fmt.callback = AUDIOIO_callback;
    fmt.userdata = NULL;

    /* Open the audio device */
    if ( SDL_OpenAudio(&fmt, NULL) < 0 ) {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        return 0;
    }

    screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE); // | SDL_NOFRAME
    if ( screen == NULL ) {
        fprintf(stderr, "Unable to open video: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

void AUDIOIO_setAudioActive(int audioActive) {
    if (audioActive && !AUDIOIO_audioActive)
        SDL_PauseAudio(0);
    else if (!audioActive && AUDIOIO_audioActive)
        SDL_PauseAudio(1);
    AUDIOIO_audioActive = audioActive;
}

void printEvent(SDL_Event* event) {
    Uint8 type = event->type;
    switch (type) {
        case SDL_ACTIVEEVENT: printf("EVENT: SDL_ACTIVEEVENT\n"); break;
        case SDL_KEYDOWN: printf("EVENT: SDL_KEYDOWN\n"); break;
        case SDL_KEYUP: printf("EVENT: SDL_KEYUP\n"); break;
        case SDL_MOUSEMOTION: printf("EVENT: SDL_MOUSEMOTION\n"); break;
        case SDL_MOUSEBUTTONDOWN: printf("EVENT: SDL_MOUSEBUTTONDOWN\n"); break;
        case SDL_MOUSEBUTTONUP: printf("EVENT: SDL_MOUSEBUTTONUP\n"); break;

        case SDL_JOYAXISMOTION: printf("EVENT: SDL_JOYAXISMOTION\n"); break;
        case SDL_JOYBALLMOTION: printf("EVENT: SDL_JOYBALLMOTION\n"); break;
        case SDL_JOYHATMOTION: printf("EVENT: SDL_JOYHATMOTION\n"); break;
        case SDL_JOYBUTTONDOWN: printf("EVENT: SDL_JOYBUTTONDOWN\n"); break;
        case SDL_JOYBUTTONUP: printf("EVENT: SDL_JOYBUTTONUP\n"); break;

        case SDL_VIDEORESIZE: printf("EVENT: SDL_VIDEORESIZE\n"); break;
        case SDL_VIDEOEXPOSE: printf("EVENT: SDL_VIDEOEXPOSE\n"); break;

        case SDL_QUIT: printf("EVENT: SDL_QUIT\n"); break;
        case SDL_USEREVENT: printf("EVENT: SDL_USEREVENT\n"); break;
        case SDL_SYSWMEVENT: printf("EVENT: SDL_SYSWMEVENT\n"); break;
        default:
            printf("EVENT: %d\n", (int)type);
            break;
    }
}

int AUDIOIO_running = 0;

void AUDIOIO_mainLoop() {
    SDL_Event event;

    AUDIOIO_setAudioActive(1);
    AUDIOIO_running = 1;

    while (AUDIOIO_running) {
        if (!SDL_WaitEvent(&event)) {
            fprintf(stderr, "Failed to wait for event");
            break;
        }

        switch (event.type) {
            case SDL_QUIT:
                AUDIOIO_running = 0;
                break;
            case SDL_KEYDOWN:
                // ignore
                break;
            case SDL_KEYUP:
                printf("mod = %d, sym = %d\n", (int)event.key.keysym.mod, (int)event.key.keysym.sym);
                if (((event.key.keysym.mod & 0xFFF) == KMOD_NONE) && (event.key.keysym.sym == SDLK_ESCAPE))
                    AUDIOIO_running = 0;
                break;
            case SDL_MOUSEMOTION:
                // ignore
                break;
            default:
                printEvent(&event);
        }
    }

    AUDIOIO_running = 0;
    AUDIOIO_setAudioActive(0);
}

void AUDIOIO_exit() {
    SDL_CloseAudio();
    SDL_Quit();
}
