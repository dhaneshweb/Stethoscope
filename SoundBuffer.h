#ifndef __SOUNDBUFFER_H__
#define __SOUNDBUFFER_H__

#include "mbed.h"

class SoundBuffer
{
public:
	SoundBuffer();

	void write_sound_input(int val);
	void get_sound_output(int * outBuffer, int count);

	void start_save(FILE * fp);

	void start() { isRecording = true; }
	void stop() { isRecording = false; }
private:
	static const int readLength = 4096;
	static const int bufferMask = readLength - 1;

	int circularBuffer[readLength];

	volatile bool isRecording;
	volatile int playbackTheta;
	volatile int saveTheta;

	int readPointer;
	int writePointer;
	int savePointer;
};

#endif
