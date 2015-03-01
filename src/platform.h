/***************************************************************************************************
    Our "platform" library.
    Currently uses SDL for everything.
***************************************************************************************************/

#ifndef PLATFORM_H
#define PLATFORM_H

typedef short int SAMPLE;

namespace MusicBox {

class Platform_Private;


class AudioBuffer {
public:
    SAMPLE *raw;
    int length;

    void clear();
};

class AudioRenderer {
public:
    virtual void render(AudioBuffer& buf);
};

class Platform {

protected:
    Platform_Private* p;

public:
    Platform();
    virtual ~Platform();

    int getAudioSamplerate();
    void setAudioRenderer(AudioRenderer& renderer);
    void run();

};

}

#endif
