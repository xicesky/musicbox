
#ifndef AUDIOIO_H
#define AUDIOIO_H

int AUDIOIO_init(int sampleRate, double (*produceSampleCallback)());
void AUDIOIO_setAudioActive(int audioActive);
void AUDIOIO_mainLoop();
void AUDIOIO_exit();

#endif
