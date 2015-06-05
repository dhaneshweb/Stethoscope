#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "mbed.h"

#define RBLENGTH 1000
#define RBWIDTH 2

class RingBuffer {

public:

RingBuffer();

int addToBuffer(int* input);
int addToBufferS(int input, int across);
int nextBuffer();

void readFirst(int* output);
int readFirstS(int across);

void readAt(int* output, int at);
int readAtS(int at, int across);

void readLast(int*output);
int readLastS(int across);

void usedFirst();
void usedLast();

int numberStored();

protected:

int16_t Buffer[RBLENGTH][RBWIDTH];

int Start;
int Finish;
int Samples;

void sortFinishNS();
void sortStartUS();
};
#endif