
#ifndef TABLES_H
#define TABLES_H

void TABLES_init();
int TABLES_sin(int x);
int TABLES_nsin(int x);
int TABLES_rect(int x);
int TABLES_saw(int x);
int TABLES_noise();
void TABLES_test();

int TABLES_calcRate(double f, double fs);
int TABLES_rateForNote(int note, double fs);

#endif
