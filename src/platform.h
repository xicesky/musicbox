/***************************************************************************************************
    Our "platform" library.
    Currently uses SDL for everything.
***************************************************************************************************/

#ifndef PLATFORM_H
#define PLATFORM_H

// XXX Isn't there something more modern?
#include <inttypes.h>

typedef int16_t SAMPLE;
typedef int32_t PIXEL;

namespace MusicBox {

class Platform_Private;

/***************************************************************************************************/

class AudioBuffer {
public:
    SAMPLE *raw;
    int length;

    AudioBuffer();
    AudioBuffer(SAMPLE *raw, int length);
    virtual ~AudioBuffer();

    void clear();
};

/***************************************************************************************************/

class AudioRenderer {
public:
    AudioRenderer();
    virtual ~AudioRenderer();

    virtual void render(AudioBuffer& buf);
};

/***************************************************************************************************/

class VideoBuffer {
public:
    // Layout is always A8,R8,G8,B8
    PIXEL *raw;

    int length;
    int width;
    int height;

    VideoBuffer();
    VideoBuffer(PIXEL *raw, int width, int height);
    virtual ~VideoBuffer();

    void clear();
    void clear(PIXEL color);
    void set(int x, int y, PIXEL color);
};

/***************************************************************************************************/

class VideoRenderer {
public:
    VideoRenderer();
    virtual ~VideoRenderer();
    
    virtual void render(VideoBuffer& buf);
};

/***************************************************************************************************/

class Platform {

protected:
    Platform_Private* p;

public:
    Platform();
    virtual ~Platform();

    int getAudioSamplerate();
    
    void setAudioRenderer(AudioRenderer& renderer);
    void setVideoRenderer(VideoRenderer& renderer);

    void run();

};

/***************************************************************************************************/

}

#endif
