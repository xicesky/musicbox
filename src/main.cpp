/***************************************************************************************************
    Main code
    GOGOGO
***************************************************************************************************/

#include <exception>
#include <iostream>

#include "tables.h"
#include "platform.h"
#include "voices.h"

// TODO: Get rid
#include <stdio.h>
#include <math.h>


using namespace MusicBox;

/***************************************************************************************************
    This is the song
    Have fun with manual pattern editing!
    Hardcode ftw.
***************************************************************************************************/

#define ___ -99

// Beats per minute
#define TEMPO           (160 * 4)
#define VOICES          4

// VTables: Beat (for shuffling)
int vtbtbl[4] = { -400, -200, 200, 400 };
VTable vt_beat = {
    4,
    vtbtbl,
};

Instrument instr[VOICES] = {
    // Attack, Decay, Signal, Volume, ModSingal, ModFreq, ModAmount
    { 410, 17640, SIGNAL_SAW, 256, SIGNAL_SIN, 10240, 128 },//    { 410, 8820, SIGNAL_SAW, 256 },
    { 410, 22000, SIGNAL_RECT, 320, SIGNAL_ZERO, 1024, 1024 },
    { 41, 2210, SIGNAL_NOISE, 512, SIGNAL_ZERO, 0, 0 },
    { 410, 600, SIGNAL_RECT, 1024, SIGNAL_ZERO, 0, 0 }
};
Voice voice[VOICES];

#define PATTERN_LEN 16
#define NPATTERNS 10

int pattern[NPATTERNS][PATTERN_LEN] = {
{ // A moll arp. [0]
      0,   3,   7,  12,
      3,   7,  12,  15,
      7,  12,  15,  19,
     12,  15,  19,  24,
},
{ // C dur arp. [1]
      3,   7,  10,  15,
      7,  10,  15,  19,
     10,  15,  19,  22,
     15,  19,  22,  27
},
{ // A moll bass [2]
    -12, ___, ___, ___,
    -12, ___, -24, ___,
    -12, ___, ___, ___,
    -12, ___, -24, ___,
},
{ // C dur bass [3]
     -9, ___, ___, ___,
     -9, ___, -21, ___,
     -9, ___, ___, ___,
     -9, ___, -21, ___
},
{ // Really cheap snare [4]
    ___, ___,   0, ___,
    ___, ___,   0, ___,
    ___, ___,   0, ___,
    ___, ___,   0, ___,
},
{ // Really cheap b.d. [5]
    -24, ___, ___, ___,
    -24, ___, ___, ___,
    -24, ___, ___, ___,
    -24, ___, ___, ___,
},
{ // F dur arp. [6] c -7
     -4,   0,   3,   8,
      0,   3,   8,  12,
      3,   8,  12,  15,
      8,  12,  15,  20
},
{ // G dur arp. [7] f +2
     -2,   2,   5,  10,
      2,   5,  10,  14,
      5,  10,  14,  17,
     10,  14,  17,  22
},
{ // Unused pattern [8]
    ___, ___, ___, ___,
    ___, ___, ___, ___,
    ___, ___, ___, ___,
    ___, ___, ___, ___,
},
{ // Unused pattern [9]
    ___, ___, ___, ___,
    ___, ___, ___, ___,
    ___, ___, ___, ___,
    ___, ___, ___, ___,
},
};

#define SONG_LEN 8
int song[SONG_LEN][VOICES] = {
    { 0, 2, 4, 5 },
    { 1, 3, 4, 5 },
    { 0, 2, 4, 5 },
    { 1, 3, 4, 5 },
    { 0, 2, 4, 5 },
    { 1, 3, 4, 5 },
    { 6, 2, 4, 5 },
    { 7, 3, 4, 5 },
};

/***************************************************************************************************
    Main music generator (pattern, song loop...)
***************************************************************************************************/

int sample_rate;

int songIndex = 0;
int patternIndex = 0;

int max = 0;

int samplesPerBeat;

int beatCounter = 0;
int vt_beat_i = 0;
int initialWait;

void voiceNote(Voice *v, Instrument *i, int note) {
#ifdef DEBUG
    printf("voiceNote(?,?,%d)\n", note);
#endif
    int modRate;
    if (note > -99) {
        printf("%3d  ", note);
        note = TABLES_rateForNote(note, sample_rate);
        modRate = TABLES_calcRate((double)(i->mod_freq) / 1024.0, sample_rate);
        startNote(v, i, note, 1024, modRate);
    } else
        printf("     ");
}

void loadPatterns() {
    // Load patterns
    //printf("\n");
    for (int i = 0; i < VOICES; i++) {
        //printf("Voice %d pattern = %d\n", i, song[songIndex][i]);
        voice[i].pattern = pattern[song[songIndex][i]];
    }
}

void produceBeat() {
    int i;

    printf("Notes: ");
    for (i = 0; i < VOICES; i++) {
#ifdef DEBUG
        printf("produceBeat(): voice=%d, patternIndex=%d\n", i, patternIndex);
#endif
        voiceNote(&voice[i], &instr[i], voice[i].pattern[patternIndex]);
    }
    //printf(" (beat: %d)\n", vt_beat_i);
    printf(" (p: %2d, %2d)\n", patternIndex, songIndex);

    patternIndex++;
    if (patternIndex >= PATTERN_LEN) {
        patternIndex = 0;
        songIndex++;
        if (songIndex >= SONG_LEN)
            songIndex = 0;

        loadPatterns();
    }
    //melodyIndex = (melodyIndex + 1) % MELODY_LEN;
}

double produceSample() {
    int y = 0;

    if (initialWait > 0) {
        initialWait--;
        return 0;
    }

    beatCounter--;
    if (beatCounter <= 0) {
        // Beat occured
        beatCounter = samplesPerBeat + vTableGet(&vt_beat, &vt_beat_i);
//        printf(".\n");
        produceBeat();
    }

    y = sampleVoices(voice, VOICES);

    if (y > max) max = y;
    if (-y > max) max = -y;

    return ((double)y) / 32768.0;
}

class MyAudioRenderer : public AudioRenderer {

    virtual void render(AudioBuffer& buf) {
        int nSamples = buf.length;
        short int *smpData = buf.raw;
        int i;
        int clip = 0;

        for (i = 0; i < nSamples; i += 1) {
            double x = produceSample();
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
};

int main(int argc, char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);

    std::cout << "Welcome." << std::endl;

    TABLES_init();
    // TABLES_test();

    // Initialize voices
    for (int i = 0; i < VOICES; i++)
        voice[i].instr = 0;
    loadPatterns();

    try {

        Platform platform;
        sample_rate = platform.getAudioSamplerate();
        initialWait = sample_rate >> 2;

        samplesPerBeat = sample_rate / (TEMPO / 60);
        printf("Samples per beat: %d\n", samplesPerBeat);

        // Set up our audio renderer
        MyAudioRenderer audio_renderer;
        platform.setAudioRenderer(audio_renderer);

        platform.run();
        printf("Max Amplitude was: %d\n", max);

    } catch (std::exception &e) {
        std::cout << e.what() << std::endl ;
        return 100;
    }

    std::cout << "Bye." << std::endl;

    return 0;

}
