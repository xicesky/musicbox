
#ifndef VOICES_H
#define VOICES_H

#define SIGNAL_ZERO     0
#define SIGNAL_SIN      1
#define SIGNAL_RECT     2
#define SIGNAL_SAW      3
#define SIGNAL_NOISE    4

typedef struct Instrument {
    int attack, decay;  // samples
    int signal;         // see SIGNAL_* constants
    int volume;         // 0..1024
    int mod_signal;     // see SIGNAL_* constants (modulation)
    int mod_freq;       // frequency of modulation, 1024 = 1Hz
    int mod_amount;     // amount of modulation 0..1024
} Instrument;

typedef struct Voice {
    int* pattern; // Current pattern

    int stage; // 0 = Attack, 1 = Decay
    int stage_ctr; // Current samples into stage
    int stage_total; // Total samples into stage
    int rate; // Rate of the note (for TABLES)
    int x; // Current TABLE position
    int volume; // 0..1024
    int modRate; // Rate of modulation
    int modScale; // Amount of modulation 0..1024
    int modX; // Current TABLE position (modulation)
    Instrument* instr;
} Voice;

typedef struct VTable {
    int  len;
    int* data;
} VTable;

int vTableGet(VTable *tbl, int *index);
void startNote(Voice *v, Instrument *i, int rate, int volume, int modRate);
int sampleVoice(Voice* v);
int sampleVoices(Voice* voice, int count);

#endif
