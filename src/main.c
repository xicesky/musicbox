
#include <stdio.h>
//#include <stdlib.h>
#include <windows.h>

#include "tables.h"
#include "audioio.h"

#include "voices.h"
#include "song.c"

//#define DEBUG

//int melodyIndex = 0;
int songIndex = 0;
int patternIndex = 0;

int max = 0;

int samplesPerBeat;

int beatCounter = 0;
int vt_beat_i = 0;
int initialWait = SAMPLE_RATE >> 2;

void voiceNote(Voice *v, Instrument *i, int note) {
#ifdef DEBUG
    printf("voiceNote(?,?,%d)\n", note);
#endif
    int modRate;
    if (note > -99) {
        printf("%3d  ", note);
        note = TABLES_rateForNote(note, SAMPLE_RATE);
        modRate = TABLES_calcRate((double)(i->mod_freq) / 1024.0, SAMPLE_RATE);
        startNote(v, i, note, 1024, modRate);
    } else
        printf("     ");
}

void loadPatterns() {
    int i;
    // Load patterns
    //printf("\n");
    for (i = 0; i < VOICES; i++) {
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

int main(int argc, char *argv[])
{
    int i;
    TABLES_init();
    //TABLES_test();

    for (i = 0; i < VOICES; i++)
        voice[i].instr = 0;
    loadPatterns();

    samplesPerBeat = SAMPLE_RATE / (TEMPO / 60);
    printf("Samples per beat: %d\n", samplesPerBeat);

    if (!AUDIOIO_init(SAMPLE_RATE, produceSample))
        exit(1);
/*
    AUDIOIO_setAudioActive(1);

    MessageBox(0, "You should hear sound now.\r\nPress ok to exit.\r\n", "SDL Audio test", MB_OK);
    //SDL_Delay(5000);

    AUDIOIO_setAudioActive(0);
*/
    AUDIOIO_mainLoop();

//    printf("Samplecounter: %d", x);
    printf("Max Amplitude was: %d\n", max);

    AUDIOIO_exit();

    return 0;
}
