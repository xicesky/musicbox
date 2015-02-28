
#include <stdlib.h>
#include <SDL.h>
//#include <stdio.h>
#include <math.h>

//#define DEBUG_TABLES_SHOW_MAXIMA
// Table size is fixed at 256

#define TABLES_SIZE 256
#define RANDTABLE_SIZE 8192 // 44100 / 8192 = ca 5 Hz remaining frequency response
#define RECT_HARMONICS 25
#define RECT_AMPLITUDE_FIX 1.1531774 // an empirical coefficient for fixing the amplitude, depends on RECT_HARMONICS
#define SAW_HARMONICS 50
#define SAW_AMPLITUDE_FIX  1.1262987 // an empirical coefficient for fixing the amplitude, depends on SAW_HARMONICS
#define AMPLITUDE 1048574.5 // 2^20 - 1.5
#define PI 3.1415926535897932384626433832795

int TABLES_sinTable[TABLES_SIZE];
int TABLES_rectTable[TABLES_SIZE];
int TABLES_sawTable[TABLES_SIZE];
int TABLES_randTable[RANDTABLE_SIZE];
int TABLES_randShift = 0;
int TABLES_randIndex = 0;

void TABLES_init() {
    int i, j;
    double maxSin = 0;
    double maxRect = 0;
    double maxSaw = 0;
    for (i = 0; i < TABLES_SIZE; i++) {
        double x = 2 * PI * ((double)i) / ((double)TABLES_SIZE);

        /***********************************************************
         * Sinus
         ***********************************************************/
        double y = sin(x) * AMPLITUDE;
        TABLES_sinTable[i] = floor(y);

        if (y < 0)
            y = -y;
        if (y > maxSin)
            maxSin = y;

        /***********************************************************
         * Rectangle
         ***********************************************************/
        y = 0;
        for (j = 0; j < RECT_HARMONICS; j++) {
            y += sin(x * (double)(2*j+1)) * AMPLITUDE / (double)(2*j+1);
        }
        y = 4.0 * y / PI / RECT_AMPLITUDE_FIX; // 1.126 is an empirical coefficient for fixing the amplitude
        TABLES_rectTable[i] = floor(y);

        if (y < 0)
            y = -y;
        if (y > maxRect)
            maxRect = y;

        /***********************************************************
         * Sawtooth
         ***********************************************************/
        y = 0;
        for (j = 0; j < SAW_HARMONICS; j++) {
            if (j & 1)
                y -= sin(x * (double)(j+1)) * AMPLITUDE / (double)(j+1);
            else
                y += sin(x * (double)(j+1)) * AMPLITUDE / (double)(j+1);
        }
        y = -2.0 * y / PI / SAW_AMPLITUDE_FIX; // 1.120 is an empirical coefficient for fixing the amplitude
        TABLES_sawTable[i] = floor(y);

        if (y < 0)
            y = -y;
        if (y > maxSaw)
            maxSaw = y;

    }

    i = RAND_MAX;
    while (i < AMPLITUDE) {
        TABLES_randShift++;
        i = i << 1;
    }
    TABLES_randShift++;

    for (i=0; i < RANDTABLE_SIZE; i++)
        TABLES_randTable[i] = ((rand() << TABLES_randShift) & 0x1FFFFF) - 0x100000;

#ifdef DEBUG_TABLES_SHOW_MAXIMA
    printf("Finished TABLES_init\n");
    printf(" -> Max. amplitudes of:\n");
    printf("        Sin  = %f\n", maxSin);
    printf("        Rect = %f\n", maxRect);
    printf("        Saw  = %f\n", maxSaw);
    printf(" -> RAND_MAX = %d      so shifting left by %d+1\n", RAND_MAX, TABLES_randShift-1);
    printf(" -> Rand table is %d samples, leaving response at ca %3.1f Hz (@ fs=44.1 kHz)\n", RANDTABLE_SIZE, 44100.0 / RANDTABLE_SIZE);
#endif
}

int TABLES_lookupLinear(int x, int* table) {
    x = x & 0xFFFF;
    int p2 = x & 0xFF;
    int p1 = 0x100 - p2;
    x = x >> 8;
    int x2 = (x+1) & 0xFF;

    int s1 = table[x];
    int s2 = table[x2];

    // Linear interp
    int y = (s1 * p1 + s2 * p2) >> 8;

    return y;
}

// Period is 65536, Amplitude ~ 2^20
int TABLES_sin(int x) {
    return TABLES_lookupLinear(x, TABLES_sinTable);
}

int TABLES_nsin(int x) {
    double i;
    i = ((double)x) / (256.0 * TABLES_SIZE);
    i = sin(i * 2 * PI) * AMPLITUDE;
    return floor(i);
}

int TABLES_rect(int x) {
    return TABLES_lookupLinear(x, TABLES_rectTable);
}

int TABLES_saw(int x) {
    return TABLES_lookupLinear(x, TABLES_sawTable);
}

int TABLES_noise() {
    TABLES_randIndex = (TABLES_randIndex + 1) % RANDTABLE_SIZE;
    return TABLES_randTable[TABLES_randIndex];
}

void TABLES_printTable(char *name, int *table) {
    int i;
    printf("Table: %s\n", name);
    for (i = 0; i < TABLES_SIZE; i++) {
        printf("\t[%3d] %d\n", i, table[i]);
    }
}

void TABLES_test() {
    int i = 0;
    int p = 983;
    int s = 0;
    for (;i<100;i++) {
        int x = (i*p);
        x = x * x;
        int y1 = TABLES_sin(x);
        int y2 = TABLES_nsin(x);
        int d = y1 - y2;
        if (d<0) d = -d;
        printf("testSin: delta=%d          (%d, %d): %d\t%d\n", d, i, x, y1, y2);
        s += d;
    }
    printf("testSin: total delta = %d", s);
    printf("\n\n");
    TABLES_printTable("sin", TABLES_sinTable);
    printf("\n\n");
    TABLES_printTable("rect", TABLES_rectTable);
    printf("\n\n");
    TABLES_printTable("saw", TABLES_sawTable);
    printf("\n\n");
}

int TABLES_calcRate(double f, double fs) {
    f *= 65536.0 / fs;
    return floor(f+0.5);
}

double fbase = 220.0;

int TABLES_rateForNote(int note, double fs) {
    double f = fbase * pow(2.0, ((double)note)/12.0);
    return TABLES_calcRate(f, fs);
}
