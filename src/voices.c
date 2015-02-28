
#include "voices.h"
#include "tables.h"

int vTableGet(VTable *tbl, int *index) {
    int value = tbl->data[*index];
    (*index)++;
    if ((*index) >= tbl->len)
        (*index) = 0;
    return value;
}

void startNote(Voice *v, Instrument *i, int rate, int volume, int modRate) {
    v->stage = 0;
    v->stage_ctr = 0;
    v->stage_total = i->attack;
    v->rate = rate;
    v->x = 0;
    v->volume = (i->volume * volume) >> 10;

    v->modRate = modRate;
    v->modScale = i->mod_amount;
    v->modX = 0;

    v->instr = i;
}

int getVolume(Voice *v) {
    int volume;

    if (v->instr == 0)
        return 0;

    // Move through ADSR (currently only Attack, Decay)
    v->stage_ctr++;
    if (v->stage_ctr >= v->stage_total) {
        // To decay stage (stage > 1 will be ignored)
        v->stage++;
        v->stage_ctr = 0;
        v->stage_total = v->instr->decay;
    }

    // Determine volume
    volume = (v->stage_ctr * 1024) / v->stage_total;
    switch (v->stage) {
        case 0: break;
        case 1: volume = 1024 - volume; break;
        default: volume = 0;
    }
    return (volume * v->volume) >> 10;
}

int getModulation(Voice *v) {
    int y = 0;

    if (v->instr == 0)
        return 0;

    switch (v->instr->mod_signal) {
        case SIGNAL_ZERO:   y = 0; break;
        case SIGNAL_SIN:    y = TABLES_sin(v->modX); break;
        case SIGNAL_RECT:   y = TABLES_rect(v->modX); break;
        case SIGNAL_SAW:    y = TABLES_saw(v->modX); break;
        case SIGNAL_NOISE:  y = TABLES_noise(); break;
        default: break;
    }

    v->modX += v->modRate;
    v->modX &= 0xFFFF;

    return y;
}

int getSignal(Voice *v) {
    int x;
    int y = 0;

    if (v->instr == 0)
        return 0;

    x = v->x;

    switch (v->instr->signal) {
        case SIGNAL_ZERO:   y = 0; break;
        case SIGNAL_SIN:    y = TABLES_sin(x); break;
        case SIGNAL_RECT:   y = TABLES_rect(x); break;
        case SIGNAL_SAW:    y = TABLES_saw(x); break;
        case SIGNAL_NOISE:  y = TABLES_noise(); break;
        default: break;
    }

    x = getModulation(v) * v->modScale; // -2^30 to +2^30
    x >>= 16; // -2^14 to +2^14;
    x *= v->rate;
    x >>= 14 + 4; // Modulation is max 1/16 of freq
    //printf("[%d]\n", x);

    v->x += v->rate + x;
    v->x &= 0xFFFF;

    return y;
}

int sampleVoice(Voice* v) {
    int volume = getVolume(v);
    int y = getSignal(v);

    // y = -2^20 to +2^20
    y = y * volume;
    // y = -2^30 to +2^30
    return y;
}

int sampleVoices(Voice* voice, int count) {
    int i;
    int y = 0;

    for (i = 0; i < count; i++)
        y += sampleVoice(&voice[i]) / count;
    // y = -2^30 to +2^30
    y = y >> 15;
    // y = -2^15 to +2^15
    return y;
}
